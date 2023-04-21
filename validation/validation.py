import subprocess
import seaborn as sns
import pandas as pd
import argparse
import matplotlib.pyplot as plt
import os


def plot_energy(df, title):
    custom_palette = ["#000000", "#7aff00", "#00BFFF"] # a list of custom colors in hex format
    sns.set(style="whitegrid", palette=custom_palette)
    sns.lineplot(data=df[['Kinetic Energy', 'Potential Energy', 'Total Energy']], dashes=False)
    plt.xlabel('Iteration')
    plt.ylabel('Energy')
    plt.title(title)
    plt.show()



# Create an argument parser to handle the program path argument
parser = argparse.ArgumentParser(description='Validation of ParticleSolver')
parser.add_argument('path', type=str, help='Path to the compiled C++ program')
args = parser.parse_args()
program_path = args.path

# Run the C++ program and capture the output
program_dir = os.path.dirname(program_path)
program_name = os.path.basename(program_path)
os.chdir(program_dir)

output = subprocess.check_output([os.path.join('.', program_name)])
output = output.decode('utf-8').split('\n')

# Parse the output
energy_cpu_sequential = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
energy_cpu_parallel = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
energy_gpu = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])

for line in output:
    if "CPU sequential" in line:
        energy_df = energy_cpu_sequential
    elif "CPU parallel" in line:
        plot_energy(energy_df, 'CPU sequential')
        energy_df = energy_cpu_parallel
    elif "GPU" in line:
        plot_energy(energy_df, 'CPU parallel')
        energy_df = energy_gpu
    elif "Iteration" in line:
        iteration = int(line.split(': ')[1])
    elif "Kinetic energy" in line:
        kinetic_energy = float(line.split(': ')[1])
    elif "Potential energy" in line:
        potential_energy = float(line.split(': ')[1])
    elif "Total energy" in line:
        total_energy = float(line.split(': ')[1])
        # Store the results in the DataFrame
        new_row = pd.DataFrame({'Iteration': iteration, 'Kinetic Energy': kinetic_energy,
                                'Potential Energy': potential_energy, 'Total Energy': total_energy}, index=[0])
        energy_df = pd.concat([energy_df, new_row], ignore_index=True)


plot_energy(energy_df, 'GPU')




