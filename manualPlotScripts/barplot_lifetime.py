import csv
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt


def calcLifetimeperDeliveredPaket(a):
    length = len(a["Node LifeTime [ms]"])
    sumLife = sum(a["Node LifeTime [ms]"])
    delPercentage = sum(
        a["Different Received Data Packets"])/(60*(length-1))/length
    return sumLife*delPercentage


def get5Th(a):
    return np.partition(a, 4)[4]


def get13Th(a):
    return np.partition(a, 12)[12]


def get9Th(a):
    return np.partition(a, 8)[8]


def get23Th(a):
    return np.partition(a, 22)[22]


sns.set_theme(style="dark")

sim_data_L_Flood = pd.read_csv(
    "../output/Line_10_Flooding_battery/results_median.csv")
sim_data_L_1D0C = pd.read_csv(
    "../output/Line_10_Mpl_1D0C_battery/results_median.csv")
sim_data_L_2D0C = pd.read_csv(
    "../output/Line_10_Mpl_2D0C_battery/results_median.csv")
sim_data_L_1D1C = pd.read_csv(
    "../output/Line_10_Mpl_1D1C_battery/results_median.csv")
sim_data_L_2D1C = pd.read_csv(
    "../output/Line_10_Mpl_2D1C_battery/results_median.csv")
sim_data_L_2D2C = pd.read_csv(
    "../output/Line_10_Mpl_2D2C_battery/results_median.csv")

sim_data_C_Flood = pd.read_csv(
    "../output/Circle_10_Flooding_battery/results_median.csv")
sim_data_C_1D0C = pd.read_csv(
    "../output/Circle_10_Mpl_1D0C_battery/results_median.csv")
sim_data_C_2D0C = pd.read_csv(
    "../output/Circle_10_Mpl_2D0C_battery/results_median.csv")
sim_data_C_1D1C = pd.read_csv(
    "../output/Circle_10_Mpl_1D1C_battery/results_median.csv")
sim_data_C_2D1C = pd.read_csv(
    "../output/Circle_10_Mpl_2D1C_battery/results_median.csv")
sim_data_C_2D2C = pd.read_csv(
    "../output/Circle_10_Mpl_2D2C_battery/results_median.csv")

sim_data_S_Flood = pd.read_csv(
    "../output/Square_5x5_Flooding_battery/results_median.csv")
sim_data_S_1D0C = pd.read_csv(
    "../output/Square_5x5_Mpl_1D0C_battery/results_median.csv")
sim_data_S_2D0C = pd.read_csv(
    "../output/Square_5x5_Mpl_2D0C_battery/results_median.csv")
sim_data_S_1D1C = pd.read_csv(
    "../output/Square_5x5_Mpl_1D1C_battery/results_median.csv")
sim_data_S_2D1C = pd.read_csv(
    "../output/Square_5x5_Mpl_2D1C_battery/results_median.csv")
sim_data_S_2D2C = pd.read_csv(
    "../output/Square_5x5_Mpl_2D2C_battery/results_median.csv")

listname = "Node LifeTime [ms]"
protNames = ["Flooding", "MPL 1D0C", "MPL 2D0C",
             "MPL 1D1C", "MPL 2D1C", "MPL 2D2C"]
TopologyLine = ["Line", " Line", "Line", "Line", "Line", "Line"]
TopologyCircle = ["Circle", " Circle", "Circle", "Circle", "Circle", "Circle"]
TopologySquare = ["Square", " Square", "Square", "Square", "Square", "Square"]

lineFirst = [min(sim_data_L_Flood[listname]), min(sim_data_L_1D0C[listname]), min(sim_data_L_2D0C[listname]), min(
    sim_data_L_1D1C[listname]), min(sim_data_L_2D1C[listname]), min(sim_data_L_2D2C[listname])]
line50 = [get5Th(sim_data_L_Flood[listname]), get5Th(sim_data_L_1D0C[listname]), get5Th(sim_data_L_2D0C[listname]), get5Th(
    sim_data_L_1D1C[listname]), get5Th(sim_data_L_2D1C[listname]), get5Th(sim_data_L_2D2C[listname])]
line90 = [get9Th(sim_data_L_Flood[listname]), get9Th(sim_data_L_1D0C[listname]), get9Th(sim_data_L_2D0C[listname]), get9Th(
    sim_data_L_1D1C[listname]), get9Th(sim_data_L_2D1C[listname]), get9Th(sim_data_L_2D2C[listname])]
lineLifePerDelivery = [calcLifetimeperDeliveredPaket(sim_data_L_Flood), calcLifetimeperDeliveredPaket(sim_data_L_1D0C), calcLifetimeperDeliveredPaket(
    sim_data_L_2D0C), calcLifetimeperDeliveredPaket(sim_data_L_1D1C), calcLifetimeperDeliveredPaket(sim_data_L_2D1C), calcLifetimeperDeliveredPaket(sim_data_L_2D2C)]

