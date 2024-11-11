#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define N 5
#define TOTAL_NUMBERS (N * N * N)
#define MAGIC_NUMBER (N * (TOTAL_NUMBERS + 1)) / 2
#define MAX_ITERATIONS 1000

// Function prototypes
void initialize_cube(int cube[N][N][N]);
void print_cube(int cube[N][N][N]);
int evaluate(int cube[N][N][N]);
void swap(int *a, int *b);
void generate_random_neighbor(int cube[N][N][N], int neighbor[N][N][N]);
void copy_cube(int src[N][N][N], int dest[N][N][N]);

int main() {
    srand(time(0));

    int current_cube[N][N][N];
    int neighbor_cube[N][N][N];
    int current_error, neighbor_error;

    // Initialize the cube with random values
    initialize_cube(current_cube);
    current_error = evaluate(current_cube);

    printf("Initial Cube:\n");
    print_cube(current_cube);
    printf("Initial Error: %d\n", current_error);

    int iterations = 0;
    while (current_error > 0 && iterations < MAX_ITERATIONS) {
        iterations++;

        // Generate a random neighbor
        generate_random_neighbor(current_cube, neighbor_cube);
        neighbor_error = evaluate(neighbor_cube);

        // If the neighbor has a lower error, move to that neighbor
        if (neighbor_error < current_error) {
            copy_cube(neighbor_cube, current_cube);
            current_error = neighbor_error;
        }

        // Print status every 1000 iterations
        if (iterations % 50 == 0) {
            printf("Iteration %d - Current Error: %d\n", iterations, current_error);
        }
    }

    printf("Final Cube after %d iterations:\n", iterations);
    print_cube(current_cube);
    printf("Final Error: %d\n", current_error);

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
        printf("Slice %d:\n", i+1);
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                printf("%3d ", cube[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
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
        sum += cube[i][i][N-i-1];
    }
    error += abs(sum - MAGIC_NUMBER);

    // Evaluate diagonal in slices
    for (int i = 0; i < N; i++) {
        sum = 0;
        for (int j = 0; j < N; j++) {
            sum += cube[i][j][j];
        }
        error += abs(sum - MAGIC_NUMBER);

        sum = 0;
        for (int j = 0; j < N; j++) {
            sum += cube[i][j][N-j-1];
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

// Generate a random neighbor by swapping two elements in the cube
void generate_random_neighbor(int cube[N][N][N], int neighbor[N][N][N]) {
    copy_cube(cube, neighbor);

    int i1 = rand() % N, j1 = rand() % N, k1 = rand() % N;
    int i2 = rand() % N, j2 = rand() % N, k2 = rand() % N;

    swap(&neighbor[i1][j1][k1], &neighbor[i2][j2][k2]);
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
