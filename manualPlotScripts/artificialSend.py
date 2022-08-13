import csv
from turtle import title
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

sim_datay = [ 5.9, 5.9,     8.75, 8.75, 5.9,    5.9,   10.1,   10.1,  5.9, 5.9]
sim_datax = [ -2,  -0.0001, 0,    2,    2.001,  2.9999, 3,      5,    5.001, 7]

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})

#g = sns.FacetGrid(pos_data, col="x", row="Successfully Sent Packets")
#f = sns.relplot(x="x", y="y", size="Successfully Sent Bytes", sizes=(100, 1000), data=sim_data)
h = sns.lineplot(x =sim_datax, y = sim_datay)
h.set(xlabel='Time [ms]', ylabel='Consumed Current [mA]', title= "NS3 Current Consumption Model",)

h.set_xlim((-1.5, 6.5))
h.set_ylim((4.5, 12))

plt.show()

#g.savefig("FacetGrid.pdf", bbox_inches='tight', dpi = 300)
#f.savefig("relplot.pdf",  bbox_inches='tight', dpi=300)
h.figure.savefig("artificialSendWithReceive.svg", bbox_inches='tight', dpi=1200)