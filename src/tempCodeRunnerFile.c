int main() {
    // srand(time(NULL));  // Seed the random number generator with the current time
    
    int population_size = 10;  // Set your population size here
    int generations = 10;      // Set your number of generations here
    int cube[N][N][N];  // The 5x5x5 cube to be optimized

    genetic_algorithm(cube, population_size, generations);

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