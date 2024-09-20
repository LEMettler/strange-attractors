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
import time

colorful = False
plot_only = True
recompile = False
#img_width, img_height = 1920, 1080
img_width, img_height = 2000, 1000


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

if not plot_only:

    #recompile c++ script with function
    if recompile: 
        cmd = "g++ cpp-scripts/extensive-attractor-calculation.cc function.cc -o cpp-scripts/extensive-attractor-calculation"  
        try:
            result = sp.run(cmd, shell=True, check=True, capture_output=True, text=True)
            print('Success compiling function!')
        except sp.CalledProcessError as e:
            print(f"Error recompiling: {e}")
            print("Output: ", e.stdout)
            print("Error: ", e.stderr)
            sys.exit(1)

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



def color_noise(x,y):
    rgb = []
    #for speed in [0.004, 0.002, 0.0021]:
    for speed in [0.009, 0.0004, 0.0005]:
        rgb.append(255*py5.noise(x*speed,y*speed))
    return rgb
# py5 version

sp.run(["mkdir", dir_py5_plot])

def setup():
    py5.size(img_width, img_height)
    py5.noise_seed(int(time.time()))

    py5.background(20)

    xvals = scale(x_list, py5.width)
    yvals = py5.height-scale(y_list, py5.height)

    if colorful:

        for xv, yv in zip(xvals, yvals):
            rgb = color_noise(xv, yv)
            py5.no_stroke()
            py5.stroke(*rgb, 70)
            py5.point(xv, yv)
        py5.save(f'{dir_py5_plot}/{attractor_index}-color.png')

    else:
        py5.stroke(220, 70)
        points = np.array([xvals, yvals]).T
        py5.points(points)
        py5.save(f'{dir_py5_plot}/{attractor_index}-bw.png')

py5.run_sketch(block=False)




