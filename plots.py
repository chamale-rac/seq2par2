import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Read the CSV file
df = pd.read_csv('complete_performance_report.csv')

# Plot 1: Execution Time Comparison
plt.figure(figsize=(12, 6))
plt.plot(df['Input Size'], df['Sequential Time'], marker='o', label='Sequential')
plt.plot(df['Input Size'], df['Parallel Time'], marker='s', label='Parallel')
plt.plot(df['Input Size'], df['Optimized Parallel Time'], marker='^', label='Optimized Parallel')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Input Size')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time Comparison')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('execution_time_comparison.png', dpi=300)
plt.show()

# Plot 2: Speedup Comparison
plt.figure(figsize=(12, 6))
plt.plot(df['Input Size'], df['Parallel Speedup'], marker='o', label='Parallel Speedup')
plt.plot(df['Input Size'], df['Optimized Speedup'], marker='s', label='Optimized Speedup')
plt.axhline(y=1, color='r', linestyle='--', label='Baseline (Sequential)')
plt.xscale('log')
plt.xlabel('Input Size')
plt.ylabel('Speedup')
plt.title('Speedup Comparison')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('speedup_comparison.png', dpi=300)
plt.show()

# Plot 3: Relative Performance
df['Sequential Relative'] = 1  # Baseline
df['Parallel Relative'] = df['Sequential Time'] / df['Parallel Time']
df['Optimized Relative'] = df['Sequential Time'] / df['Optimized Parallel Time']

plt.figure(figsize=(12, 6))
plt.bar(np.arange(len(df)) - 0.2, df['Sequential Relative'], width=0.2, label='Sequential', align='center')
plt.bar(np.arange(len(df)), df['Parallel Relative'], width=0.2, label='Parallel', align='center')
plt.bar(np.arange(len(df)) + 0.2, df['Optimized Relative'], width=0.2, label='Optimized Parallel', align='center')
plt.xlabel('Input Size')
plt.ylabel('Relative Performance')
plt.title('Relative Performance Comparison')
plt.xticks(np.arange(len(df)), df['Input Size'])
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('relative_performance_comparison.png', dpi=300)
plt.show()
