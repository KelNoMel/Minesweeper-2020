// Assignment 1 20T1 COMP1511: Minesweeper
// minesweeper.c
//
// This program was written by Kellen Liew (z5312656)
// on 15/03/20
//
// Version 1.0.0 (2020-03-08): Assignment released.
// Version 1.0.1 (2020-03-08): Fix punctuation in comment.
// Version 1.0.2 (2020-03-08): Fix second line of header comment to say minesweeper.c
// Program that plays the minesweeper game

#include <stdio.h>
#include <stdlib.h>

// Possible square states.
#define VISIBLE_SAFE    0
#define HIDDEN_SAFE     1
#define HIDDEN_MINE     2

// The size of the starting grid.
#define SIZE 8

// The possible command codes.
#define DETECT_ROW              1
#define DETECT_COL              2
#define DETECT_SQUARE           3
#define REVEAL_SQUARE           4
#define GAMEPLAY_MODE           5
#define DEBUG_MODE              6
#define REVEAL_RADIAL           7

// Add any extra #defines here.

void initialise_field(int minefield[SIZE][SIZE]);
void print_debug_minefield(int minefield[SIZE][SIZE]);
void print_gameplay_minefield(int minefield[SIZE][SIZE], int doomswitch);
int scan_adjacent(int minefield[SIZE][SIZE], int size, int grid1, int grid2);
void protect_reveal(int minefield[SIZE][SIZE]);

// Place your function prototyes here.

