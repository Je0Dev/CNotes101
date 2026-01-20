#include <stdio.h>

// N is the size of the 2D array (N*N)
#define N 9

/* Function to print the Sudoku board */
void printBoard(int board[N][N]) {
    printf("-------------------------\n");
    for (int r = 0; r < N; r++) {
        printf("| ");
        for (int d = 0; d < N; d++) {
            printf("%d ", board[r][d]);
            if ((d + 1) % 3 == 0) {
                printf("| ");
            }
        }
        printf("\n");
        if ((r + 1) % 3 == 0) {
            printf("-------------------------\n");
        }
    }
}

/* Check if a number is already in the row */
int isInRow(int board[N][N], int row, int num) {
    for (int col = 0; col < N; col++) {
        if (board[row][col] == num) {
            return 1;
        }
    }
    return 0;
}

/* Check if a number is already in the col */
int isInCol(int board[N][N], int col, int num) {
    for (int row = 0; row < N; row++) {
        if (board[row][col] == num) {
            return 1;
        }
    }
    return 0;
}

/* Check if a number is already in the 3x3 box */
int isInBox(int board[N][N], int boxStartRow, int boxStartCol, int num) {
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (board[row + boxStartRow][col + boxStartCol] == num) {
                return 1;
            }
        }
    }
    return 0;
}

/* Combined safety check */
int isSafe(int board[N][N], int row, int col, int num) {
    return !isInRow(board, row, num) &&
           !isInCol(board, col, num) &&
           !isInBox(board, row - row % 3, col - col % 3, num) &&
           board[row][col] == 0;
}

/* Find the next empty cell (value 0) */
int findEmptyCell(int board[N][N], int *row, int *col) {
    for (*row = 0; *row < N; (*row)++) {
        for (*col = 0; *col < N; (*col)++) {
            if (board[*row][*col] == 0) {
                return 1; // Found
            }
        }
    }
    return 0; // Not found (puzzle is full)
}

/* The main backtracking solver function */
int solveSudoku(int board[N][N]) {
    int row, col;

    // 1. If no empty cell is left, we are done
    if (!findEmptyCell(board, &row, &col)) {
        return 1; // Solved!
    }

    // 2. Try numbers 1 through 9
    for (int num = 1; num <= 9; num++) {
        
        // 3. Check if it's safe to place 'num' here
        if (isSafe(board, row, col, num)) {
            
            // 4. Make the move
            board[row][col] = num;

            // 5. Recursively call solve for the next cell
            if (solveSudoku(board)) {
                return 1; // Success!
            }

            // 6. Backtrack: Move failed, reset the cell
            board[row][col] = 0;
        }
    }
    
    // 7. If all numbers failed, trigger backtracking
    return 0; 
}

int main() {
    // 0 represents an empty cell
    int board[N][N] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    printf("Unsolved Board:\n");
    printBoard(board);

    if (solveSudoku(board)) {
        printf("\nSolved Board:\n");
        printBoard(board);
    } else {
        printf("\nNo solution exists for this board.\n");
    }

    return 0;
}
