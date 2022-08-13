import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

sim_data = pd.read_csv("../output/Square_5x5_Mpl_1D0C_battery/results_median.csv")
pos_data = pd.read_csv("../examples/multicast/Square_5x5.csv")

sim_data['x'] = pos_data['x']
sim_data['y'] = pos_data['y']

for col in sim_data.columns:
    print(col)

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})
pal = sns.dark_palette("#69d", reverse=True, as_cmap=True)

#g = sns.FacetGrid(pos_data, col="x", row="Successfully Sent Packets")
#f = sns.relplot(x="x", y="y", size="Successfully Sent Bytes", sizes=(100, 1000), data=sim_data)
h = sns.scatterplot(data=sim_data, x ='x', y = 'y', size='Node LifeTime [s]', hue="Successfully Sent Packets" ,sizes=(50, 500), legend=True, linewidth=0,  palette = pal)
h.set(xlabel='X-Coordinate [m]', ylabel='Y-Coordinate [m]', title= "MPL 1D0C Node Lifetime")
h.legend(loc='center left', bbox_to_anchor=(1, 0.5))


h.set_xlim((-50, 500))
h.set_ylim((-50, 500))

h.set_xticks((sim_data['x']))
h.set_yticks((sim_data['y']))


plt.tight_layout()
plt.plot()

#g.savefig("FacetGrid.pdf", bbox_inches='tight', dpi = 300)
#f.savefig("relplot.pdf",  bbox_inches='tight', dpi=300)
h.figure.savefig("scatterplot_LifeTime_Square.svg",  dpi=1200)