int main(void) {
    int minefield[SIZE][SIZE];
    initialise_field(minefield);
    
    //Introduce Player and set parameters
    printf("Welcome to minesweeper!\n");
    printf("How many mines? ");

    int mines_num = 0, mines_tot = 0;
    scanf("%d", &mines_num);

    printf("Enter pairs:\n");

    int row, column;
    while (mines_tot < mines_num) {
        scanf("%d %d", &row, &column);

        if (row < SIZE && column < SIZE &&
            row >= 0 && column >= 0) {
            minefield[row][column] = HIDDEN_MINE;
            mines_tot++;
        }
        else {
            mines_tot++;
        }

    }
    row = 0;
    column = 0;

    printf("Game Started\n");
    print_debug_minefield(minefield);
    
    //Win condition
    int to_clear = SIZE*SIZE - mines_num;

    // Game variables external from commands
    int doomswitch = 0, count = 0, cleared = 0, viewmode = 0, safety_net = 0;
    
    // Commands for during the game
    int command_type = 0, grid1 = 0, grid2 = 0;
    
    // Variables within commands
    int mines_in = 0, size = 0;

    // Game in session
    while (doomswitch == 0) {
        scanf("%d", &command_type);
        
        // Restrict to 3 hints
        if (command_type < 3 && command_type != 0 && count > 2) {
            printf("Help already used\n");
            scanf("%d", &grid1);
        }
        else if (command_type == 3 && command_type != 0 && count > 2) {
            printf("Help already used\n");
            scanf("%d %d %d", &grid1, &grid2, &size);
        }

        // Command for counting mines in a row (1)
        if (command_type == 1 && count < 3) {
            scanf("%d", &grid1);
            while (column < SIZE) {

                if (minefield[grid1][column] == 2) {
                    mines_in++;
                }
                column++;
            }
            printf("There are %d mine(s) in row %d \n", mines_in, grid1);
            column = 0;
            mines_in = 0;
            count++;
        }

        // Command for counting mines in a column (2)
        if (command_type == 2 && count < 3) {

            scanf("%d", &grid1);
            while (row < SIZE) {

                if (minefield[row][grid1] == 2) {
                    mines_in++;
                }
                row++;
            }
            printf("There are %d mine(s) in column %d \n", mines_in, grid1);
            row = 0;
            mines_in = 0;
            count++;
        }
        

        // Command for detecting mines in a square of n size (3)
        int hold_row = 0, hold_column = 0;
        if (command_type == 3 && count < 3) {

            scanf("%d %d %d", &grid1, &grid2, &size);

            mines_in = scan_adjacent(minefield, size, grid1, grid2);

            printf("There are %d mine(s) in the square ", mines_in);
            printf("centered at row %d, ", grid1);
            printf("column %d of size %d \n", grid2, size);
            
            mines_in = 0;
            count++;
        }

        // Command for revealing squares (4)
        if (command_type == 4) {

            size = 3;
            scanf("%d %d", &grid1, &grid2);
            hold_row = grid1;
            hold_column = grid2;

            // Coordinates are a mine = game over
            if (minefield[grid1][grid2] == 2 && safety_net > 0) {
                printf("Game over\n");
                doomswitch--;
            }
            // Protect first reveal
            else if (minefield[grid1][grid2] == 2 && safety_net == 0) {
                while (minefield[grid1][grid2] == 2) {
                    protect_reveal(minefield);
                }
            }

            // Coordinates are not a mine = reveal
            if (minefield[grid1][grid2] == 1) {
                // Scan for mines nearby
                mines_in = scan_adjacent(minefield, size, grid1, grid2);

                // Mines nearby = reveal selected coordinates
                if (mines_in > 0) {
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                }
                // No mines nearby = reveal square as safe
                else if (mines_in == 0) {

                    // Move to top left corner
                    while (grid1 > 0 && grid1 > hold_row - size / 2) {
                        grid1--;
                    }

                    while (grid2 > 0 && grid2 > hold_column - size / 2) {
                        grid2--;
                    }
                    // Reveal left to right by descending row
                    while (grid1 <= hold_row + size / 2 && grid1 < SIZE) {

                        while (grid2 <= hold_column + size / 2 && grid2 < SIZE) {
                            if (minefield[grid1][grid2] != VISIBLE_SAFE) {
                                minefield[grid1][grid2] = VISIBLE_SAFE;
                                cleared++;
                            }
                            grid2++;
                        }

                        // Move back left and down one row
                        while (grid2 > 0 && grid2 > hold_column - size / 2) {
                            grid2--;
                        }
                        grid1++;
                    }
                }
                mines_in = 0;
            }
            safety_net++;
        }

        // Switch between gameplay/debug mode (5) (6)
        if (command_type == 5 && viewmode == 0) {
            viewmode++;
            printf("Gameplay mode activated\n");
        }

        if (command_type == 6 && viewmode == 1) {
            viewmode--;
            printf("Debug mode activated\n");
        }
        
        // Reveal Radial (7)
        if (command_type == 7) {
            scanf("%d %d", &grid1, &grid2);
            hold_row = grid1;
            hold_column = grid2;
            
            // Coordinates are a mine = game over
            if (minefield[grid1][grid2] == 2 && safety_net > 0) {
                printf("Game over\n");
                doomswitch--;
            }
            
            // Protect first reveal
            else if (minefield[grid1][grid2] == 2 && safety_net == 0) {
                while (minefield[grid1][grid2] == 2) {
                    protect_reveal(minefield);
                }
            }
                
            // Coordinates are not a mine = revealing directions
            if (minefield[grid1][grid2] == 1) {
                size = 3;
                // Reveal Up
                while (grid1 >= 0 && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid1--;
                }
                grid1 = hold_row;
                mines_in = 0;
                // Reveal Down
                while (grid1 < SIZE && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid1++;
                }
                grid1 = hold_row;
                mines_in = 0;
                // Reveal Left
                while (grid2 >= 0 && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid2--;
                }
                grid2 = hold_column;
                mines_in = 0;
                // Reveal Right
                while (grid2 < SIZE && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid2++;
                }
                grid2 = hold_column;
                mines_in = 0;
                // Reveal Up Right
                while (grid1 >= 0 && grid2 < SIZE && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid1--;
                    grid2++;
                }
                grid1 = hold_row;
                grid2 = hold_column;
                mines_in = 0;
                // Reveal Down Right
                while (grid1 < SIZE && grid2 < SIZE && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid1++;
                    grid2++;
                }
                grid1 = hold_row;
                grid2 = hold_column;
                mines_in = 0;
                // Reveal Up Left
                while (grid1 >= 0 && grid2 >= 0 && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid1--;
                    grid2--;
                }
                grid1 = hold_row;
                grid2 = hold_column;
                mines_in = 0;
                // Reveal Down Left
                while (grid1 < SIZE && grid2 >= 0 && mines_in == 0) {
                    mines_in = scan_adjacent(minefield, size, grid1, grid2);
                    minefield[grid1][grid2] = VISIBLE_SAFE;
                    cleared++;
                    grid1++;
                    grid2--;
                }
                grid1 = hold_row;
                grid2 = hold_column;
                mines_in = 0;
            }
            safety_net++;
        }

        // Win condition fulfilled
        if (cleared == to_clear) {
            printf("Game Won!\n");
            doomswitch++;
        }

        // End the game by inaction
        if (command_type == 0) {
            doomswitch++;
        }

        // Print the minefield
        if (viewmode == 0 && command_type != 0) {
            print_debug_minefield(minefield);
        }

        else if (viewmode == 1 && command_type != 0) {
            print_gameplay_minefield(minefield, doomswitch);
        }
        command_type = 0;
        //printf("%d %d\n", command_type, count);
    }

    return 0;
}

