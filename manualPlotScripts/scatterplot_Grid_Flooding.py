import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator

sns.set_theme(style="dark")

sim_data = pd.read_csv("../output/Square_5x5_Flooding/results_median.csv")
pos_data = pd.read_csv("../examples/multicast/Square_5x5.csv")

sim_data['x'] = pos_data['x']
sim_data['y'] = pos_data['y']

sim_data['# Sent Packets'] = sim_data['Successfully Sent Data-Packets']
sim_data['PDR [%]'] = sim_data['Successfully Received Data-Packets'] / sim_data['# Sent Packets'][0] * 100

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})
pal = sns.dark_palette("#69d", reverse=True, as_cmap=True)

#g = sns.FacetGrid(pos_data, col="x", row="Successfully Sent Packets")
#f = sns.relplot(x="x", y="y", size="Successfully Sent Bytes", sizes=(100, 1000), data=sim_data)
h = sns.scatterplot(data=sim_data, x ='x', y = 'y', hue="# Sent Packets", size="# Sent Packets", sizes=(100,500), hue_norm = (0, 60), s=400, legend=True, linewidth=0, palette = pal)
h.set(xlabel='X-Coordinate [m]', ylabel='Y-Coordinate [m]')
h.legend(loc='center left', bbox_to_anchor=(1, 0.5))
h.yaxis.set_major_locator(MultipleLocator(110))
h.xaxis.set_major_locator(MultipleLocator(110))

h.legend_.set_title("# Sent Packets")
h.set_xlim((-30, 500))
h.set_ylim((-30, 500))

for line in range(0, 1):
     h.text(sim_data['x'][line]-20, sim_data['y'][line]+25, "Sender", horizontalalignment='left', size='small', color='black')

#h.set_xticks((sim_data['x']))
#h.set_yticks((sim_data['y']))

#g.savefig("FacetGrid.pdf", bbox_inches='tight', dpi = 300)
#f.savefig("relplot.pdf",  bbox_inches='tight', dpi=300)
h.figure.savefig("scatterplot_Sqare_5x5_Flooding.pdf",bbox_inches='tight', dpi=300)
