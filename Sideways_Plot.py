import matplotlib.pyplot as plt

# Initialize lists to store data
iterations = []
errors = []

# Load data from the objective_function.txt file
with open("output/objective_function.txt", "r") as file:
    for line in file:
        iter_num, error = line.split()
        iterations.append(int(iter_num))
        errors.append(int(error))

# Plot the data
plt.plot(iterations, errors, marker='o', markersize=2, linestyle='-')
plt.xlabel('Iteration')
plt.ylabel('Objective Function (Error)')
plt.title('Objective Function vs. Iterations')
plt.grid(True)
plt.show()
