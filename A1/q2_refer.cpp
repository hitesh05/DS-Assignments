#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

int INF = (int)1e7;

void printMatrix(int *mat, int n, int proc)
{
	string s;
	for (int i = 0; i < n; i++)
	{
		// s = "";
		for (int j = 0; j < n; j++)
		{
			s += to_string(mat[i * n + j]) + " ";
		}
		// cout << "proc: " << proc << " " << s << endl;
		s += "\n";
	}
	s = "proc: " + to_string(proc) + " \n" + s;
	cout << s << endl;
}

int Owner(int k, int p, int n)
{
	if (k < (p - 1) * (n / p))
		return k / (n / p);
	else
		return (p - 1);
}

void send_col(int my_rank, int n, int p, int *mat, int k, MPI_Comm comm, int *col_k_recv)
{
	int row_start = my_rank * (n / p);
	int row_end = (my_rank + 1) * (n / p) - 1;
	if (my_rank == p - 1)
	{
		row_start = (p - 1) * (n / p);
		row_end = n - 1;
	}
	int *to_recv = new int[n];
	for (int i = 0; i < p - 1; i++)
		to_recv[i] = n / p;
	to_recv[p - 1] = n - ((n / p) * (p - 1));
	int *disp = new int[p];
	disp[0] = 0;
	for (int i = 1; i < p; i++)
		disp[i] = disp[i - 1] + to_recv[i - 1];
	if (my_rank != p - 1)
	{
		int s = n / p;
		int *col_k = new int[s];
		for (int i = row_start; i <= row_end; i++)
			col_k[i - row_start] = mat[i * n + k];
		MPI_Allgatherv(col_k, s, MPI_INT, col_k_recv, to_recv, disp, MPI_INT, comm);
	}
	else
	{
		int s = n - ((n / p) * (p - 1));
		int *col_k = new int[s];
		for (int i = row_start; i <= row_end; i++)
			col_k[i - row_start] = mat[i * n + k];
		MPI_Allgatherv(col_k, s, MPI_INT, col_k_recv, to_recv, disp, MPI_INT, comm);
	}
}

void Floyd(int my_rank, int n, int *mat, MPI_Comm comm, int p)
{
	int row_start = my_rank * (n / p);
	int row_end = (my_rank + 1) * (n / p) - 1;
	if (my_rank == p - 1)
	{
		row_start = (p - 1) * (n / p);
		row_end = n - 1;
	}
	int *row_k = (int *)malloc(n * sizeof(int));
	for (int k = 0; k < n; k++)
	{
		// Get Kth row and coloumn
		int root = Owner(k, p, n);
		if (my_rank == root)
		{
			for (int j = 0; j < n; j++)
				row_k[j] = mat[k * n + j];
		}
		MPI_Bcast(row_k, n, MPI_INT, root, comm);
		int *col_k_recv = new int[n];
		send_col(my_rank, n, p, mat, k, comm, col_k_recv);
		for (int i = row_start; i <= row_end; i++)
		{
			for (int j = 0; j < n; j++)
			{
				mat[i * n + j] = min(mat[i * n + j], col_k_recv[i] + row_k[j]);
			}
		}
	}
	int mat2[n * n];
	MPI_Reduce(mat, mat2, n * n, MPI_INT, MPI_MIN, 0, comm);
	if (my_rank == 0)
	{
		printMatrix(mat2, n, my_rank);
	}
}

int main(int argc, char **argv)
{
	int p, my_rank;
	MPI_Comm comm;

	MPI_Init(&argc, &argv);
	comm = MPI_COMM_WORLD;
	MPI_Comm_size(comm, &p);
	MPI_Comm_rank(comm, &my_rank);

	int n;
	if (my_rank == 0)
	{
		cout << "Enter n: ";
		cin >> n;
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, comm);
	int *local_mat = new int[n * n];

	if (my_rank == 0)
	{
		cout << "Enter the matrix: " << endl;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				cin >> local_mat[i * n + j];
				if (local_mat[i * n + j] == -1)
					local_mat[i * n + j] = INF;
			}
		}
	}
	MPI_Bcast(local_mat, n * n, MPI_INT, 0, comm);
	Floyd(my_rank, n, local_mat, comm, p);
	MPI_Finalize();
}