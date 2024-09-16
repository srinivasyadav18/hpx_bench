import pandas as pd
import matplotlib.pyplot as plt

# Load the sequential data
seq = pd.read_csv('seq.csv')

# Dictionary of backends and corresponding CSV files
backends = {
    'HPX': 'hpx.csv',
    # 'HPX Fork-Join': 'hpx_forkjoin.csv',
    # 'C++ stdpar': 'std.csv',
    'TBB': 'tbb.csv',
    'OpenMP': 'omp.csv',
    'Taskflow': 'taskflow.csv'
}

# Function to calculate speedup for each backend and size
def calculate_speedup(backend_name, backend_data, seq_time):
    # For each thread count, calculate speedup as seq_time / parallel_time
    return seq_time / backend_data['time']

# Function to plot the speedup for all backends and sizes
def plot_speedup():
    sizes = seq['size'].unique()  # Get all unique problem sizes
    
    # Create square-shaped plots
    fig, axes = plt.subplots(1, len(sizes), figsize=(6 * len(sizes), 6))  # 6x6 inches per subplot
    plt.suptitle("for each compute bound")
    
    for i, size in enumerate(sizes):
        ax = axes[i]
        
        # Get the sequential time for the current size
        seq_time = seq[seq['size'] == size]['time'].values[0]
        
        for backend_name, backend_file in backends.items():
            # Load backend data from CSV
            backend_data = pd.read_csv(backend_file)
            
            # Filter backend data by the current size
            backend_data_size = backend_data[backend_data['size'] == size]
            
            # Calculate speedup for this backend
            speedup = calculate_speedup(backend_name, backend_data_size, seq_time)
            speedup /= 32
            # Plot the speedup for this backend
            ax.plot(backend_data_size['threads'], speedup, marker='o', label=backend_name)
        
        ax.set_title(f'Problem Size = {size}')
        ax.set_xticks([1, 2, 4, 8, 16, 32])
        ax.set_xlabel('Threads')
        ax.set_ylabel('Speedup')
        ax.legend()
        ax.grid(True)
        
        # Ensure the plot is square-shaped
        # ax.set_aspect('equal', adjustable='box')
        
        # Set custom x-ticks in ascending order
        threads = sorted(backend_data_size['threads'].unique())
        print(threads)
        ax.set_xticks(threads)
        ax.set_xticklabels([str(t) for t in threads], fontsize=12)
    # Add a main title for the complete plot
    plt.suptitle('Speedup Comparison for Different Backends and Sizes', fontsize=12)
    
    plt.tight_layout(pad=5)  # Increase padding between plots for better visibility
    plt.subplots_adjust(top=0.9)  # Adjust the top t
    plt.savefig("plot.png")
# Call the function to plot speedup
plot_speedup()
