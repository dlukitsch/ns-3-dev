import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

topology = [ "Line_3", "Line_10", "Line_100", "Square_2x2", "Square_5x5", "Square_15x15", "Circle_3", "Circle_10", "Random_Line_10", "Random_Line_100", "Random_Rectangle_5", "Random_Rectangle_10", "Random_Rectangle_100", "SquareLineSquare_15x15-15-15x15" ]
protocols = [ "Mpl_1D0C", "Mpl_1D1C", "Mpl_2D0C", "Mpl_2D1C", "Mpl_2D2C", "Flooding" ]
dataPathResults = "../output/"
dataPathModel = "../examples/multicast/"
fileEnding = "/results_mean.csv"

pos_data = pd.read_csv(dataPathModel + "Line_10" + ".csv")

sim_data_1d0c = pd.read_csv(dataPathResults + "Line_10_Mpl_1D0C" + fileEnding)
sim_data_1d0c['Protocol'] = "MPL_1D0C"
sim_data_1d0c['x'] = pos_data['x']
sim_data_1d0c['y'] = pos_data['y']

sim_data_1d1c = pd.read_csv(dataPathResults + "Line_10_Mpl_1D1C" + fileEnding)
sim_data_1d1c['Protocol'] = "MPL_1D1C"
sim_data_1d1c['x'] = pos_data['x']
sim_data_1d1c['y'] = pos_data['y']

sim_data_2d0c = pd.read_csv(dataPathResults + "Line_10_Mpl_2D0C" + fileEnding)
sim_data_2d0c['Protocol'] = "MPL_2D0C"
sim_data_2d0c['x'] = pos_data['x']
sim_data_2d0c['y'] = pos_data['y']

sim_data_2d1c = pd.read_csv(dataPathResults + "Line_10_Mpl_2D1C" + fileEnding)
sim_data_2d1c['Protocol'] = "MPL_2D1C"
sim_data_2d1c['x'] = pos_data['x']
sim_data_2d1c['y'] = pos_data['y']

sim_data_2d2c = pd.read_csv(dataPathResults + "Line_10_Mpl_2D2C" + fileEnding)
sim_data_2d2c['Protocol'] = "MPL_2D2C"
sim_data_2d2c['x'] = pos_data['x']
sim_data_2d2c['y'] = pos_data['y']

sim_data_flooding = pd.read_csv(dataPathResults + "Line_10_Flooding" + fileEnding)
sim_data_flooding['Protocol'] = "Flooding"
sim_data_flooding['x'] = pos_data['x']
sim_data_flooding['y'] = pos_data['y']


data = pd.concat([sim_data_1d0c, sim_data_1d1c, sim_data_2d0c, sim_data_2d1c, sim_data_2d2c, sim_data_flooding])


#for col in sim_data.columns:
#    print(col)

data['Different Received Data Packets'] /= 60; 

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})

#g = sns.FacetGrid(pos_data, col="x", row="Successfully Sent Packets")
#f = sns.relplot(x="x", y="y", size="Successfully Sent Bytes", sizes=(100, 1000), data=sim_data)
#h = sns.scatterplot(data=sim_data, x ='x', y = 'y', size='Successfully Sent Packets', sizes=(1100, 1100), legend=False, linewidth=0)

fig = sns.barplot(x='x', y='Different Received Data Packets', hue='Protocol', data=data)

plt.show()

#g.savefig("FacetGrid.pdf", bbox_inches='tight', dpi = 300)
#f.savefig("relplot.pdf",  bbox_inches='tight', dpi=300)
fig.figure.savefig("successRatePerProtocolAndTopology.pdf",  bbox_inches='tight', dpi=300)