import subprocess
import seaborn as sns
import pandas as pd
import argparse
import matplotlib.pyplot as plt

# Create an argument parser to handle the program path argument
parser = argparse.ArgumentParser(description='Run N-body simulation and analyze performance')
parser.add_argument('path', type=str, help='Path to the compiled C++ program')
parser.add_argument('--algorithms', type=int, nargs='+', choices=[1, 2, 3, 4, 5],
                    help='Algorithms to test (choose from 1, 2, 3, 4, 5)')
args = parser.parse_args()
program_path = args.path


# Set the algorithms to test
algorithms = [1, 2, 3, 4, 5] if args.algorithms is None else args.algorithms


# Set the number of bodies to test
bodies = [2**i for i in range(2, 12, 1)]




# Create a DataFrame to store the results
df = pd.DataFrame(columns=['Algorithm', 'Bodies', 'Elapsed Time', 'Average FPS', 'Average Frame Time', 'Iterations', 'Total iterations in 1 second'])


# Loop through the algorithms and number of bodies
for algorithm in algorithms:
    for n in bodies:
        # Run the C++ program and capture the output
        output = subprocess.check_output([program_path, '-v', str(algorithm), '-n', str(n), '-b', '-i' , '1'])
        output = output.decode('utf-8').split('\n')

        print(output)
        # Parse the output
        for line in output:
            if "Total elapsed time" in line:
                elapsed_time = float(line.split(': ')[1].split(' ')[0])
            elif "Average FPS" in line:
                average_fps = float(line.split(': ')[1].split(' ')[0])
            elif "Average Frame Time" in line:
                average_frame_time = float(line.split(': ')[1].split(' ')[0])
            elif "Total iterations" in line:
                iterations = int(line.split(': ')[1].split(' ')[0])

        # Store the results in the DataFrame
        new_row = pd.DataFrame({'Algorithm': algorithm, 'Bodies': n, 'Elapsed Time': elapsed_time,
                                'Average FPS': average_fps, 'Average Frame Time': average_frame_time,
                                'Iterations': iterations, 'Total iterations in 1 second': average_fps * n}, index=[0])
        df = pd.concat([df, new_row], ignore_index=True)

algorithm_names = {
    1: "Sequential Particle-particle CPU",
    2: "Parallel Particle-particle CPU",
    3: "Parallel Particle-particle GPU",
    4: "Fast Particle-Particle GPU",
    5: "Parallel Grid CPU"
}
df['Algorithm'] = df['Algorithm'].replace(algorithm_names)

def plot_results(x, y, hue, kind, ylabel):
    custom_palette = ["#000000", "#7aff00", "#00BFFF", ["#AC5B27"]] # a list of custom colors in hex format
    sns.set(style="whitegrid")
    if kind == "line":
        sns.lineplot(x=x, y=y, hue=hue, data=df, palette=custom_palette)
    elif kind == "bar":
        g = sns.catplot(x=x, y=y, hue=hue, data=df, kind=kind, palette = custom_palette)
        g.despine(left=True)
        g.set_ylabels(ylabel)
        g.set_xlabels("Number of Bodies")
    plt.xlabel("Number of Bodies")
    plt.ylabel(ylabel)
    plt.show()

plot_results(x="Bodies", y="Average Frame Time", hue="Algorithm", kind="bar", ylabel="Average Frame Time (ms)\nLess is better")
plot_results(x="Bodies", y="Average Frame Time", hue="Algorithm", kind="line", ylabel="Average Frame Time (ms)\nLess is better")
plot_results(x="Bodies", y="Total iterations in 1 second", hue="Algorithm", kind="bar", ylabel="Particle updates per second\n More is better")

#plot_results(x="Bodies", y="Average FPS", hue="Algorithm", kind="bar", ylabel="Average FPS\nMore is better")
#plot_results(x="Bodies", y="Iterations", hue="Algorithm", kind="line", ylabel="Iterations in 8 seconds\nMore is better")
