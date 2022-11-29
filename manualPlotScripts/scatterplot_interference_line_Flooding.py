import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

sim_data = pd.read_csv("../output/Line_10_Interference_5ms_Flooding/results/results_1.csv")
pos_data = pd.read_csv("../examples/multicast/Line_10_Interference.csv")

sim_data['x'] = pos_data['x']
sim_data['y'] = pos_data['y']

sim_data['# Sent Packets'] = sim_data['Successfully Sent Data-Packets']
sim_data['PDR [%]'] = round(sim_data['Different Received Data Packets'] / 60 * 100, 1)
sim_data['PDR [%]'][0] = 100

inter_x = [ 495, 605]
inter_y = [ 0, 0]

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})
pal = sns.dark_palette("#69d", reverse=True, as_cmap=True)

#g = sns.FacetGrid(pos_data, col="x", row="Successfully Sent Packets")
#f = sns.relplot(x="x", y="y", size="Successfully Sent Bytes", sizes=(100, 1000), data=sim_data)
ax1 = sns.scatterplot(data=sim_data, x ='x', y = 'y', hue = "PDR [%]", size = "# Sent Packets", sizes=(50,500), hue_norm = (0, 100), size_norm = (0, 60), legend=True, linewidth=0, palette =pal)
sns.scatterplot(x=inter_x, y = inter_y, legend=False, linewidth=1,  palette = "orange" )
sns.scatterplot(x=inter_x, y = inter_y, legend=False, facecolors='orange', edgecolor='orange', size=(110))
sns.scatterplot(x=inter_x, y = inter_y, legend=False, s = 22000 ,facecolors='none',  linewidth=1, edgecolor="red" )

#ax1.ax_joint.plot([0],[0],'o',ms=60 , mec='r', mfc='none')

ax1.set(xlabel='X-Coordinate [m]', ylabel='Y-Coordinate [m]', title= "Flooding with Interference")
ax1.legend(loc='center left', bbox_to_anchor=(1, 0.5))

ax1.set_xlim((-50, 1030))
ax1.set_ylim((-20, 20))

ax1.set_xticks((sim_data['x']))
ax1.set_yticks((sim_data['y']))

ax1.figure.savefig("scatterplot_Intererence_Line_Flooding.pdf", bbox_inches='tight', dpi=300)
