import pandas as pd
import matplotlib.pyplot as plt

# Corrected data
data = {
    'Population': [1000] * 9 + [1500] * 3 + [2000] * 3 + [1000] * 3,
    'Iterations': [5000] * 3 + [50000] * 3 + [75000] * 3 + [5000] * 9,
    'Try': [1, 2, 3] * 6,
    'Time': [29.13, 19.22, 20.07, 135.49, 131.07, 130.16, 197.10, 204.26, 194.53,
             11.48, 11.42, 11.40, 19.78, 16.72, 16.97, 24.84, 23.32, 23.41],
    'Best Cost': [1078, 1142, 1154, 748, 766, 800, 752, 646, 636, 
                  1056, 1086, 1118, 1140, 1128, 1158, 1096, 1200, 1076]
}

# Create a DataFrame
df = pd.DataFrame(data)

# Plot where Population is fixed (Population = 1000)
df_pop_1000 = df[df['Population'] == 1000]
iterations_summary = df_pop_1000.groupby('Iterations')['Best Cost'].agg(['max', 'mean']).reset_index()

plt.figure(figsize=(10, 6))
plt.plot(iterations_summary['Iterations'], iterations_summary['max'], marker='o', color='r', label='Max Best Cost')
plt.plot(iterations_summary['Iterations'], iterations_summary['mean'], marker='s', color='b', label='Average Best Cost')
plt.title('Objective Function Analysis (Population = 1000)')
plt.xlabel('Iterations')
plt.ylabel('Best Cost')
plt.legend()
plt.grid(True)
plt.show()

# Plot where Iterations are fixed (Iterations = 5000)
df_iter_5000 = df[df['Iterations'] == 5000]
population_summary = df_iter_5000.groupby('Population')['Best Cost'].agg(['max', 'mean']).reset_index()

plt.figure(figsize=(10, 6))
plt.plot(population_summary['Population'], population_summary['max'], marker='o', color='r', label='Max Best Cost')
plt.plot(population_summary['Population'], population_summary['mean'], marker='s', color='b', label='Average Best Cost')
plt.title('Objective Function Analysis (Iterations = 5000)')
plt.xlabel('Population')
plt.ylabel('Best Cost')
plt.legend()
plt.grid(True)
plt.show()
