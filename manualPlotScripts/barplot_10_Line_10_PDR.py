import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

df = pd.DataFrame({
    "Protocol" : ["Flooding", "1D0C", "2D0C", "1D1C", "2D1C", "2D2C", "4D1C"],
    "PDR [%]" : [25.7, 23.9, 45.2, 34.6, 66.3, 69.6, 92.2],
    "Energy/Packet [J]" : [4.51, 4.86, 2.57, 3.36, 1.76, 1.69, 1.30]
})

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid' : True})
h = sns.barplot(data=df, x ='Protocol', y = 'PDR [%]')
#ax2 = h.twinx()
#h = sns.barplot(data=graphdata, x ='Node', y = '# Sent Data Packets', hue = 'Packets', ax=ax2)


#h.legend_.set_title(None)
h.set(ylim=(0,100))

#plt.show()
h.figure.savefig("barplot_10_Line_PDR.pdf",  bbox_inches='tight', dpi=300)