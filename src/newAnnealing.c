#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 5
#define TOTAL_NUMBERS (N * N * N)
#define MAGIC_NUMBER (N * (TOTAL_NUMBERS + 1)) / 2
#define INITIAL_TEMPERATURE 1000.0
#define FINAL_TEMPERATURE 0
#define ALPHA 0.999
#define MAX_ITERATIONS 100000
#define STUCK_THRESHOLD 100  // Define how many iterations of no improvement is considered 'stuck'

// Function prototypes
void initialize_cube(int cube[N][N][N]);
void print_cube(int cube[N][N][N]);
int evaluate(int cube[N][N][N]);
void swap(int *a, int *b);
void generate_random_neighbor(int cube[N][N][N], int new_cube[N][N][N]);
void copy_cube(int src[N][N][N], int dest[N][N][N]);
double acceptance_probability(int current_error, int new_error, double temperature);

int main() {
    clock_t start_time = clock();
    srand(time(0));

    int current_cube[N][N][N];
    int new_cube[N][N][N];
    int current_error, new_error;
    double temperature = INITIAL_TEMPERATURE;

    // Initialize the cube with random values
    initialize_cube(current_cube);
    current_error = evaluate(current_cube);

    printf("Initial Cube:\n");
    print_cube(current_cube);
    printf("Initial Error: %d\n", current_error);

    int iterations = 0;
    int stuck_count = 0;  // Counter for local optima "stuck" cases
    int no_improvement_count = 0;  // To detect prolonged lack of improvement

    // Array to store acceptance probability values for plotting
    double acceptance_probs[MAX_ITERATIONS];
    
    while (temperature > FINAL_TEMPERATURE && iterations < MAX_ITERATIONS) {
        iterations++;

        // Generate a random neighbor
        generate_random_neighbor(current_cube, new_cube);
        new_error = evaluate(new_cube);
        
        double prob = acceptance_probability(current_error, new_error, temperature);
        acceptance_probs[iterations - 1] = prob;  // Store the probability for plotting

        // Accept the new solution if it's better, or with a probability if worse
        if (new_error < current_error || prob > ((double)rand() / RAND_MAX)) {
            copy_cube(new_cube, current_cube);
            current_error = new_error;
            no_improvement_count = 0;  // Reset count as we've found improvement
        } else {
            no_improvement_count++;
        }

        // Check if we've been stuck in local optima for a while
        if (no_improvement_count >= STUCK_THRESHOLD) {
            stuck_count++;
            no_improvement_count = 0;  // Reset to avoid repeated counting
        }

        // Cool down the temperature
        temperature *= ALPHA;

        // Print status every 1000 iterations
        if (iterations % 10000 == 0) {
            printf("Iteration %d - Current Error: %d - Temperature: %.2f\n", iterations, current_error, temperature);
        }
    }

    printf("Final Cube after %d iterations:\n", iterations);
    print_cube(current_cube);
    printf("Final Error: %d\n", current_error);
    printf("Total stuck occurrences (local optima): %d\n", stuck_count);

    // Plotting acceptance probability (to be done outside C or by exporting data)
    FILE *fptr = fopen("acceptance_probs.txt", "w");
    for (int i = 0; i < iterations; i++) {
        fprintf(fptr, "%d %f\n", i + 1, acceptance_probs[i]);
    }
    fclose(fptr);
    printf("Acceptance probabilities saved to 'acceptance_probs.txt'.\n");
    
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

    // Evaluate diagonal in slices
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

// Generate a random neighbor by swapping two elements in the cube
void generate_random_neighbor(int cube[N][N][N], int new_cube[N][N][N]) {
    copy_cube(cube, new_cube);

    // Randomly select two positions to swap
    int i1 = rand() % N, j1 = rand() % N, k1 = rand() % N;
    int i2 = rand() % N, j2 = rand() % N, k2 = rand() % N;

    // Swap two elements
    swap(&new_cube[i1][j1][k1], &new_cube[i2][j2][k2]);
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

// Calculate the acceptance probability for worse solutions
double acceptance_probability(int current_error, int new_error, double temperature) {
    if (new_error < current_error) {
        return 1.0;
    }
    return exp((double)(current_error - new_error) / temperature);
}
