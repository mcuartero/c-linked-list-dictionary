import pandas as pd
import matplotlib.pyplot as plt

# Manually create the dataset
data = {
    "csv_size": [1, 2, 22, 1067],
    "linked_list": [0.00, 0.005, 0.011, 0.041],
    "patricia_tree": [0.003, 0.007, 0.017, 0.027]  # stripped 's' to store as float
}

df = pd.DataFrame(data)

# Plot
plt.figure(figsize=(8, 5))
plt.plot(df["csv_size"], df["linked_list"], marker='o', label="Linked List")
plt.plot(df["csv_size"], df["patricia_tree"], marker='o', label="Patricia Tree")

plt.title("CPU Time vs CSV Size")
plt.xlabel("CSV Size")
plt.ylabel("CPU Time (seconds)")# optional, makes large jumps in size clearer
plt.grid(True, which="both", linestyle="--", linewidth=0.5)
plt.legend()
plt.tight_layout()

plt.savefig("figures/cpu_time_vs_csv_size.png") 
