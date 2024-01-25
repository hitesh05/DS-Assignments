#include <bits/stdc++.h>
#include <mpi.h>

using namespace std;

bool isSafe(const vector<int> &board, int row, int col)
{
    for (int i = 0; i < row; i++)
    {
        if (board[i] == col || board[i] - i == col - row || board[i] + i == col + row) 
            return false;
    }
    return true;
}

void countNQueens(int N, int row, vector<int> &board, int &count)
{
    if (row == N)
    {
        count++;
        return;
    }
    for (int col = 0; col < N; col++)
    {
        if (isSafe(board, row, col))
        {
            board[row] = col;
            countNQueens(N, row + 1, board, count);
        }
    }
}

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    
    int size, rank;

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    int N;
    if(rank == 0){
        cout << "enter n: " << " ";
        cin >> N;
    }
    MPI_Bcast(&N,1,MPI_INT,0,comm);

    int local_count = 0;
    vector<int> board(N, 0);

    // Divide the work among processes
    for (int i = rank; i < N; i += size)
    {
        board[0] = i; // Place queen in the first row
        countNQueens(N, 1, board, local_count);
    }

    // Reduce the local counts to get the total count
    int total_count;
    MPI_Reduce(&local_count, &total_count, 1, MPI_INT, MPI_SUM, 0, comm);

    if (rank == 0)
    {
        cout << "Answer: " << total_count << endl;
    }

    MPI_Finalize();
    return 0;
}