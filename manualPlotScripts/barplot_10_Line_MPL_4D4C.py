import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

sim_data = pd.read_csv("../output/Line_10_Mpl_4D4C/results_median.csv")
pos_data = pd.read_csv("../examples/multicast/Line_10.csv")
sim_data['x'] = pos_data['x']
sim_data['y'] = pos_data['y'] 

#for col in sim_data.columns:
#    print(col)

rows_list = []
for index, row in sim_data.iterrows():   
    rows_list.append( { 'Node':int(row['Node ID']), '# Sent Packets':row['Successfully Sent Packets'], 'Packets':'Overall Packets' })
    rows_list.append( { 'Node':int(row['Node ID']), '# Sent Packets':row['Successfully Sent Data-Packets'], 'Packets':'Data Packets' })
    rows_list.append( { 'Node':int(row['Node ID']), '# Sent Packets':row['Successfully Sent Control-Packets'], 'Packets':'Control Packets' })

graphdata = pd.DataFrame(rows_list)

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})
h = sns.barplot(data=graphdata, x ='Node', y = '# Sent Packets', hue = 'Packets')
h.legend_.set_title(None)

for i in h.containers:
    h.bar_label(i, size=5.5)

#plt.show()
h.figure.savefig("barplot_10_Line_MPL_4D4C.pdf",  bbox_inches='tight', dpi=300)