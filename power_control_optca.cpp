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
	int cm_n=atoi(argv[1]);
	int cm_no_specs=atoi(argv[2]);
	double cm_target_sinr=atof(argv[3]);
	const int n = cm_n+1;
	const int no_specs = cm_no_specs+1;
	const int hop = 1;
	const int r = 1;
	double noise = 0.1; //0.0000000001;
	double max_power = 10000;
	//double opc_constant = 0.01;
	double target_sinr = cm_target_sinr; //0.05; //4;
	const int iteration_bound = 500; //minimun amount is 1 where we have just one iteration

	double sinr[iteration_bound][n][no_specs];
	double sum_sinr;
	int sinr_received_counter = 0;

	double p[iteration_bound][n][no_specs];
	double sum_power;

	double I[iteration_bound][n][no_specs];

	double outage_ratio;

	int iteration_loop_index_counter = 0;
	int iteration_loop_flag = 0;

	int next_node[n];
	double R[iteration_bound][n][no_specs];
	int iteration = 0;
	double temp[n];

	int channel[n];
	for(int i=0; i<n; i++)
		channel[i]=0;
		
	double x[n];
	double y[n];
	int temp_a = 0;
	int temp_c = 0;
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

	temp_a = 0;
	temp_b = 0;
	ifstream chfile;
	chfile.open("S06_opt_ch.txt");


	while (!chfile.eof())
	{
		chfile >> temp_a >> temp_c >> temp_b;
		if (temp_b > 0.9)
			channel[temp_a] = temp_c;
	}
	chfile.close();


	for (int i = 0; i < n; i++)
		//next_node[i] = func_next_node(i, n, r, x, y);
		next_node[i] = 0;
	
	for (int i = 1; i < n; i++)
	{
		for (int k = 0; k < no_specs; k++)
			p[iteration][i][k] = 0;
		p[iteration][i][channel[i]] = max_power;
	}

	iteration_loop_flag = 0;

	// power control itterations
	while (iteration < iteration_bound && iteration_loop_flag != 1)
	{
		if (iteration != 0)
			for (int i = 1; i < n; i++)
			{
				for (int k = 0; k < no_specs; k++)
					p[iteration][i][k] = 0;
				//p[iteration][i][channel[round][i]] = target_sinr*(p[iteration - 1][i][channel[round][i]] / sinr[iteration - 1][i][channel[round][i]]);
				p[iteration][i][channel[i]] = min(max_power, target_sinr*(p[iteration - 1][i][channel[i]] / sinr[iteration - 1][i][channel[i]]));
			}

		for (int i = 1; i < n; i++)
			for (int k = 0; k < no_specs; k++)
			{
				I[iteration][i][k] = 0;
				for (int j = 1; j < n; j++)
					if (j != i && j != next_node[i])
						I[iteration][i][k] = I[iteration][i][k] + p[iteration][j][k] * h(x[j], x[next_node[i]], y[j], y[next_node[i]]);
				I[iteration][i][k] = I[iteration][i][k] + noise;
				sinr[iteration][i][k] = (p[iteration][i][k] * h(x[i], x[next_node[i]], y[i], y[next_node[i]])) / I[iteration][i][k];
				R[iteration][i][k] = I[iteration][i][k] / h(x[i], x[next_node[i]], y[i], y[next_node[i]]);
			}

		iteration++;

		iteration_loop_index_counter = 0;

		if (iteration > 5)
		{
			for (int it = iteration - 2; it >= iteration - 5; it--)
				for (int i = 1; i < n; i++)
					if (diff(sinr[iteration - 1][i][channel[i]], sinr[it][i][channel[i]]) == 0)
						iteration_loop_index_counter++;
			if (iteration_loop_index_counter == 4 * (n - 1))
				iteration_loop_flag = 1;
		}
	}

	// final values of each power control procedure
	sinr_received_counter = 0;
	sum_sinr = 0;
	sum_power = 0;
	for (int i = 1; i < n; i++)
		for (int k = 0; k < no_specs; k++)
		{
			sum_sinr = sum_sinr + sinr[iteration - 1][i][k];
			sum_power = sum_power + p[iteration - 1][i][k];
		}
	outage_ratio = 1 - (sinr_received_counter / (double)(n - 1));

	// writing Sum of Power to file
	ofstream file_SoP;
	file_SoP.open("R01_opt+cpp_SoP.txt", std::ios::app);
	file_SoP<<sum_power<<"\n";
	file_SoP.close();

	// writing Sum of SINR to file
	ofstream file_SoS;
	file_SoS.open("R02_opt_SoS.txt", std::ios::app);
	file_SoS<<sum_sinr<<"\n";
	file_SoS.close();

	// reporting results 

	/*cout << fixed << setprecision(15);
	cout << "#" << no_specs << "_" << n << "_" << 1 << " " << max_sum_sinr << " " << min_sum_power << " " << min_outage_ratio << " " << no_trans << "\n";*/

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}
