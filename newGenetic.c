#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define N 5  // Size of the cube

typedef struct {
    int cube[N][N][N];  // The N x N x N cube
    int fitness;        // Fitness score
} Individual;

double mutation_rate = 0.05; // Starting mutation rate

// Initialize the cube with numbers from 1 to N^3 randomly
void initialize_random_single_cube(int cube[N][N][N]) {
    int used[N * N * N] = {0};  
    int val;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                do {
                    val = (rand() % (N * N * N)) + 1;
                } while (used[val - 1]);  
                cube[i][j][k] = val;
                used[val - 1] = 1;
            }
        }
    }
}

// Calculate fitness using the difference from the expected value
int objective_function(int cube[N][N][N]) {
    int fitness = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                fitness += abs(cube[i][j][k] - (i * N * N + j * N + k + 1));
            }
        }
    }
    return fitness;
}

// Initialize the population with random individuals
void initialize_population(Individual population[], int population_size) {
    for (int i = 0; i < population_size; i++) {
        initialize_random_single_cube(population[i].cube);
        population[i].fitness = objective_function(population[i].cube);
    }
}

// Tournament selection
int tournament_selection(Individual population[], int population_size) {
    int best = rand() % population_size;
    for (int i = 1; i < 5; i++) {
        int competitor = rand() % population_size;
        if (population[competitor].fitness < population[best].fitness) {
            best = competitor;
        }
    }
    return best;
}

// Crossover using cycle crossover (CX)
void crossover(Individual *parent1, Individual *parent2, Individual *child) {
    int map[N * N * N] = {0};  
    int idx = 0;

    // Initialize the child with -1 values (unset)
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
                if (child->cube[i][j][k] == -1) {
                    int val = parent1->cube[i][j][k];
                    child->cube[i][j][k] = val;
                    map[val - 1] = 1;  
                }
            }
        }
    }

    // Fill the rest from parent2 without duplicates
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                if (child->cube[i][j][k] == -1) {
                    int val = parent2->cube[i][j][k];
                    if (!map[val - 1]) {
                        child->cube[i][j][k] = val;
                        map[val - 1] = 1;
                    } else {
                        // Find an unused value and insert it
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

    child->fitness = objective_function(child->cube);
}

// Mutation function using swap to ensure unique values
void mutate(Individual *individual) {
    int mutations = N * N * N * mutation_rate;
    for (int m = 0; m < mutations; m++) {
        int i1 = rand() % N, j1 = rand() % N, k1 = rand() % N;
        int i2 = rand() % N, j2 = rand() % N, k2 = rand() % N;

        int temp = individual->cube[i1][j1][k1];
        individual->cube[i1][j1][k1] = individual->cube[i2][j2][k2];
        individual->cube[i2][j2][k2] = temp;
    }
    individual->fitness = objective_function(individual->cube);
}

// Find the best individual in the population
int find_best_individual(Individual population[], int population_size) {
    int best_idx = 0;
    for (int i = 1; i < population_size; i++) {
        if (population[i].fitness < population[best_idx].fitness) {
            best_idx = i;
        }
    }
    return best_idx;
}

// Main genetic algorithm
void genetic_algorithm(int cube[N][N][N], int population_size, int iterations) {
    Individual population[population_size];
    Individual new_population[population_size];
    initialize_population(population, population_size);

    int iteration = 0;
    clock_t start = clock();
    int best_idx = find_best_individual(population, population_size);

    // Print the initial state
    printf("Initial Cube:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                printf("%3d ", population[best_idx].cube[i][j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("Initial Fitness: %d\n", population[best_idx].fitness);
    printf("Population Size: %d\n", population_size);
    printf("Iterations: %d\n", iterations);

    while (iteration < iterations) {
        #pragma omp parallel for
        for (int i = 0; i < 2; i++) {
            new_population[i] = population[best_idx];
        }

        #pragma omp parallel for
        for (int i = 2; i < population_size; i++) {
            int parent1_idx = tournament_selection(population, population_size);
            int parent2_idx = tournament_selection(population, population_size);
            crossover(&population[parent1_idx], &population[parent2_idx], &new_population[i]);
            mutate(&new_population[i]);
        }

        #pragma omp parallel for
        for (int i = 0; i < population_size; i++) {
            population[i] = new_population[i];
        }

        best_idx = find_best_individual(population, population_size);
        iteration++;

        if (population[best_idx].fitness == 0) {
            break;
        }
    }

    clock_t end = clock();
    printf("Genetic Algorithm: Iterations=%d, Time=%.2f seconds, Best Cost=%d\n", iteration, (double)(end - start) / CLOCKS_PER_SEC, population[best_idx].fitness);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                cube[i][j][k] = population[best_idx].cube[i][j][k];
            }
        }
    }
}

// Main function
int main() {
    srand(time(NULL));  // Seed the random number generator with the current time

    int population_size = 1000;  // Set your population size here
    int iterations = 16000;      // Set your number of iterations here
    int cube[N][N][N];  // The 5x5x5 cube to be optimized

    genetic_algorithm(cube, population_size, iterations);

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
