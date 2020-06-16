// iot_pc_cha_d2a.cpp : Defines the entry point for the console application.
//
#include "iostream"
#include "fstream"
#include "iomanip"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "vector"
#include "ctime"

using namespace std;

double distance(double x_i, double x_j, double y_i, double y_j)
{
	return sqrt(pow((x_i - x_j), 2) + pow((y_i - y_j), 2));
}

double h(double x_i, double x_next_node_j, double y_i, double y_next_node_j)
{
	return 0.09*pow(distance(x_i, x_next_node_j, y_i, y_next_node_j), -3);
}

int random_generator(int min, int max)
{
	int random_number;
	//srand(time(NULL));
	random_number = rand() % (1000 - 0) + 0;
	for (int i = 0; i<max - min + 1; i++)
		if (random_number >= ((1000 * i) / (max - min + 1)) && random_number <= ((1000 * (i + 1)) / (max - min + 1)))
			return i + min;
}

void print_progress_bar(int percent)
{
	string bar;
	cout << "\r" << bar;
	cout << percent << "% " << std::flush;
}

int func_next_node(int i, int n, int r, double x[], double y[])
{
	int temp_distance = 1000000;
	int n_n = 0;
	if (i == 0)
		return n_n;
	else
	{
		for (int j = 0; j < i; j++)
			if (i != j)
				if (distance(x[i], x[j], y[i], y[j]) <= r)
					if (distance(x[j], x[0], y[j], y[0]) <= temp_distance)
					{
						temp_distance = distance(x[j], x[0], y[j], y[0]);
						n_n = j;
					}
		return n_n;
	}
}

double func_get_max_index(double arr[], int size)
{
	int MaxIndex;
	double temp_max = 0;
	for (int i = 0; i<size; i++)
		if (arr[i]>temp_max)
		{
			temp_max = arr[i];
			MaxIndex = i;
		}

	return MaxIndex;
}

double func_get_min_index(double arr[], int size)
{
	int MinIndex;
	double temp_min = 1000000000000;
	for (int i = 0; i<size; i++)
		if (arr[i]<temp_min)
		{
			temp_min = arr[i];
			MinIndex = i;
		}

	return MinIndex;
}

double func_get_max(double arr[], int size)
{
	int MaxIndex;
	double temp_max = 0;
	for (int i = 0; i<size; i++)
		if (arr[i]>temp_max)
		{
			temp_max = arr[i];
			MaxIndex = i;
		}

	return temp_max;
}

double func_get_min(double arr[], int size)
{
	int MinIndex;
	double temp_min = 1000000000000;
	for (int i = 0; i<size; i++)
		if (arr[i]<temp_min)
		{
			temp_min = arr[i];
			MinIndex = i;
		}

	return temp_min;
}

double diff(double a, double b)
{
	if (a >= b)
		return a - b;
	else
		return b - a;
}

int main(int argc, char* argv[])
{
	int cm_n = atoi(argv[1]);
	int cm_no_specs = atoi(argv[2]);
	const int n = cm_n + 1;
	const int no_specs = cm_no_specs + 1;
	const int hop = 1;
	const int r = 1;
	double noise = 1; //0.0000000001;
	double max_power = 10000;
	//double opc_constant = 0.01;
	double target_sinr[n];

	int next_node[n];
	double temp[n+no_specs];
	int pgsan[n - 1]; //min to max path-gain sorted arrey of nodes
	int channel[n];
	double p[n];
	double stsir[no_specs]; //sum of target SIRs of users on channel k

	for (int i = 0; i < n; i++)
	{
		if (i < n - 1)
			pgsan[i] = 0;
		channel[i] = 0;
	}

	double x[n];
	double y[n];
	int temp_a = 0;
	double temp_b = 0;

	ifstream rxfile;
	rxfile.open("S04_x.txt");
	while (!rxfile.eof())
	{
		rxfile >> temp_a >> temp_b;
		if (temp_a != n)
			x[temp_a] = temp_b;
		else
			x[0] = temp_b;
	}
	rxfile.close();

	temp_a = 0;
	temp_b = 0;
	ifstream ryfile;
	ryfile.open("S05_y.txt");


	while (!ryfile.eof())
	{
		ryfile >> temp_a >> temp_b;
		if (temp_a != n)
			y[temp_a] = temp_b;
		else
			y[0] = temp_b;
	}
	ryfile.close();

	int start_s = clock();

	target_sinr[0] = 0;
	for (int i = 1; i < n; i++)
	{
		if (i <= 5)
			target_sinr[i] = 0.04;
		else if (i <= 10)
			target_sinr[i] = 0.03;
		else if (i <= 15)
			target_sinr[i] = 0.02;
		else if (i <= 20)
			target_sinr[i] = 0.01;
	}

	for (int i = 0; i < n; i++)
		next_node[i] = 0;

	temp[0] = 1;
	for (int i = 1; i < n; i++)
		temp[i] = h(x[i], x[next_node[i]], y[i], y[next_node[i]]);

	for (int i = 0; i < n - 1; i++)
	{
		pgsan[i] = func_get_min_index(temp, n);
		temp[pgsan[i]] = 1;
	}

	for (int i = 1; i < n; i++)
		channel[i] = 1;


	for (int round = 0; round < n - 1; round++)
	{
		for (int k = 0; k < no_specs; k++)
			temp[k] = 0;

		for (int k = 1; k < no_specs; k++)
		{
			channel[pgsan[round]] = k;

			for (int k1 = 0; k1 < no_specs; k1++)
				stsir[k1] = 0;

			for (int k1 = 1; k1 < no_specs; k1++)
				for (int i = 1; i < n; i++)
					if (channel[i] == k1)
						stsir[k1] = stsir[k1] + target_sinr[i] / (target_sinr[i] + 1);

			for (int i = 1; i < n; i++)
			{
				if (1 - stsir[channel[i]] <= 0)
					p[i] = max_power;
				else
					p[i] = (target_sinr[i] / (h(x[i], x[next_node[i]], y[i], y[next_node[i]])*(target_sinr[i] + 1)))*(noise / (1 - stsir[channel[i]]));
			}

			for (int i = 1; i < n; i++)
				temp[k] = temp[k] + p[i];
		}

		temp[0] = (n+1)*max_power;
		channel[pgsan[round]] = func_get_min_index(temp, no_specs);

	}

	int stop_s = clock();

	// writing assigned channels to the file
	ofstream file_channels;
	file_channels.open("S06_cpp_ch.txt");
	for (int i = 1; i < n; i++)
		file_channels << i << " " << channel[i] << " " << "1" << "\n";
	file_channels.close();

	// writing execution time to file
	ofstream file_et;
	file_et.open("R04_cpp_chat.txt", std::ios::app);
	file_et << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << endl;
	file_et.close();
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}
