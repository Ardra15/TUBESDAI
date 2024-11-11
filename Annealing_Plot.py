import matplotlib.pyplot as plt

# Load data from acceptance_probs.txt
iteration = []
acceptance_probs = []

with open("output/acceptance_probs.txt", "r") as f:
    for line in f:
        iter_num, prob = line.split()
        iteration.append(int(iter_num))
        acceptance_probs.append(float(prob))

# Plot the data
plt.plot(iteration, acceptance_probs)
plt.xlabel('Iteration')
plt.ylabel(r'$e^{\Delta E / T}$')
plt.title('Acceptance Probability vs Iterations')
plt.grid(True)
plt.show()