#
# Created on:  Thu Sep 19 2024
# By:  Lukas Mettler
# https://github.com/LEMettler
#

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import subprocess as sp
import sys


recompile = True


def get_input():
    # fallback if parameters are not provided
    run_name = input('Run name: ')
    n_attractors = int(input('Number of attractors: '))
    n_steps = int(input('Number of steps: '))
    return run_name, n_attractors, n_steps



# get user input from argv
try:
    if len(sys.argv) == 4:  # exepct 3 arguments
        run_name = sys.argv[1]
        n_attractors = int(sys.argv[2])
        n_steps = int(sys.argv[3])
    else:
        print("Incorrect number of parameters. Falling back to user input.")
        run_name, n_attractors, n_steps = get_input()
except (ValueError, IndexError):
    print("Error parsing command-line arguments. Falling back to user input.")
    run_name, n_attractors, n_steps = get_input()


dir_run = f'data/{run_name}'
dir_coeffs =dir_run + '/coeffs'
dir_points =dir_run + '/points'
dir_plots =dir_run + '/plots'


# create directory
sp.run(["mkdir", dir_run, dir_coeffs, dir_points, dir_plots])


#recompile c++ script with function
if recompile: 
    cmd = "g++ cpp-scripts/attractor-calculation.cc function.cc -o cpp-scripts/attractor-calculation"  
    try:
        result = sp.run(cmd, shell=True, check=True, capture_output=True, text=True)
        print('Success compiling function!')
    except sp.CalledProcessError as e:
        print(f"Error recompiling: {e}")
        print("Output: ", e.stdout)
        print("Error: ", e.stderr)
        sys.exit(1)
        

# call script
sp.run(["./cpp-scripts/attractor-calculation", run_name, str(n_attractors), str(n_steps)])
print('All attractors generated!')

# plot attractors
for i in range(n_attractors):
    dir_file = dir_points + f'/{i}.txt'
    x, y = pd.read_csv(dir_file).to_numpy().T
    plt.figure(figsize=(7,7))
    plt.scatter(x,y, s=0.02, color='k')
    plt.title(dir_file)
    plt.xticks([])
    plt.yticks([])

    dir_fig = dir_plots + f'/{i}.jpg'
    plt.savefig(dir_fig, dpi=150)
    plt.close()

    print(f'{i}/{n_attractors} plotted!')

