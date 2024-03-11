#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

vector<int> dx = {-1, 1, 0, 0, 1, -1, 1, -1};
vector<int> dy = {0, 0, -1, 1, 1, -1, -1, 1};

int get_row_start(int r, int n, int s)
{
    if (r != s - 1)
        return int(r * (n / s));
    else
        return int((s - 1) * (n / s));
}

int get_row_end(int r, int n, int s)
{
    if (r != s - 1)
        return int((r + 1) * (n / s) - 1);
    else
        return n - 1;
}

void printMatrix(int *mat, int n, int m, int proc)
{
    int i = 0;
    while (i < n)
    {
        int j = 0;
        while (j < m)
        {
            cout << mat[i * m + j] << " ";
            j++;
        }
        cout << endl;
        i++;
    }
}

void send_mat(int my_rank, int n, int s, int *mat, int m, MPI_Comm comm, int *final_cur_col)
{
    int row_s = get_row_start(my_rank, n, s);
    int row_e = get_row_end(my_rank, n, s);

    int *to_recv = new int[n];
    int *disp = new int[n];
    int iter = n / s;

    for (int i = 0; i < s; i++)
        to_recv[i] = iter * m;
    to_recv[s - 1] = (n - (iter * (s - 1))) * m;

    for (int i = 0; i < s; i++)
    {
        if (i != 0)
            disp[i] = disp[i - 1] + to_recv[i - 1];
        else
            disp[0] = 0;
    }

    int x = -1;
    if (my_rank != s - 1)
        x = iter * m;
    else
        x = (n - (iter * (s - 1))) * m;

    MPI_Allgatherv(mat + row_s * m, x, MPI_INT, final_cur_col, to_recv, disp, MPI_INT, comm);
    delete[] to_recv;
    delete[] disp;
}

void send_bottom_row_to_nxt_proc(int my_rank, int n, int s, int m, int *mat, int row_s, int row_e, MPI_Comm comm)
{
    if (my_rank != s - 1)
        MPI_Send(mat + row_e * m, m, MPI_INT, my_rank + 1, 0, comm);
    if (my_rank)
        MPI_Recv(mat + (row_s - 1) * m, m, MPI_INT, my_rank - 1, 0, comm, MPI_STATUS_IGNORE);
}

void send_top_row_to_prev_proc(int my_rank, int n, int s, int m, int *mat, int row_s, int row_e, MPI_Comm comm)
{
    if (my_rank != s - 1){
        int ind = (row_e + 1) * m;
        MPI_Recv(mat + ind, m, MPI_INT, my_rank + 1, 0, comm, MPI_STATUS_IGNORE);
    }
    if (my_rank)
        MPI_Send(mat + row_s * m, m, MPI_INT, my_rank - 1, 0, comm);
}

void game(int *mat, int n, int m, int t, int my_rank, int s, MPI_Comm comm, int *mat2, int row_s, int row_e, bool is_print)
{
    std::copy(mat, mat + m * n, mat2);

    for (int i = row_s; i < row_e + 1; i++)
    {
        for (int j = 0; j < m; j++)
        {
            int count = 0;
            int z = 0;
            while (z < 8)
            {
                int x = i + dx.at(z);
                int y = j + dy.at(z);
                if (x > -1 && x <= n - 1 && y > -1 && y <= m - 1 && mat[x * m + y] == 1)
                    count += 1;
                z++;
            }
            int ind = i * m + j;
            if (count < 2)
                mat2[ind] = 0;
            else if (mat[ind] == 1 && count > 3)
                mat2[ind] = 0;
            else if (mat[ind] == 0 && count == 3)
                mat2[ind] = 1;
            else if (mat[ind] == 1 && (count == 2 || count == 3))
                mat2[ind] = 1;
            else
                mat2[ind] = 0;
        }
    }
    std::copy(mat2, mat2 + m * n, mat);

    MPI_Barrier(comm);

    if (s <= n)
    {
        send_bottom_row_to_nxt_proc(my_rank, n, s, m, mat, row_s, row_e, comm);

        send_top_row_to_prev_proc(my_rank, n, s, m, mat, row_s, row_e, comm);
    }

    int mat3[n * m];
    send_mat(my_rank, n, s, mat, m, comm, mat3);
    if (!my_rank && is_print)
        printMatrix(mat3, n, m, my_rank);
}

void game_of_life(int *mat, int n, int m, int t, int my_rank, int s, MPI_Comm comm)
{
    // get row start and end
    int row_s = get_row_start(my_rank, n, s);
    int row_e = get_row_end(my_rank, n, s);
    int size = n * m;
    int *mat2 = new int[size];

    int num_iterations = 0;
    while (num_iterations < t)
    {
        bool is_print = false;
        if (num_iterations == t - 1)
            is_print = true;

        game(mat, n, m, t, my_rank, s, comm, mat2, row_s, row_e, is_print);
        num_iterations++;
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;

    int size, rank;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    int n, m, t;
    if (!rank)
    {
        cin >> n >> m >> t;
    }
    
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int mat_size = n * m;
    int *matrix = new int[mat_size];
    if (!rank)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                int ind = i * m + j;
                cin >> matrix[ind];
            }
        }
    }
    MPI_Bcast(matrix, mat_size, MPI_INT, 0, comm);
    game_of_life(matrix, n, m, t, rank, size, comm);
    delete[] matrix;

    MPI_Finalize();
    return 0;
}