// Scan the squares adjacent to coordinates
int scan_adjacent(int minefield[SIZE][SIZE], int size, int grid1, int grid2) {
    int hold_row = grid1;
    int hold_column = grid2;
    int mines_in = 0;

    // Move array to the top left of the square or grid
    while (grid1 > 0 && grid1 > hold_row - size / 2) {
        grid1--;
    }

    while (grid2 > 0 && grid2 > hold_column - size / 2) {
        grid2--;
    }

    // Scanning left to right by descending rows
    while (grid1 <= hold_row + size / 2 && grid1 < SIZE) {

        while (grid2 <= hold_column + size / 2 && grid2 < SIZE) {
            if (minefield[grid1][grid2] == HIDDEN_MINE) {
                mines_in++;
            }
            grid2++;
        }

        // Move back left and down one row
        while (grid2 > 0 && grid2 > hold_column - size / 2) {
            grid2--;
        }

        grid1++;
    }
    return mines_in;
}

// Set the minefield so that first reveal is safe
void protect_reveal(int minefield[SIZE][SIZE]) {
    int i = 0;
    int j = 0;
    while (j < SIZE) {
        minefield[i][j] = minefield[i + SIZE - 1][j];
        j++;
    }
    i++;
    j = 0;
    
    while (i < SIZE) {
        while (j < SIZE) {
            minefield[i][j] = minefield[i - 1][j];
            j++;
        }
        i++;
    }
}

// Set the entire minefield to HIDDEN_SAFE.
void initialise_field(int minefield[SIZE][SIZE]) {
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            minefield[i][j] = HIDDEN_SAFE;
            j++;
        }
        i++;
    }
}

// Print out the actual values of the minefield.
void print_debug_minefield(int minefield[SIZE][SIZE]) {
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        while (j < SIZE) {
            printf("%d ", minefield[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }
}

// Print out the gameplay interface for the minefield
void print_gameplay_minefield(int minefield[SIZE][SIZE], int doomswitch) {
    if (doomswitch != -1) {
        printf(".. \n");
        printf("\\/ \n");
    }
    else if (doomswitch == -1) {
        printf("xx\n");
        printf("/\\ \n");
    }
    printf("    00 01 02 03 04 05 06 07\n");
    printf("   -------------------------\n");
    int i = 0;
    while (i < SIZE) {
        int j = 0;
        printf("0%d |", i);
        while (j < SIZE) {

            if (minefield[i][j] == HIDDEN_SAFE) {
                printf("##");
            }
            if (minefield[i][j] == HIDDEN_MINE && doomswitch != -1) {
                printf("##");
            }
            else if (minefield[i][j] == HIDDEN_MINE && doomswitch == -1) {
                printf("()");
            }

            if (minefield[i][j] == VISIBLE_SAFE) {
                int mines_in = 0, size = 3;
                // Check for adjacent mines
                mines_in = scan_adjacent(minefield, size, i, j);

                // All adjacents are VISIBLE_SAFE or HIDDEN_SAFE
                if (mines_in == 0) {
                    printf("  ");
                }
                // One or more adjacent are HIDDEN_MINE
                else {
                    printf("0%d", mines_in);
                }
            }
            if (j + 1 != SIZE) {
                printf(" ");
            }
            j++;
        }
        printf("|\n");
        i++;
    }
    printf("   -------------------------\n");
}
