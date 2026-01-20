#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 50
#define MAX_COLS 50

char maze[MAX_ROWS][MAX_COLS];
int rows = 0, cols = 0;
int start_x, start_y;

// Function to load the maze from a file
int load_maze(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Error opening maze file");
        return 0;
    }

    rows = 0;
    while (fgets(maze[rows], MAX_COLS, fp) && rows < MAX_ROWS) {
        // Find the 'S' and store its position
        for (int c = 0; maze[rows][c] != '\0'; c++) {
            if (maze[rows][c] == 'S') {
                start_x = c;
                start_y = rows;
            }
            // Find max column width
            if (rows == 0) {
                cols++;
            }
        }
        // Remove newline
        maze[rows][strcspn(maze[rows], "\n")] = 0;
        rows++;
    }
    fclose(fp);
    cols--; // Adjust for newline/null
    return 1;
}

// Function to print the maze
void print_maze() {
    for (int r = 0; r < rows; r++) {
        printf("%s\n", maze[r]);
    }
}

// The recursive solving function (Depth-First Search)
int solve(int y, int x) {
    // 1. Base Case: Found the end
    if (maze[y][x] == 'E') {
        return 1;
    }

    // 2. Base Case: Hit a wall or already visited
    if (maze[y][x] == '#' || maze[y][x] == '.') {
        return 0;
    }
    
    // 3. Mark the path
    //    Don't overwrite 'S'
    if(maze[y][x] != 'S') {
        maze[y][x] = '.';
    }

    // 4. Recursive Step: Try all 4 directions
    if (solve(y, x + 1)) return 1; // Right
    if (solve(y, x - 1)) return 1; // Left
    if (solve(y + 1, x)) return 1; // Down
    if (solve(y - 1, x)) return 1; // Up

    // 5. Backtrack: This is a dead end
    if(maze[y][x] != 'S') {
        maze[y][x] = ' '; // Unmark
    }
    
    return 0;
}

int main() {
    if (!load_maze("maze.txt")) {
        return 1;
    }

    printf("Maze before solving:\n");
    print_maze();

    if (solve(start_y, start_x)) {
        printf("\nSolution found!\n");
        print_maze();
    } else {
        printf("\nNo solution exists.\n");
    }

    return 0;
}
