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
	const int n = cm_n+1;
	const int no_specs = cm_no_specs+1;
	const int hop = 1;
	const int r = 1;
	double noise = 1; //0.0000000001;
	double max_power = 10000;
	const int round_bound = 20; //minimun amount is 1 where we have just one round
	double target_sinr[n];

	double sinr[round_bound][n][no_specs];
	double sum_sinr[round_bound];
	int sinr_received_counter = 0;

	double p[round_bound][n][no_specs];
	double sum_power[round_bound];

	double I[round_bound][n][no_specs];

	double outage_ratio[round_bound];

	int max_powered_node;
	int min_interfered_channel;

	int round_loop_index_counter = 0;
	int round_loop_flag = 0;

	int next_node[n];
	double stsir[no_specs];
	int chflag[n];
	int round;
	int channel[round_bound][n];
	double temp[n+no_specs];

	double max_sum_sinr;
	double min_sum_power;
	int round_max_sum_sinr;
	int round_min_sum_power;
	double min_outage_ratio;

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

	for (int i = 0; i<n; i++)
		//next_node[i] = func_next_node(i, n, r, x, y);
		next_node[i] = 0;

	round = 0;

	//while (sinr_received_counter != no_trans && round<round_bound && round_loop_flag != 1)
	while (round < round_bound && round_loop_flag != 1)
	{
		//double prog_bar_var=100*round/(round_bound-1);
		//print_progress_bar(prog_bar_var);

		// first assignment of channel and power to all nodes
		if (round == 0)
			for (int i = 1; i < n; i++)
			{
				channel[round][i] = 1;// temp_nu%no_specs;
				chflag[i] = 0;
			}
		else
		{
			// finding max_interfered_node

			for (int i = 0; i<n+no_specs; i++)
				temp[i] = 0;

			for (int i = 1; i < n; i++)
				if (chflag[i] == 0)
					temp[i] = p[round - 1][i][channel[round - 1][i]];
				else
					temp[i] = 0;
			max_powered_node = func_get_max_index(temp, n);

			// finding min_interfered_channel
			for (int k = 1; k<no_specs; k++)
				temp[k] = I[round - 1][max_powered_node][k];
			temp[0]=(n+1)*max_power;
			min_interfered_channel = func_get_min_index(temp, no_specs);

			for (int i = 1; i<n; i++)
			{
				// assigning channel and power to max_interfered_node
				if (i == max_powered_node)
					channel[round][i] = min_interfered_channel;

				// assigning channel and power to other nodes
				else
					channel[round][i] = channel[round - 1][i];
			}
		}

		for (int k = 1; k < no_specs; k++)
			stsir[k] = 0;

		for (int k = 1; k < no_specs; k++)
			for (int i = 1; i < n; i++)
				if (channel[round][i] == k)
					stsir[k] = stsir[k] + target_sinr[i] / (target_sinr[i] + 1);

		// power control procedure
		for (int i = 1; i < n;i++)
			for (int k = 1; k < no_specs; k++)
			{
				if (k == channel[round][i] && 1 - stsir[k] > 0)
					p[round][i][k] = min(max_power, (target_sinr[i] / (h(x[i], x[next_node[i]], y[i], y[next_node[i]])*(target_sinr[i] + 1)))*(noise / (1 - stsir[k])));
				if (k == channel[round][i] && 1 - stsir[k] <= 0)
					p[round][i][k] = max_power;
				if (k != channel[round][i])
					p[round][i][k] = 0;
			}

		for (int i = 1; i < n; i++)
			for (int k = 1; k < no_specs; k++)
			{
				I[round][i][k] = 0;
				for (int j = 1; j < n; j++)
					if (j != i && j != next_node[i])
						I[round][i][k] = I[round][i][k] + p[round][j][k] * h(x[j], x[next_node[i]], y[j], y[next_node[i]]);
				I[round][i][k] = I[round][i][k] + noise;
				sinr[round][i][k] = (p[round][i][k] * h(x[i], x[next_node[i]], y[i], y[next_node[i]])) / I[round][i][k];
			}
	
		// final values of each power control procedure
		sinr_received_counter = 0;
		sum_sinr[round] = 0;
		sum_power[round] = 0;
		for (int i = 1; i<n; i++)
		{
			if (diff(sinr[round][i][channel[round][i]],target_sinr[i])<0.00001)
				sinr_received_counter++;

			for (int k = 1; k<no_specs; k++)
			{
				sum_sinr[round] = sum_sinr[round] + sinr[round][i][k];
				sum_power[round] = sum_power[round] + p[round][i][k];
			}
		}
		outage_ratio[round] = 1 - (sinr_received_counter / (double)(n-1));

		if (round > 0)
		{
			if (sum_power[round] > sum_power[round - 1])
				for (int i = 1; i < n; i++)
				{
					channel[round][i] = channel[round - 1][i];
					for (int k = 1; k < no_specs; k++)
					{
						I[round][i][k] = I[round - 1][i][k];
						p[round][i][k] = p[round - 1][i][k];
						sum_power[round] = sum_power[round - 1];
						sinr[round][i][k] = sinr[round - 1][i][k];
					}
				}
			else
				chflag[max_powered_node] = 1;
		}

		round_loop_flag = 0;

		for (int r = 0; r < round; r++)
		{
			round_loop_index_counter = 0;
			for (int i = 1; i < n; i++)
				if (channel[round][i] == channel[r][i])
					round_loop_index_counter++;
			if (round_loop_index_counter == n-1)
				round_loop_flag = 1;
		}

		round++;
	}

	int stop_s = clock();

	// preparing results for colormap picture /////////////////////////////////////////////////////////////////////////////////////////////
	
	// writing Sum of Power to file
	ofstream file_SoP;
	file_SoP.open("R01_game_SoP.txt", std::ios::app);
	file_SoP << sum_power[round-1] << "\n";
	file_SoP.close();

	// writing Sum of SINR to file
	ofstream file_SoS;
	file_SoS.open("R02_game_SoS.txt", std::ios::app);
	file_SoS << sum_sinr[round-1] << "\n";
	file_SoS.close();

	// writing execution time to file
	ofstream file_et;
	file_et.open("R04_game_t.txt", std::ios::app);
	file_et << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << endl;
	file_et.close();


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}
