import sys
import pandas as pd
import matplotlib.pyplot as plt

# use plot graph to visualize the usage of memory
def visualize_memory(csv1, csv2 = None):
    '''Visualize memory usage from two CSV files.'''


    df1 = pd.read_csv(csv1)
    if csv2:
        df2 = pd.read_csv(csv2)

    # Remove file extensions for labels
    import os
    label1 = os.path.splitext(os.path.basename(csv1))[0]
    
    # Two files comparing
    if csv2:
        # Use the maximum time from both files as 100%
        global_min_time = min(df1['time'].min(), df2['time'].min())
        global_max_time = max(df1['time'].max(), df2['time'].max())
        time_range = global_max_time - global_min_time
        
        # Normalize both files using the same global scale
        time1_normalized = (df1['time'] - global_min_time) / time_range * 100
        time2_normalized = (df2['time'] - global_min_time) / time_range * 100
    else:
        # Single file normalization
        time1_normalized = (df1['time'] - df1['time'].min()) / (df1['time'].max() - df1['time'].min()) * 100
    
    plt.figure(figsize=(10, 6))
    plt.plot(time1_normalized, df1['heap'] / 1024, label=label1, color='orange', linewidth=2.5)
    if csv2:
        label2 = os.path.splitext(os.path.basename(csv2))[0]
        plt.plot(time2_normalized, df2['heap'] / 1024, label=label2, color='blue', linewidth=2.5)

    plt.xlabel('Running percentage (%)')
    plt.ylabel('Memory (KB)')
    plt.title('Memory Usage Over Time')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    if csv2:
        plt.savefig('memory_usage_comparison.png')
    else:
        plt.savefig('memory_usage.png')




if __name__ == "__main__":

    if len(sys.argv) == 3:
        csv_file1 = sys.argv[1]
        csv_file2 = sys.argv[2]
        visualize_memory(csv_file1, csv_file2)
    if len(sys.argv) == 2:
        csv_file1 = sys.argv[1]
        visualize_memory(csv_file1)