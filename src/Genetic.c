#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define N 5                                         // Size of the cube
#define TOTAL_NUMBERS (N * N * N)                   // Total number of cubes
#define MAGIC_NUMBER (N * (TOTAL_NUMBERS + 1)) / 2  // Magic number of cubes

typedef struct {
    int cube[N][N][N];  // The N x N x N cube
    int fitness;        // Fitness score
} Individual;

// Swap two integers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Initialize the cube with numbers from 1 to N^3 randomly
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

// Evaluate objective function
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

// Generate a population with random individuals
void generate_population(Individual population[], int population_size) {

    // Loop to initialize the population
    for (int i = 0; i < population_size; i++) {
        initialize_cube(population[i].cube);
        population[i].fitness = evaluate(population[i].cube);
    }
}

// Generate individual
int generate_individual(Individual population[], int population_size) {

    // Initialize best individual by generating random individual from population
    int best_individual = rand() % population_size;

    // Loop to find best individual
    for (int i = 1; i < 5; i++) {

        // Initialize competitor individiual by generating another random individual from population
        int competitor_individual = rand() % population_size;

        // Check if competitor individual's fitness is less than the best individual's fitness
        if (population[competitor_individual].fitness < population[best_individual].fitness) {
            best_individual = competitor_individual;
        }
    }
    return best_individual;
}

// Do a crossover 
void crossover(Individual *parent1, Individual *parent2, Individual *child) {
    int map[N * N * N] = {0};  
    int idx = 0;

    // Initialize child's value with -1
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                child->cube[i][j][k] = -1;
            }
        }
    }

    // Copy a cycle of values from parent1
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {

                // If a child's value is -1
                if (child->cube[i][j][k] == -1) {

                    // Store parent1 & child value in a temporary variable
                    int temp = parent1->cube[i][j][k];
                    child->cube[i][j][k] = temp;
                    map[temp - 1] = 1;  
                }
            }
        }
    }

    // Fill the rest from parent2 without duplicates
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {

                // If child's value is -1
                if (child->cube[i][j][k] == -1) {

                    // Store parent2's value in a temporary variable
                    int temp = parent2->cube[i][j][k];


                    if (!map[temp - 1]) {
                        child->cube[i][j][k] = temp;
                        map[temp - 1] = 1;
                    } else {

                        // Find unused value and insert to the array
                        for (int m = 0; m < N * N * N; m++) {
                            if (!map[m]) {
                                child->cube[i][j][k] = m + 1;
                                map[m] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Evaluate child's fitness
    child->fitness = evaluate(child->cube);
}

// Do a mutation
void mutation(Individual *individual) {

    // Initialize the mutation rate & total mutation
    double initial_mutation_rate = 0.05;
    int total_mutation = N * N * N * initial_mutation_rate;

    // Loop to do a mutation
    for (int m = 0; m < total_mutation; m++) {

        // Randomize the child
        int i1 = rand() % N, j1 = rand() % N, k1 = rand() % N;
        int i2 = rand() % N, j2 = rand() % N, k2 = rand() % N;

        // Swapping two individuals
        int temp = individual->cube[i1][j1][k1];
        individual->cube[i1][j1][k1] = individual->cube[i2][j2][k2];
        individual->cube[i2][j2][k2] = temp;
    }

    // Evaluate an individual's fitness
    individual->fitness = evaluate(individual->cube);
}

// Find the best individual in the population
int find_best_individual(Individual population[], int population_size) {

    // Best individual initialization
    int best_individual = 0;

    // Loop to find the best individual
    for (int i = 1; i < population_size; i++) {
        if (population[i].fitness < population[best_individual].fitness) {
            best_individual = i;
        }
    }
    return best_individual;
}

// Main function
int main() {
    srand(time(NULL));  // Seed the random number generator with the current time

    int iterations = 100;       // Set number of iterations
    int population_size = 100;  // Set population size
    int cube[N][N][N];          // Initialize the 5x5x5 cube

    // Initialize 2 individuals with different population
    Individual population[population_size];
    Individual new_population[population_size];

    // Generate population
    generate_population(population, population_size);

    // Initialize iteration counter, timer, and best_individual
    int counter = 0;
    clock_t start = clock();
    int best_individual = find_best_individual(population, population_size);

    // Print the initial state
    printf("Initial Cube:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                printf("%3d ", population[best_individual].cube[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("Initial Fitness: %d\n", population[best_individual].fitness);
    printf("Population Size: %d\n", population_size);
    printf("Iterations: %d\n", iterations);

    // Main loop
    while (counter < iterations) {

        // Loop to generate new population
        #pragma omp parallel for
        for (int i = 0; i < 2; i++) {
            new_population[i] = population[best_individual];
        }

        // Loop to do crossover & mutation
        #pragma omp parallel for
        for (int i = 2; i < population_size; i++) {

            // Initialize parents
            int parent1_idx = generate_individual(population, population_size);
            int parent2_idx = generate_individual(population, population_size);

            // Crossover & mutation
            crossover(&population[parent1_idx], &population[parent2_idx], &new_population[i]);
            mutation(&new_population[i]);
        }

        // Loop to generate population
        #pragma omp parallel for
        for (int i = 0; i < population_size; i++) {
            population[i] = new_population[i];
        }

        // Find another best individual, and the number of iteration will increase
        best_individual = find_best_individual(population, population_size);
        counter++;

        // If fitness of a best individual from a population reaches zero, the program will stop
        if (population[best_individual].fitness == 0) {
            break;
        }
    }

    // Timer ends
    clock_t end = clock();
    printf("Genetic Algorithm: Iterations=%d, Time=%.2f seconds, Best Cost=%d\n", counter, (double)(end - start) / CLOCKS_PER_SEC, population[best_individual].fitness);

    // Generate the final & optimized cube
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                cube[i][j][k] = population[best_individual].cube[i][j][k];
            }
        }
    }

    printf("Optimized Cube:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                printf("%3d ", cube[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    return 0;
}