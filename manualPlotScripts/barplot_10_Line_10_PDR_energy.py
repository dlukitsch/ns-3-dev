import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

df = pd.DataFrame({
    "Protocol" : ["Flooding", "1D0C", "2D0C", "3D0C",
                  "1D1C", "2D1C", "2D2C", "4D1C", "6D1C", "7D1C",
                
                  "Flooding", "1D0C", "2D0C", "3D0C",
                  "1D1C", "2D1C", "2D2C", "4D1C", "6D1C", "7D1C" ],
    "Delivered Messages [%]" : [25.7, 23.9, 45.2, 62.4,
                                34.6, 66.3, 69.6, 92.2, 100, 100,
                                
                                24.3*4, 23.6*4, 13.9*4, 10.2*4,
                                18.2*4, 9.5*4, 8.4*4, 7.0*4, 6.5*4, 6.6*4 ],
   # "Energy / Delivered Data Packet [J]" : [4.51, 4.86, 2.57, 3.36, 1.76, 1.69, 1.30],

    'Type' : ['Delivered Messages [%]', 'Delivered Messages [%]', 'Delivered Messages [%]', 'Delivered Messages [%]',
              'Delivered Messages [%]', 'Delivered Messages [%]', 'Delivered Messages [%]', 'Delivered Messages [%]', 'Delivered Messages [%]', 'Delivered Messages [%]',
              
              'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]',
              'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]', 'Energy per Delivered Messages [J]' ]
})

#cmap = sns.cubehelix_palette(rot=-.2, as_cmap=True)
sns.set(style="ticks")
sns.set(rc={'figure.figsize':(11.7,8.27)})
sns.set(font_scale=1.5)
 
sns.set_style("darkgrid", {'axes.grid' : False})
h = sns.barplot(data=df, x ='Protocol', y = 'Delivered Messages [%]', hue="Type")
ax2 = h.twinx()
ax2.set_ylabel('Energy per Delivered Messages [J]')
h.set_xlabel(None)
ax2.set_ylim(0,25)
h.legend_.set_title(None)
h.legend_._set_loc(9)
#h.set(ylim=(0,100))

#plt.show()
h.figure.savefig("barplot_10_Line_PDR_Energy.pdf",  bbox_inches='tight', dpi=300)