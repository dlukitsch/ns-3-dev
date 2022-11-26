import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

sns.set_theme(style="dark")

sim_data_square = pd.read_csv("../output/Square_5x5_Flooding/results_median.csv")
pos_data_square = pd.read_csv("../examples/multicast/Square_5x5.csv")

sim_data_square['x'] = pos_data_square['x']
sim_data_square['y'] = pos_data_square['y']

sim_data_square['Node LifeTime [ms]'] = sim_data_square['Successfully Sent Data-Packets']






#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})
pal = sns.dark_palette("#69d", reverse=True, as_cmap=True)

h = sns.scatterplot(data=sim_data_square, x ='x', y = 'y', hue="Node LifeTime [ms]", size="Node LifeTime [ms]", sizes=(100,500), hue_norm = (0, 60), s=400, legend=True, linewidth=0, palette = pal)
h.set(xlabel='X-Coordinate [m]', ylabel='Y-Coordinate [m]')
h.legend(loc='center left', bbox_to_anchor=(1, 0.5))
h.yaxis.set_major_locator(MultipleLocator(110))
h.xaxis.set_major_locator(MultipleLocator(110))

h.legend_.set_title("# Sent Packets")
h.set_xlim((-30, 500))
h.set_ylim((-30, 500))
for line in range(0, 1):
     h.text(sim_data_square['x'][line]-20, sim_data_square['y'][line]+25, "Sender", horizontalalignment='left', size='small', color='black')
h.figure.savefig("scatterplot_Sqare_5x5_Lifetime.pdf",bbox_inches='tight', dpi=300)
