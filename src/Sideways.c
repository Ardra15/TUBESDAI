#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define N 5
#define TOTAL_NUMBERS (N * N * N)
#define MAGIC_NUMBER (N * (TOTAL_NUMBERS + 1)) / 2
#define MAX_SIDEWAYS_MOVES 5000  // Limit for sideways moves

// Function prototypes
void initialize_cube(int cube[N][N][N]);
void print_cube(int cube[N][N][N]);
int evaluate(int cube[N][N][N]);
void swap(int *a, int *b);
void generate_all_neighbors(int cube[N][N][N], int best_cube[N][N][N], int *best_error);
void copy_cube(int src[N][N][N], int dest[N][N][N]);

int main() {
    clock_t start_time = clock();
    srand(time(0));

    int current_cube[N][N][N];
    int best_cube[N][N][N];
    int current_error, best_error;
    int sideways_moves = 0;

    FILE *fptr = fopen("objective_function.txt", "w");
    if (fptr == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    // Initialize the cube with random values
    initialize_cube(current_cube);
    current_error = evaluate(current_cube);

    printf("Initial Cube:\n");
    print_cube(current_cube);
    printf("Initial Error: %d\n", current_error);

    int iterations = 0;
    while (current_error > 0) {
        iterations++;

        // Generate all neighbors and choose the best one
        best_error = current_error;
        generate_all_neighbors(current_cube, best_cube, &best_error);

        // If a better neighbor is found, reset sideways move counter
        if (best_error < current_error) {
            copy_cube(best_cube, current_cube);
            current_error = best_error;
            sideways_moves = 0;  // Reset sideways move counter
        }
        // If no improvement, but the neighbor has the same error (sideways move)
        else if (best_error == current_error && sideways_moves < MAX_SIDEWAYS_MOVES) {
            copy_cube(best_cube, current_cube);
            current_error = best_error;
            sideways_moves++;  // Increment sideways move counter
            // printf("Sideways move: %d\n", sideways_moves);
        }
        // If no better neighbor and sideways limit is reached, stop (local optimum)
        else {
            printf("Reached local optimum or sideways move limit. Stopping.\n");
            break;
        }
        
        fprintf(fptr, "%d %d\n", iterations, current_error);


        // Print status every 10 iterations
        if (iterations % 1000 == 0) {
            printf("Iteration %d - Current Error: %d\n", iterations, current_error);
        }
    }

    printf("Final Cube after %d iterations:\n", iterations);
    print_cube(current_cube);
    printf("Final Error: %d\n", current_error);
    fclose(fptr);

    // Record the end time
    clock_t end_time = clock();

    // Calculate the time difference in seconds
    double duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Program execution time: %.2f seconds\n", duration);

    return 0;
}

// Initialize the cube with numbers from 1 to TOTAL_NUMBERS randomly
void initialize_cube(int cube[N][N][N]) {
    int numbers[TOTAL_NUMBERS];
    for (int i = 0; i < TOTAL_NUMBERS; i++) {
        numbers[i] = i + 1;
    }

    // Shuffle the numbers array
    for (int i = TOTAL_NUMBERS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(&numbers[i], &numbers[j]);
    }

    // Fill the cube with the shuffled numbers
    int idx = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                cube[i][j][k] = numbers[idx++];
            }
        }
    }
}

// Print the 5x5x5 cube
void print_cube(int cube[N][N][N]) {
    for (int i = 0; i < N; i++) {
        printf("Slice %d:\n", i + 1);
        
        // Print the top border for the slice
        printf("   +");
        for (int k = 0; k < N; k++) {
            printf("-----+");
        }
        printf("\n");

        for (int j = 0; j < N; j++) {
            printf("   | ");
            for (int k = 0; k < N; k++) {
                printf("%3d | ", cube[i][j][k]);
            }
            printf("\n");
            
            // Print the row separator
            printf("   +");
            for (int k = 0; k < N; k++) {
                printf("-----+");
            }
            printf("\n");
        }
        
        printf("\n"); // Add a newline between slices for better readability
    }
}

