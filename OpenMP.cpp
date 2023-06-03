#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <chrono>

using namespace std;

#define SIZE 2000
#define MAX_THREADS 4

void multiply_matrices(int** matrix1, int** matrix2, int** result_matrix, int size) {
#pragma omp parallel for num_threads(MAX_THREADS)
	for (int i = 0; i < size; i++) 
	{
		for (int j = 0; j < size; j++)
		{
			result_matrix[i][j] = 0;
			for (int k = 0; k < size; k++)
			{
				result_matrix[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
}

int main() {
	int** matrix1 = new int* [SIZE];
	int** matrix2 = new int* [SIZE];
	int** result_matrix = new int* [SIZE];

	for (int i = 0; i < SIZE; i++)
	{
		matrix1[i] = new int[SIZE];
		matrix2[i] = new int[SIZE];
		result_matrix[i] = new int[SIZE];
	}

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			matrix1[i][j] = rand() % 10 + 1;
			matrix2[i][j] = rand() % 10 + 1;
		}
	}

	auto start_time = chrono::steady_clock::now();

	multiply_matrices(matrix1, matrix2, result_matrix, SIZE);

	auto end_time = chrono::steady_clock::now();

	auto time_taken = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
	cout << "Time taken: " << time_taken << " milliseconds" << endl;

	return 0;
}