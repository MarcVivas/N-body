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
parser.add_argument('--softening', type=float, help='Squared softening')
parser.add_argument('--step-size', type=float, help='Step size')
args = parser.parse_args()
program_path = args.path

step_size = args.step_size if args.step_size else 0.000035
squared_softening = args.softening if args.softening else 0.00135

print(f'Step size: {step_size}' )
print(f"Squared softening: {squared_softening}" )

# Run the C++ program and capture the output
program_dir = os.path.dirname(program_path)
program_name = os.path.basename(program_path)
os.chdir(program_dir)


output = subprocess.check_output([os.path.join('.', program_name), '-s', str(squared_softening), '-t', str(step_size)], stderr=subprocess.STDOUT)
output = output.decode('utf-8').split('\n')


# Parse the output
energy_cpu_sequential = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
energy_cpu_parallel = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
energy_gpu = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])

print("Executing tests...")

for line in output:
    if "Energy test: CPU sequential" in line:
        energy_df = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
    elif "Energy test: GPU optimized" in line:
        plot_energy(energy_df, 'GPU')
        energy_df = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
    elif "Energy test: CPU parallel grid" in line:
        plot_energy(energy_df, 'GPU optimized')
        energy_df = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
    elif "Energy test: CPU parallel" in line:
        plot_energy(energy_df, 'CPU sequential')
        energy_df = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
    elif "Energy test: GPU" in line:
        plot_energy(energy_df, 'CPU parallel')
        energy_df = pd.DataFrame(columns=['Iteration', 'Kinetic Energy', 'Potential Energy', 'Total Energy'])
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


plot_energy(energy_df, 'CPU parallel grid')