circleFirst = [min(sim_data_C_Flood[listname]), min(sim_data_C_1D0C[listname]), min(sim_data_C_2D0C[listname]), min(
    sim_data_C_1D1C[listname]), min(sim_data_C_2D1C[listname]), min(sim_data_C_2D2C[listname])]
circle50 = [get5Th(sim_data_C_Flood[listname]), get5Th(sim_data_C_1D0C[listname]), get5Th(sim_data_C_2D0C[listname]), get5Th(
    sim_data_C_1D1C[listname]), get5Th(sim_data_C_2D1C[listname]), get5Th(sim_data_C_2D2C[listname])]
circle90 = [get9Th(sim_data_C_Flood[listname]), get9Th(sim_data_C_1D0C[listname]), get9Th(sim_data_C_2D0C[listname]), get9Th(
    sim_data_C_1D1C[listname]), get9Th(sim_data_C_2D1C[listname]), get9Th(sim_data_C_2D2C[listname])]
circleLifePerDelivery = [calcLifetimeperDeliveredPaket(sim_data_C_Flood), calcLifetimeperDeliveredPaket(sim_data_C_1D0C), calcLifetimeperDeliveredPaket(
    sim_data_C_2D0C), calcLifetimeperDeliveredPaket(sim_data_C_1D1C), calcLifetimeperDeliveredPaket(sim_data_C_2D1C), calcLifetimeperDeliveredPaket(sim_data_C_2D2C)]

squareFirst = [min(sim_data_S_Flood[listname]), min(sim_data_S_1D0C[listname]), min(sim_data_S_2D0C[listname]), min(
    sim_data_S_1D1C[listname]), min(sim_data_S_2D1C[listname]), min(sim_data_S_2D2C[listname])]
square50 = [get13Th(sim_data_S_Flood[listname]), get13Th(sim_data_S_1D0C[listname]), get13Th(sim_data_S_2D0C[listname]), get13Th(
    sim_data_S_1D1C[listname]), get13Th(sim_data_S_2D1C[listname]), get13Th(sim_data_S_2D2C[listname])]
square90 = [get23Th(sim_data_S_Flood[listname]), get23Th(sim_data_S_1D0C[listname]), get23Th(sim_data_S_2D0C[listname]), get23Th(
    sim_data_S_1D1C[listname]), get23Th(sim_data_S_2D1C[listname]), get23Th(sim_data_S_2D2C[listname])]
squareLifePerDelivery = [calcLifetimeperDeliveredPaket(sim_data_S_Flood), calcLifetimeperDeliveredPaket(sim_data_S_1D0C), calcLifetimeperDeliveredPaket(
    sim_data_S_2D0C), calcLifetimeperDeliveredPaket(sim_data_S_1D1C), calcLifetimeperDeliveredPaket(sim_data_S_2D1C), calcLifetimeperDeliveredPaket(sim_data_S_2D2C)]


print("Line:")
print(['%.2f' % elem for elem in np.divide(lineFirst, 1000)])
print(['%.2f' % elem for elem in np.divide(line50, 1000)])
print(['%.2f' % elem for elem in np.divide(line90, 1000)])
print(['%.2f' % elem for elem in np.divide(lineLifePerDelivery, 1000)])

print("Circle:")
print(['%.2f' % elem for elem in np.divide(circleFirst, 1000)])
print(['%.2f' % elem for elem in np.divide(circle50, 1000)])
print(['%.2f' % elem for elem in np.divide(circle90, 1000)])
print(['%.2f' % elem for elem in np.divide(circleLifePerDelivery, 1000)])


print("Square:")
print(['%.2f' % elem for elem in np.divide(squareFirst, 1000)])
print(['%.2f' % elem for elem in np.divide(square50, 1000)])
print(['%.2f' % elem for elem in np.divide(square90, 1000)])
print(['%.2f' % elem for elem in np.divide(squareLifePerDelivery, 1000)])

sns.set(style="ticks")
sns.set_style("darkgrid", {'axes.grid': True})
h = sns.barplot(x=protNames, y=np.divide(lineLifePerDelivery, 1000))
h.set(ylim=(0, 3500))
h.set(ylabel='Normed Lifetime per Delivered Data Message [s]')
h.figure.savefig("NormedLifeTimeLine.pdf",  bbox_inches='tight', dpi=300)

h = sns.barplot(x=protNames, y=np.divide(circleLifePerDelivery, 1000))
h.set(ylim=(0, 3500))
h.set(ylabel='Normed Lifetime per Delivered Data Message [s]')
h.figure.savefig("NormedLifeTimeCircle.pdf",  bbox_inches='tight', dpi=300)

h = sns.barplot(x=protNames, y=np.divide(squareLifePerDelivery, 1000))
h.set(ylim=(0, 3500))
h.set(ylabel='Normed Lifetime per Delivered Data Message [s]')
h.figure.savefig("NormedLifeTimeSquare.pdf",  bbox_inches='tight', dpi=300)
