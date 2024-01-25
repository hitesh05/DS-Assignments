#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

int get_row_start(int r, int n, int s)
{
    if (r == s - 1)
        return int((s - 1) * (n / s));
    else
        return int(r * (n / s));
}

int get_row_end(int r, int n, int s)
{
    if (r == s - 1)
        return n - 1;
    else
        return int((r + 1) * (n / s) - 1)
}

void printMatrix(int *mat, int n, int proc)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << mat[i * n + j] << " ";
        }
        cout << endl;
    }
}

int Owner(int k, int s, int n)
{
    int tot = s - 1;
    int iter = n / p;
    if (k < tot * iter)
        return k / iter;
    else
        return tot;
}

void recv_col(int my_rank, int n, int s, int *mat, int k, MPI_Comm comm, int *final_cur_col)
{
    int row_s = get_row_start(my_rank, n, s);
    int row_e = get_row_end(my_rank, n, s);

    int *to_recv = new int[n];
    int *disp = new int[n];
    int iter = n / s;

    for (int i = 0; i < s; i++)
        to_recv[i] = iter;
    to_recv[s - 1] = n - (iter * (s - 1));

    for (int i = 0; i < s; i++)
    {
        if (i != 0)
            disp[i] = disp[i - 1] + to_recv[i - 1];
        else
            disp[0] = 0;
    }

    int x = -1;
    if (my_rank != s - 1)
        x = iter;
    else
        x = n - (iter * (s - 1));

    int *col_k = new int[x];
    for (int i = row_s; i <= row_e; i++)
        col_k[i - row_s] = mat[i * n + k];

    MPI_Allgatherv(col_k, x, MPI_INT, final_cur_col, to_recv, disp, MPI_INT, comm);
    delete[] to_recv;
    delete[] disp;
    delete[] col_k
}

void Algo(int my_rank, int n, int *mat, int s, MPI_Comm comm)
{
    int row_s = get_row_start(my_rank, n, s);
    int row_e = get_row_end(my_rank, n, s);

    int *cur_row = new int[n];
    int k = 0;
    while (k < n)
    {
        int cur_proc = Owner(k, s, n);
        if (cur_proc == my_rank)
            std::copy(mat + k * n, mat + k * n + n, cur_row);

        MPI_Bcast(cur_row, n, MPI_INT, cur_proc, comm);
        int *final_cur_col = new int[n];
        recv_col(my_rank, n, s, mat, k, comm, final_cur_col);

        for (int i = row_s; i <= row_e; i++)
        {
            for (int j = 0; j < n; j++)
            {
                mat[i * n + j] = min(mat[i * n + j], final_cur_col[i] + cur_row[j]);
            }
        }
        k++;
    }
    int mat2 = new int[n * n];
    MPI_Reduce(mat, mat2, n * n, MPI_INT, MPI_MIN, 0, comm);
    if (!my_rank)
        printMatrix(mat2, n, my_rank);

    delete[] cur_row;
    delete[] final_cur_col;
    delete[] mat2;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;

    int size, rank;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    int N;
    if (rank == 0)
    {
        cout << "enter N: "
             << " ";
        cin >> N;
    }

    int mat_size = n * n;
    int *matrix = new int[mat_size];
    if (!rank)
    {
        cout << "matrix: "
             << "\n";
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                int x;
                cin >> x;
                int ind = i * N + j;
                if (x != -1)
                    matrix[ind] = x;
                else
                    matrix[ind] = int(1e7);
            }
        }
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, comm);
    MPI_Bcast(matrix, mat_size, MPI_INT, 0, comm);

    Algo(rank, n, matrix, size, comm);
    MPI_Finalize();
    delete[] matrix;

    return 0;
}