// Evaluate the error of the cube (difference from MAGIC_NUMBER)
int evaluate(int cube[N][N][N]) {
    int error = 0;
    int sum;

    // Evaluate rows
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            sum = 0;
            for (int k = 0; k < N; k++) {
                sum += cube[i][j][k];
            }
            error += abs(sum - MAGIC_NUMBER);
        }
    }

    // Evaluate columns
    for (int j = 0; j < N; j++) {
        for (int k = 0; k < N; k++) {
            sum = 0;
            for (int i = 0; i < N; i++) {
                sum += cube[i][j][k];
            }
            error += abs(sum - MAGIC_NUMBER);
        }
    }

    // Evaluate pillars
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            sum = 0;
            for (int j = 0; j < N; j++) {
                sum += cube[i][j][k];
            }
            error += abs(sum - MAGIC_NUMBER);
        }
    }

    // Evaluate main space diagonals
    sum = 0;
    for (int i = 0; i < N; i++) {
        sum += cube[i][i][i];
    }
    error += abs(sum - MAGIC_NUMBER);

    sum = 0;
    for (int i = 0; i < N; i++) {
        sum += cube[i][i][N - i - 1];
    }
    error += abs(sum - MAGIC_NUMBER);

    // Evaluate diagonals in horizontal (x-y) slices
    for (int i = 0; i < N; i++) {
        sum = 0;
        for (int j = 0; j < N; j++) {
            sum += cube[i][j][j];
        }
        error += abs(sum - MAGIC_NUMBER);

        sum = 0;
        for (int j = 0; j < N; j++) {
            sum += cube[i][j][N - j - 1];
        }
        error += abs(sum - MAGIC_NUMBER);
    }

    // Evaluate diagonals in vertical (y-z) slices
    for (int j = 0; j < N; j++) {
        sum = 0;
        for (int k = 0; k < N; k++) {
            sum += cube[k][j][k];
        }
        error += abs(sum - MAGIC_NUMBER);

        sum = 0;
        for (int k = 0; k < N; k++) {
            sum += cube[N - k - 1][j][k];
        }
        error += abs(sum - MAGIC_NUMBER);
    }

    // Evaluate diagonals in vertical (x-z) slices
    for (int k = 0; k < N; k++) {
        sum = 0;
        for (int i = 0; i < N; i++) {
            sum += cube[i][i][k];
        }
        error += abs(sum - MAGIC_NUMBER);

        sum = 0;
        for (int i = 0; i < N; i++) {
            sum += cube[i][N - i - 1][k];
        }
        error += abs(sum - MAGIC_NUMBER);
    }

    return error;
}

// Swap two integers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Generate all possible neighbors by swapping two elements in the cube
void generate_all_neighbors(int cube[N][N][N], int best_cube[N][N][N], int *best_error) {
    int temp_cube[N][N][N];
    copy_cube(cube, temp_cube);

    // Try all possible pairs of positions in the cube
    for (int i1 = 0; i1 < N; i1++) {
        for (int j1 = 0; j1 < N; j1++) {
            for (int k1 = 0; k1 < N; k1++) {
                for (int i2 = i1; i2 < N; i2++) {
                    for (int j2 = (i1 == i2 ? j1 + 1 : 0); j2 < N; j2++) {
                        for (int k2 = (i1 == i2 && j1 == j2 ? k1 + 1 : 0); k2 < N; k2++) {
                            // Swap two elements
                            swap(&temp_cube[i1][j1][k1], &temp_cube[i2][j2][k2]);

                            // Evaluate the neighbor
                            int neighbor_error = evaluate(temp_cube);

                            // If this neighbor is better or equal (for sideways), remember it
                            if (neighbor_error <= *best_error) {
                                copy_cube(temp_cube, best_cube);
                                *best_error = neighbor_error;
                            }

                            // Swap back to restore the original cube
                            swap(&temp_cube[i1][j1][k1], &temp_cube[i2][j2][k2]);
                        }
                    }
                }
            }
        }
    }
}

// Copy one cube to another
void copy_cube(int src[N][N][N], int dest[N][N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                dest[i][j][k] = src[i][j][k];
            }
        }
    }
}
