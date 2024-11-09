#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define POPULATION_SIZE 100
#define ITERATIONS 1000
#define TOURNAMENT_SIZE 5
#define ELITISM 2
#define N 5

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
void initialize_population(Individual population[]) {
    for (int i = 0; i < POPULATION_SIZE; i++) {
        initialize_random_single_cube(population[i].cube);
        population[i].fitness = objective_function(population[i].cube);
    }
}

// Tournament selection
int tournament_selection(Individual population[]) {
    int best = rand() % POPULATION_SIZE;
    for (int i = 1; i < TOURNAMENT_SIZE; i++) {
        int competitor = rand() % POPULATION_SIZE;
        if (population[competitor].fitness < population[best].fitness) {
            best = competitor;
        }
    }
    return best;
}

// Crossover using cycle crossover (CX)
void crossover(Individual *parent1, Individual *parent2, Individual *child) {
    int map[N * N * N] = {0};

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
int find_best_individual(Individual population[]) {
    int best_idx = 0;
    for (int i = 1; i < POPULATION_SIZE; i++) {
        if (population[i].fitness < population[best_idx].fitness) {
            best_idx = i;
        }
    }
    return best_idx;
}

// Dynamic mutation rate adjustment based on population diversity
void adjust_mutation_rate(Individual population[]) {
    int unique_count = 0;
    for (int i = 0; i < POPULATION_SIZE - 1; i++) {
        if (population[i].fitness != population[i + 1].fitness) unique_count++;
    }
    if (unique_count < POPULATION_SIZE / 2) {
        mutation_rate = fmin(0.15, mutation_rate * 1.1);
    } else {
        mutation_rate = fmax(0.01, mutation_rate * 0.9);
    }
}

// Main genetic algorithm
void genetic_algorithm(int cube[N][N][N]) {
    Individual population[POPULATION_SIZE];
    Individual new_population[POPULATION_SIZE];
    initialize_population(population);

    int iteration = 0;
    clock_t start = clock();
    int best_idx = find_best_individual(population);

    while (iteration < ITERATIONS) {
        for (int i = 0; i < ELITISM; i++) {
            new_population[i] = population[best_idx];
        }

        for (int i = ELITISM; i < POPULATION_SIZE; i++) {
            int parent1_idx = tournament_selection(population);
            int parent2_idx = tournament_selection(population);
            crossover(&population[parent1_idx], &population[parent2_idx], &new_population[i]);
            mutate(&new_population[i]);
        }

        for (int i = 0; i < POPULATION_SIZE; i++) {
            population[i] = new_population[i];
        }

        adjust_mutation_rate(population);

        best_idx = find_best_individual(population);
        
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
    
    int cube[N][N][N];  // The 5x5x5 cube to be optimized

    genetic_algorithm(cube);

    printf("Optimized Cube:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                printf("%d ", cube[i][j][k]);
            }
            printf("\n");
        }
    }

    return 0;
}