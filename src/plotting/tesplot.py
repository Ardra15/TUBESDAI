import pandas as pd
import matplotlib.pyplot as plt

# Define the data
data = {
    'Population': [1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1500, 1500, 1500, 2000, 2000, 2000],
    'Iterations': [4000, 4000, 4000, 16000, 16000, 16000, 32000, 32000, 32000, 5000, 5000, 5000, 5000, 5000, 5000, 5000],
    'Try': [1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3, 1],
    'Time': [13.11, 12.17, 16.34, 47.80, 48.30, 50.81, 106.47, 98.67, 97.79, 17.65, 15.50, 15.09, 22.85, 19.58, 22.34, 43.85],
    'Best Cost': [1146, 1128, 1188, 902, 1000, 902, 816, 880, 788, 1106, 1160, 1160, 1080, 1068, 1130, 1178]
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
