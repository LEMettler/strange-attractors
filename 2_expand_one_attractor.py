#
# Created on:  Thu Sep 19 2024
# By:  Lukas Mettler (lukas.mettler@student.kit.edu)
# https://github.com/LEMettler
#

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import py5
import subprocess as sp
import sys


def scale(arr, max_length):
    arr = (arr - np.min(arr))/np.max(arr-np.min(arr)) # [0, 1]

    scaling = 0.95*max_length
    offset = 0.025*max_length

    return arr*scaling + offset 


def get_input():
    # fallback if parameters are not provided
    run_name = input('Run name: ')
    attractor_index = input('Attractor index/name: ')
    n_steps = input('N steps: ')
    return run_name, attractor_index, n_steps


#####################################################################################

# get user input from argv
try:
    if len(sys.argv) == 4:  # exepct 3 arguments
        run_name = sys.argv[1]
        attractor_index = int(sys.argv[2])
        n_steps = int(sys.argv[3])
    else:
        print("Incorrect number of parameters. Falling back to user input.")
        run_name, attractor_index, n_steps = get_input()
except (ValueError, IndexError):
    print("Error parsing command-line arguments. Falling back to user input.")
    run_name, attractor_index, n_steps = get_input()



# run c++ script to further calculate
sp.run(["./cpp-scripts/extensive-attractor-calculation", run_name, str(attractor_index), str(n_steps)])


# drectories
dir_run = f'data/{run_name}'
dir_coeffs = dir_run + '/coeffs'
dir_points = dir_run + '/points'
dir_plots = dir_run + '/plots'
dir_py5_plot = dir_run + '/py5_plots'


# matplotlib version
dir_file = dir_points + f'/{attractor_index}exp.txt'
x_list, y_list = pd.read_csv(dir_file).to_numpy().T
plt.figure(figsize=(7,7))
plt.scatter(x_list, y_list, s=0.02, color='k')
plt.title(dir_file)
plt.xticks([])
plt.yticks([])

dir_fig = dir_plots + f'/{attractor_index}exp.jpg'
plt.savefig(dir_fig, dpi=150)
plt.close()



# py5 version

sp.run(["mkdir", dir_py5_plot])

def setup():
    py5.size(1000, 1000)
    py5.background(20)

    xvals = scale(x_list, py5.width)
    yvals = py5.height-scale(y_list, py5.height)
    points = np.array([xvals, yvals]).T

    py5.stroke(240)
    py5.points(points)

    py5.save(f'{dir_py5_plot}/{attractor_index}.png')

py5.run_sketch()



