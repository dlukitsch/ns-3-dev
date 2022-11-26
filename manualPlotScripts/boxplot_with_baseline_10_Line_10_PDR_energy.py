import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
sns.set_theme(style="dark")

dm = 'Delivered Messages [%]'
epdm = 'Energy per Delivered Messages [J]'
epdmwob = 'Energy without Baseline per Delivered Messages [J]'

data1 = [
    ["Flooding", 32],           ["1D0C", 29.3],         ["2D0C", 61.7],         ["3D0C", 80.2],         ["1D1C", 54.4],         ["2D1C", 83.9],         ["2D2C", 85.6],         ["4D1C", 95.6],         ["6D1C", 100],         ["7D1C", 100],
    ["Flooding", 31.9],         ["1D0C", 25.9],         ["2D0C", 59.6],         ["3D0C", 86.7],         ["1D1C", 52.2],         ["2D1C", 83.1],         ["2D2C", 84.1],         ["4D1C", 94.3],         ["6D1C", 100],         ["7D1C", 100],
    ["Flooding", 35.7],         ["1D0C", 29.1],         ["2D0C", 62.8],         ["3D0C", 74.1],         ["1D1C", 55.0],         ["2D1C", 84.3],         ["2D2C", 83.5],         ["4D1C", 97.2],         ["6D1C", 100],         ["7D1C", 100],
    ["Flooding", 34.4],         ["1D0C", 24.8],         ["2D0C", 53.0],         ["3D0C", 80.4],         ["1D1C", 48.7],         ["2D1C", 86.1],         ["2D2C", 84.3],         ["4D1C", 95.6],         ["6D1C", 100],         ["7D1C", 100],
    ["Flooding", 28.9],         ["1D0C", 26.1],         ["2D0C", 56.3],         ["3D0C", 76.5],         ["1D1C", 61.1],         ["2D1C", 77.2],         ["2D2C", 85.0],         ["4D1C", 94.8],         ["6D1C", 100],         ["7D1C", 100]
]

data2 = [
    ["Flooding", 19.6], ["1D0C", 21.4], ["2D0C", 10.3], ["3D0C", 7.9], ["1D1C", 11.6], ["2D1C", 7.7], ["2D2C", 7.5], ["4D1C", 6.7], ["6D1C", 6.6], ["7D1C", 6.6],
    ["Flooding", 19.7], ["1D0C", 24.2], ["2D0C", 10.7], ["3D0C", 7.4], ["1D1C", 12.1], ["2D1C", 7.8], ["2D2C", 7.5], ["4D1C", 7.0], ["6D1C", 6.5], ["7D1C", 6.6],
    ["Flooding", 17.5], ["1D0C", 21.6], ["2D0C", 10.0], ["3D0C", 8.7], ["1D1C", 11.5], ["2D1C", 7.6], ["2D2C", 7.7], ["4D1C", 6.7], ["6D1C", 6.5], ["7D1C", 6.5],
    ["Flooding", 18.2], ["1D0C", 25.3], ["2D0C", 11.9], ["3D0C", 8.0], ["1D1C", 13.0], ["2D1C", 7.4], ["2D2C", 7.7], ["4D1C", 6.8], ["6D1C", 6.6], ["7D1C", 6.6],
    ["Flooding", 21.7], ["1D0C", 24.1], ["2D0C", 11.3], ["3D0C", 8.3], ["1D1C", 10.6], ["2D1C", 8.3], ["2D2C", 7.6], ["4D1C", 6.8], ["6D1C", 6.5], ["7D1C", 6.6]
]

data3 = [
    ["Flooding", 0.0003], ["1D0C", 0.0039], ["2D0C", 0.1214], ["3D0C", 0.1189], ["1D1C", 0.0765], ["2D1C", 0.2438], ["2D2C", 0.2161], ["4D1C", 0.1394], ["6D1C", 0.2980], ["7D1C", 0.2889],
    ["Flooding", 0.0003], ["1D0C", 0.0644], ["2D0C", 0.1661], ["3D0C", 0.1229], ["1D1C", 0.1187], ["2D1C", 0.2376], ["2D2C", 0.0917], ["4D1C", 0.3360], ["6D1C", 0.2682], ["7D1C", 0.2959],
    ["Flooding", 0.0003], ["1D0C", 0.0872], ["2D0C", 0.0381], ["3D0C", 0.2373], ["1D1C", 0.1079], ["2D1C", 0.1625], ["2D2C", 0.1628], ["4D1C", 0.2900], ["6D1C", 0.2361], ["7D1C", 0.2723],
    ["Flooding", 0.0003], ["1D0C", 0.0314], ["2D0C", 0.0983], ["3D0C", 0.2361], ["1D1C", 0.0922], ["2D1C", 0.1519], ["2D2C", 0.2449], ["4D1C", 0.2562], ["6D1C", 0.2957], ["7D1C", 0.3043],
    ["Flooding", 0.0005], ["1D0C", 0.1004], ["2D0C", 0.1795], ["3D0C", 0.1183], ["1D1C", 0.3194], ["2D1C", 0.1803], ["2D2C", 0.2322], ["4D1C", 0.1709], ["6D1C", 0.2345], ["7D1C", 0.3189]
]

df1 = pd.DataFrame(data1, columns=['Protocol', dm])
df2 = pd.DataFrame(data2, columns=['Protocol', epdm])
df3 = pd.DataFrame(data3, columns=['Protocol', epdmwob])

sns.set(style="ticks")
sns.set(rc={'figure.figsize':(11.7,8.27)})
sns.set(font_scale=1.5)
sns.set_style("darkgrid", {'axes.grid' : True})


plt.figure()
h = sns.boxplot(data=df1, x ='Protocol', y = dm, orient='v')
h.set_xlabel(None)
h.figure.savefig("boxplot_with_baseline_10_Line_PDR.pdf",  bbox_inches='tight', dpi=300)
 
plt.figure()
f = sns.boxplot(data=df2, x ='Protocol', y = epdm, orient='v')
f.set_xlabel(None)
f.figure.savefig("boxplot_with_baseline_10_Line_PDR_Energy.pdf",  bbox_inches='tight', dpi=300)

plt.figure()
f = sns.boxplot(data=df3, x ='Protocol', y = epdmwob, orient='v')
f.set_xlabel(None)
f.figure.savefig("boxplot_without_baseline_10_Line_PDR_Energy.pdf",  bbox_inches='tight', dpi=300)
