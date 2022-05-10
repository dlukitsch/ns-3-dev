import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

sim_data = pd.read_csv("../output/Line_10_Mpl_1D1C/results_median.csv")
pos_data = pd.read_csv("../examples/multicast/Line_10.csv")

sim_data['x'] = pos_data['x']
sim_data['y'] = pos_data['y']

for col in sim_data.columns:
    print(col)

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})

#g = sns.FacetGrid(pos_data, col="x", row="Successfully Sent Packets")
#f = sns.relplot(x="x", y="y", size="Successfully Sent Bytes", sizes=(100, 1000), data=sim_data)
h = sns.scatterplot(data=sim_data, x ='x', y = 'y', size='Successfully Sent Packets', sizes=(1100, 1100), legend=False, linewidth=0)

plt.show()

#g.savefig("FacetGrid.pdf", bbox_inches='tight', dpi = 300)
#f.savefig("relplot.pdf",  bbox_inches='tight', dpi=300)
h.figure.savefig("scatterplot.pdf",  bbox_inches='tight', dpi=300)