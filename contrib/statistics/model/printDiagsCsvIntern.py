import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdate
import time
import sys, getopt
from scipy import stats
from statistics import mean 
from statistics import median

def main(argv):
    inputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print ('printDiagsCsvIntern.py -i <inputfile>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ('printDiagsCsvIntern.py -i <inputfile>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg

    fileList = [ inputfile+"_mean.csv", inputfile+"_median.csv" , inputfile+"_variance.csv" ]

    ts_nodeIdsIntList = []
    ts_succSentPackets = []
    ts_succSentBytes = []
    ts_succSentDataPackets = []
    ts_succSentDataBytes = []
    ts_succSentControlPackets = []
    ts_succSentControlBytes = []
    
    ts_dropSentPackets = []
    ts_dropSentBytes = []
    ts_dropSentDataPackets = []
    ts_dropSentDataBytes = []
    ts_dropSentControlPackets = []
    ts_dropSentControlBytes = []

    ts_succRecPackets = []
    ts_succRecBytes = []
    ts_succRecDataPackets = []
    ts_succRecDataBytes = []
    ts_succRecControlPackets = []
    ts_succRecControlBytes = []

    ts_dropRecPackets = []
    ts_dropRecBytes = []
    ts_dropRecDataPackets = []
    ts_dropRecDataBytes = []
    ts_dropRecControlPackets = []
    ts_dropRecControlBytes = []

    ts_doubleSentPackets = []
    ts_doubleSentBytes = []
    ts_doubleSentDataPackets = []
    ts_doubleSentDataBytes = []
    ts_doubleSentControlPackets = []
    ts_doubleSentControlBytes = []

    ts_doubleRecPackets = []
    ts_doubleRecBytes = []
    ts_doubleRecDataPackets = []
    ts_doubleRecDataBytes = []
    ts_doubleRecControlPackets = []
    ts_doubleRecControlBytes = []

    ts_recNewPackets = []
    ts_minDelay = []
    ts_maxDelay = []
    ts_medianDelay = []
    ts_varianceDelay = []

    ts_txOff = []
    ts_txOn = []
    ts_txBusy = []
    ts_rxOff = []
    ts_rxOn = []
    ts_rxBusy = []
    ts_idle = []
    ts_txRxOff = []
    ts_busy = []

#////////////////////////////////////////////////////////////////////////////////////////////
    # parse all the files into lists of lists
    for i in fileList:
        # data imports
        dataset = pd.read_csv(i,skipinitialspace=True) #loading the data
        dataset.apply (pd.to_numeric, errors='coerce')
        dataset = dataset.dropna() #replace empty days with 0 packets

        # [0] -> mean
        # [1] -> median
        # [2] -> variance

        ts_nodeIdsIntList.append(dataset.iloc[:,0].tolist())
        ts_succSentPackets.append(dataset.iloc[:,1].tolist())
        ts_succSentBytes.append(dataset.iloc[:,2].tolist())
        ts_succSentDataPackets.append(dataset.iloc[:,3].tolist())
        ts_succSentDataBytes.append(dataset.iloc[:,4].tolist())
        ts_succSentControlPackets.append(dataset.iloc[:,5].tolist())
        ts_succSentControlBytes.append(dataset.iloc[:,6].tolist())
        
        ts_dropSentPackets.append(dataset.iloc[:,7].tolist())
        ts_dropSentBytes.append(dataset.iloc[:,8].tolist())
        ts_dropSentDataPackets.append(dataset.iloc[:,9].tolist())
        ts_dropSentDataBytes.append(dataset.iloc[:,10].tolist())
        ts_dropSentControlPackets.append(dataset.iloc[:,11].tolist())
        ts_dropSentControlBytes.append(dataset.iloc[:,12].tolist())

        ts_succRecPackets.append(dataset.iloc[:,13].tolist())
        ts_succRecBytes.append(dataset.iloc[:,14].tolist())
        ts_succRecDataPackets.append(dataset.iloc[:,15].tolist())
        ts_succRecDataBytes.append(dataset.iloc[:,16].tolist())
        ts_succRecControlPackets.append(dataset.iloc[:,17].tolist())
        ts_succRecControlBytes.append(dataset.iloc[:,18].tolist())

        ts_dropRecPackets.append(dataset.iloc[:,19].tolist())
        ts_dropRecBytes.append(dataset.iloc[:,20].tolist())
        ts_dropRecDataPackets.append(dataset.iloc[:,21].tolist())
        ts_dropRecDataBytes.append(dataset.iloc[:,22].tolist())
        ts_dropRecControlPackets.append(dataset.iloc[:,23].tolist())
        ts_dropRecControlBytes.append(dataset.iloc[:,24].tolist())

        ts_doubleSentPackets.append(dataset.iloc[:,25].tolist())
        ts_doubleSentBytes.append(dataset.iloc[:,26].tolist())
        ts_doubleSentDataPackets.append(dataset.iloc[:,27].tolist())
        ts_doubleSentDataBytes.append(dataset.iloc[:,28].tolist())
        ts_doubleSentControlPackets.append(dataset.iloc[:,29].tolist())
        ts_doubleSentControlBytes.append(dataset.iloc[:,30].tolist())

        ts_doubleRecPackets.append(dataset.iloc[:,31].tolist())
        ts_doubleRecBytes.append(dataset.iloc[:,32].tolist())
        ts_doubleRecDataPackets.append(dataset.iloc[:,33].tolist())
        ts_doubleRecDataBytes.append(dataset.iloc[:,34].tolist())
        ts_doubleRecControlPackets.append(dataset.iloc[:,35].tolist())
        ts_doubleRecControlBytes.append(dataset.iloc[:,36].tolist())

        ts_recNewPackets.append(dataset.iloc[:,37].tolist())
        ts_minDelay.append(dataset.iloc[:,38].tolist())
        ts_maxDelay.append(dataset.iloc[:,39].tolist())
        ts_medianDelay.append(dataset.iloc[:,40].tolist())
        ts_varianceDelay.append(dataset.iloc[:,41].tolist())

        ts_txOff.append(dataset.iloc[:,42].tolist())
        ts_txOn.append(dataset.iloc[:,43].tolist())
        ts_txBusy.append(dataset.iloc[:,44].tolist())
        ts_rxOff.append(dataset.iloc[:,45].tolist())
        ts_rxOn.append(dataset.iloc[:,46].tolist())
        ts_rxBusy.append(dataset.iloc[:,47].tolist())
        ts_idle.append(dataset.iloc[:,48].tolist())
        ts_txRxOff.append(dataset.iloc[:,49].tolist())
        ts_busy.append(dataset.iloc[:,50].tolist())

    ts_nodeIdsInt = ts_nodeIdsIntList[1]
    ts_nodeIds = ["Node " + str(int) for int in ts_nodeIdsInt]
    X = np.arange(len(ts_nodeIdsInt))
    rects = []
    outputfile = inputfile[0:len(inputfile)]

    # 3D-plot preparation
    rowNames = ["", "Control", "", "Data", "", "Overall" ]
    nodeNames = []
    colorsAvailable = ['red', 'blue', 'green']
    colors = []

    for i in ts_nodeIds:
        nodeNames.extend(["", i])
        
    for i in colorsAvailable:
        colors.extend([i]*len(ts_nodeIdsInt))

    lx = len(ts_nodeIdsInt)
    ly = len(rowNames)//2
    xpos = np.arange(0, 2*lx, 2)
    ypos = np.arange(0, 2*ly, 2)
    xpos, ypos = np.meshgrid(xpos, ypos)

    xpos = xpos.flatten()
    ypos = ypos.flatten()

    zpos = np.zeros(lx*ly)
    dx = np.ones_like(zpos)
    dy = dx.copy()

#////////////////////////////////////////////////////////////////////////////////////////////
    fig = plt.figure()
    ax = fig.add_subplot(projection='3d')
    dz = [item for sublist in [ ts_succSentControlPackets[1], ts_succSentDataPackets[1], ts_succSentPackets[1] ] for item in sublist]
    ax.bar3d(xpos, ypos, zpos, dx, dy, dz, color=colors)
    ax.xaxis.set_ticks(range(len(nodeNames)))
    ax.yaxis.set_ticks(range(len(rowNames)))
    ax.w_xaxis.set_ticklabels(nodeNames)
    ax.w_yaxis.set_ticklabels(rowNames)
    ax.set_xlabel('Node IDs')
    ax.set_zlabel('# of Sent Packets')

    fig.savefig(outputfile + "_3DSentPackets.png") # .svg

#////////////////////////////////////////////////////////////////////////////////////////////

    fig, fig_axes = plt.subplots(2, 2, constrained_layout=True)
    plotSizeGcd = 5
    fig.set_size_inches(plotSizeGcd*4, plotSizeGcd*3)
    width = 0.25
    
    fig_axes[0, 0].set_xlabel("Network Nodes")
    fig_axes[0, 0].set_ylabel("# of Packets")
    fig_axes[0, 0].set_title("Successfully Sent Packets")
    rects.append(fig_axes[0, 0].bar(X+0*width, ts_succSentPackets[1], width, yerr=np.sqrt(ts_succSentPackets[2]), align='center', ecolor='black', capsize=10))
    rects.append(fig_axes[0, 0].bar(X+1*width, ts_succSentDataPackets[1], width, yerr=np.sqrt(ts_succSentDataPackets[2]), align='center', ecolor='black', capsize=10))
    rects.append(fig_axes[0, 0].bar(X+2*width, ts_succSentControlPackets[1], width, yerr=np.sqrt(ts_succSentControlPackets[2]), align='center', ecolor='black', capsize=10))
    fig_axes[0, 0].set_xticks(X+width)
    fig_axes[0, 0].set_xticklabels(ts_nodeIds)
    fig_axes[0, 0].legend(labels = ["Packets", "Data-Packets", "Control-Packets"])
    for i in rects:
        fig_axes[0,0].bar_label(i)
    rects.clear()

    fig_axes[0, 1].set_xlabel("Network Nodes")
    fig_axes[0, 1].set_ylabel("# of Bytes")
    fig_axes[0, 1].set_title("Successfully Sent Bytes")
    rects.append(fig_axes[0, 1].bar(X+0*width, ts_succSentBytes[1], width))
    rects.append(fig_axes[0, 1].bar(X+1*width, ts_succSentDataBytes[1], width))
    rects.append(fig_axes[0, 1].bar(X+2*width, ts_succSentControlBytes[1], width))
    fig_axes[0, 1].set_xticks(X+width)
    fig_axes[0, 1].set_xticklabels(ts_nodeIds)
    fig_axes[0, 1].legend(labels = ["Bytes", "Data-Bytes", "Control-Bytes"])
    for i in rects:
        fig_axes[0,1].bar_label(i)
    rects.clear()

    fig_axes[1, 0].set_xlabel("Network Nodes")
    fig_axes[1, 0].set_ylabel("# of Packets")
    fig_axes[1, 0].set_title("Successfully Received Packets")
    rects.append(fig_axes[1, 0].bar(X+0*width, ts_succRecPackets[1], width))
    rects.append(fig_axes[1, 0].bar(X+1*width, ts_succRecDataPackets[1], width))
    rects.append(fig_axes[1, 0].bar(X+2*width, ts_succRecControlPackets[1], width))
    fig_axes[1, 0].set_xticks(X+width)
    fig_axes[1, 0].set_xticklabels(ts_nodeIds)
    fig_axes[1, 0].legend(labels = ["Packets", "Data-Packets", "Control-Packets"])
    for i in rects:
        fig_axes[1,0].bar_label(i)
    rects.clear()

    fig_axes[1, 1].set_xlabel("Network Nodes")
    fig_axes[1, 1].set_ylabel("# of Bytes")
    fig_axes[1, 1].set_title("Successfully Received Bytes")
    rects.append(fig_axes[1, 1].bar(X+0*width, ts_succRecBytes[1], width))
    rects.append(fig_axes[1, 1].bar(X+1*width, ts_succRecDataBytes[1], width))
    rects.append(fig_axes[1, 1].bar(X+2*width, ts_succRecControlBytes[1], width))
    fig_axes[1, 1].set_xticks(X+width)
    fig_axes[1, 1].set_xticklabels(ts_nodeIds)
    fig_axes[1, 1].legend(labels = ["Bytes", "Data-Bytes", "Control-Bytes"])
    for i in rects:
        fig_axes[1,1].bar_label(i)
    rects.clear()

    fig.savefig(outputfile + "_SuccessfullyStats.png") # .svg
#////////////////////////////////////////////////////////////////////////////////////////////

    fig, fig_axes = plt.subplots(2, 2, constrained_layout=True)
    plotSizeGcd = 5
    fig.set_size_inches(plotSizeGcd*4, plotSizeGcd*3)
    width = 0.25

    fig_axes[0, 0].set_xlabel("Network Nodes")
    fig_axes[0, 0].set_ylabel("# of Packets")
    fig_axes[0, 0].set_title("Duplicated Sent Packets")
    rects.append(fig_axes[0, 0].bar(X+0*width, ts_doubleSentPackets[1], width))
    rects.append(fig_axes[0, 0].bar(X+1*width, ts_doubleSentDataPackets[1], width))
    rects.append(fig_axes[0, 0].bar(X+2*width, ts_doubleSentControlPackets[1], width))
    fig_axes[0, 0].set_xticks(X+width)
    fig_axes[0, 0].set_xticklabels(ts_nodeIds)
    fig_axes[0, 0].legend(labels = ["Packets", "Data-Packets", "Control-Packets"])
    for i in rects:
        fig_axes[0,0].bar_label(i)
    rects.clear()

    fig_axes[0, 1].set_xlabel("Network Nodes")
    fig_axes[0, 1].set_ylabel("# of Bytes")
    fig_axes[0, 1].set_title("Duplicated Sent Bytes")
    rects.append(fig_axes[0, 1].bar(X+0*width, ts_doubleSentBytes[1], width))
    rects.append(fig_axes[0, 1].bar(X+1*width, ts_doubleSentDataBytes[1], width))
    rects.append(fig_axes[0, 1].bar(X+2*width, ts_doubleSentControlBytes[1], width))
    fig_axes[0, 1].set_xticks(X+width)
    fig_axes[0, 1].set_xticklabels(ts_nodeIds)
    fig_axes[0, 1].legend(labels = ["Bytes", "Data-Bytes", "Control-Bytes"])
    for i in rects:
        fig_axes[0,1].bar_label(i)
    rects.clear()

    fig_axes[1, 0].set_xlabel("Network Nodes")
    fig_axes[1, 0].set_ylabel("# of Packets")
    fig_axes[1, 0].set_title("Duplicated Received Packets")
    rects.append(fig_axes[1, 0].bar(X+0*width, ts_doubleRecPackets[1], width))
    rects.append(fig_axes[1, 0].bar(X+1*width, ts_doubleRecDataPackets[1], width))
    rects.append(fig_axes[1, 0].bar(X+2*width, ts_doubleRecControlPackets[1], width))
    fig_axes[1, 0].set_xticks(X+width)
    fig_axes[1, 0].set_xticklabels(ts_nodeIds)
    fig_axes[1, 0].legend(labels = ["Packets", "Data-Packets", "Control-Packets"])
    for i in rects:
        fig_axes[1,0].bar_label(i)
    rects.clear()

    fig_axes[1, 1].set_xlabel("Network Nodes")
    fig_axes[1, 1].set_ylabel("# of Bytes")
    fig_axes[1, 1].set_title("Duplicated Received Bytes")
    rects.append(fig_axes[1, 1].bar(X+0*width, ts_doubleRecBytes[1], width))
    rects.append(fig_axes[1, 1].bar(X+1*width, ts_doubleRecDataBytes[1], width))
    rects.append(fig_axes[1, 1].bar(X+2*width, ts_doubleRecControlBytes[1], width))
    fig_axes[1, 1].set_xticks(X+width)
    fig_axes[1, 1].set_xticklabels(ts_nodeIds)
    fig_axes[1, 1].legend(labels = ["Bytes", "Data-Bytes", "Control-Bytes"])
    for i in rects:
        fig_axes[1,1].bar_label(i)
    rects.clear()

    fig.savefig(outputfile + "_DoubleSentReceivedStats.png") # .svg
#////////////////////////////////////////////////////////////////////////////////////////////

if __name__ == "__main__":
   main(sys.argv[1:])