import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

sim_data = pd.read_csv("../output/Square_5x5_Interference_5ms_Mpl_2D1C/results_mean.csv")
pos_data = pd.read_csv("../examples/multicast/Square_5x5_Interference.csv")

sim_data['x'] = pos_data['x']
sim_data['y'] = pos_data['y']

sim_data['# Sent Packets'] = sim_data['Successfully Sent Data-Packets']
print(sim_data['# Sent Packets'][0])
sim_data['PDR [%]'] = sim_data['Successfully Received Data-Packets'] / sim_data['# Sent Packets'][0] * 100
print(sim_data['PDR [%]'])

inter_x = [ 165, 275]
inter_y = [ 220, 220]

for col in sim_data.columns:
    print(col)

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})

colors = ["#242629"] #['#6699dd', '#5982b9', '#4c6b95', '#3e5471', '#313d4d', '#242629']
pal_or = sns.dark_palette("#69d", reverse=True, as_cmap=True)
pal = sns.set_palette(colors)


fig = plt.figure()

#g = sns.FacetGrid(pos_data, col="x", row="Successfully Sent Packets")
#f = sns.relplot(x="x", y="y", size="Successfully Sent Bytes", sizes=(100, 1000), data=sim_data)
ax1 = sns.scatterplot(data=sim_data, x ='x', y = 'y', hue = "PDR [%]", size = "# Sent Packets", sizes=(50,500), legend=True, hue_norm = (0, 500), size_norm = (40, 120) ,linewidth=0, palette =pal_or)
sns.scatterplot(x=inter_x, y = inter_y, legend=False, linewidth=1,  edgecolor="orange", facecolors="orange",  size=(110) )
sns.scatterplot(x=inter_x, y = inter_y, legend=False, s = 22000 ,facecolors='none',  linewidth=1, edgecolor="red" )

#ax1.ax_joint.plot([0],[0],'o',ms=60 , mec='r', mfc='none')

ax1.set(xlabel='X-Coordinate [m]', ylabel='Y-Coordinate [m]', title= "MPL 2D1C with Interference")
ax1.legend(loc='center left', bbox_to_anchor=(1, 0.5))


ax1.set_xlim((-50, 490))
ax1.set_ylim((-50, 490))

ax1.set_xticks((sim_data['x']))
ax1.set_yticks((sim_data['y']))

plt.tight_layout()
plt.plot()

#g.savefig("FacetGrid.pdf", bbox_inches='tight', dpi = 300)
#f.savefig("relplot.pdf",  bbox_inches='tight', dpi=300)
ax1.figure.savefig("scatterplot_Intererence_Square_2D1C.svg", dpi=1200)