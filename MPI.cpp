#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

const int MATRIX_SIZE = 2000;

int main(int argc, char** argv) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	vector < vector < int>> a(MATRIX_SIZE, vector<int>(MATRIX_SIZE, 1));
	vector < vector < int>> b(MATRIX_SIZE, vector<int>(MATRIX_SIZE, 1));
	vector < vector < int>> result(MATRIX_SIZE, vector<int>(MATRIX_SIZE, 0));

	if (rank == 0) {
		auto start_time = chrono::steady_clock::now();

		int rows_per_proc = MATRIX_SIZE / size;
		for (int i = 1; i < size; i++) {
			MPI_Send(&rows_per_proc, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&b[0][0], MATRIX_SIZE * MATRIX_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&a[i * rows_per_proc][0], rows_per_proc * MATRIX_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		for (int i = 0; i < rows_per_proc; i++) {
			for (int j = 0; j < MATRIX_SIZE; j++) {
				for (int k = 0; k < MATRIX_SIZE; k++) {
					result[i][j] += a[i][k] * b[k][j];
				}
			}
		}

		for (int i = 1; i < size; i++) {
			MPI_Recv(&result[i * rows_per_proc][0], rows_per_proc * MATRIX_SIZE, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		auto end_time = chrono::steady_clock::now();
		auto time_taken = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
		cout << "Time taken: " << time_taken << " milliseconds" << endl;
	}
	else {
		int rows_per_proc;
		MPI_Recv(&rows_per_proc, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&b[0][0], MATRIX_SIZE * MATRIX_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		vector < vector < int>> a(rows_per_proc, vector<int>(MATRIX_SIZE));
		MPI_Recv(&a[0][0], rows_per_proc * MATRIX_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		vector < vector < int>> c(rows_per_proc, vector<int>(MATRIX_SIZE, 0));

		for (int i = 0; i < rows_per_proc; i++) {
			for (int j = 0; j < MATRIX_SIZE; j++) {
				for (int k = 0; k < MATRIX_SIZE; k++) {
					c[i][j] += a[i][k] * b[k][j];
				}
			}
		}

		MPI_Send(&c[0][0], rows_per_proc * MATRIX_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}