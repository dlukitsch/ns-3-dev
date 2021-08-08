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
    outputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
    except getopt.GetoptError:
        print ('printDiags.py -i <inputfile> -o <outputfile>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ('printDiags.py -i <inputfile> -o <outputfile>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg
    
#////////////////////////////////////////////////////////////////////////////////////////////
    # data imports
    dataset = pd.read_csv(inputfile,skipinitialspace=True) #loading the data
    dataset.apply (pd.to_numeric, errors='coerce')
    dataset = dataset.dropna() #replace empty days with 0 packets

    ts_nodeIdsInt = dataset.iloc[:,0].tolist()
    ts_succSentPackets = dataset.iloc[:,1].tolist()
    ts_succSentBytes = dataset.iloc[:,2].tolist()
    ts_succSentDataPackets = dataset.iloc[:,3].tolist()
    ts_succSentDataBytes = dataset.iloc[:,4].tolist()
    ts_succSentControlPackets = dataset.iloc[:,5].tolist()
    ts_succSentControlBytes = dataset.iloc[:,6].tolist()
    
    ts_dropSentPackets = dataset.iloc[:,7].tolist()
    ts_dropSentBytes = dataset.iloc[:,8].tolist()
    ts_dropSentDataPackets = dataset.iloc[:,9].tolist()
    ts_dropSentDataBytes = dataset.iloc[:,10].tolist()
    ts_dropSentControlPackets = dataset.iloc[:,11].tolist()
    ts_dropSentControlBytes = dataset.iloc[:,12].tolist()

    ts_succRecPackets = dataset.iloc[:,13].tolist()
    ts_succRecBytes = dataset.iloc[:,14].tolist()
    ts_succRecDataPackets = dataset.iloc[:,15].tolist()
    ts_succRecDataBytes = dataset.iloc[:,16].tolist()
    ts_succRecControlPackets = dataset.iloc[:,17].tolist()
    ts_succRecControlBytes = dataset.iloc[:,18].tolist()

    ts_dropRecPackets = dataset.iloc[:,19].tolist()
    ts_dropRecBytes = dataset.iloc[:,20].tolist()
    ts_dropRecDataPackets = dataset.iloc[:,21].tolist()
    ts_dropRecDataBytes = dataset.iloc[:,22].tolist()
    ts_dropRecControlPackets = dataset.iloc[:,23].tolist()
    ts_dropRecControlBytes = dataset.iloc[:,24].tolist()

    ts_doubleSentPackets = dataset.iloc[:,25].tolist()
    ts_doubleSentBytes = dataset.iloc[:,26].tolist()
    ts_doubleSentDataPackets = dataset.iloc[:,27].tolist()
    ts_doubleSentDataBytes = dataset.iloc[:,28].tolist()
    ts_doubleSentControlPackets = dataset.iloc[:,29].tolist()
    ts_doubleSentControlBytes = dataset.iloc[:,30].tolist()

    ts_doubleRecPackets = dataset.iloc[:,31].tolist()
    ts_doubleRecBytes = dataset.iloc[:,32].tolist()
    ts_doubleRecDataPackets = dataset.iloc[:,33].tolist()
    ts_doubleRecDataBytes = dataset.iloc[:,34].tolist()
    ts_doubleRecControlPackets = dataset.iloc[:,35].tolist()
    ts_doubleRecControlBytes = dataset.iloc[:,36].tolist()

    ts_txOff = dataset.iloc[:,37].tolist()
    ts_txOn = dataset.iloc[:,38].tolist()
    ts_txBusy = dataset.iloc[:,39].tolist()
    ts_rxOff = dataset.iloc[:,40].tolist()
    ts_rxOn = dataset.iloc[:,41].tolist()
    ts_rxBusy = dataset.iloc[:,42].tolist()
    ts_idle = dataset.iloc[:,43].tolist()
    ts_txRxOff = dataset.iloc[:,44].tolist()
    ts_busy = dataset.iloc[:,45].tolist()

    ts_nodeIds = ["Node " + str(int) for int in ts_nodeIdsInt]
    X = np.arange(len(ts_nodeIdsInt))

#////////////////////////////////////////////////////////////////////////////////////////////
    fig, fig_axes = plt.subplots(2, 2)
    fig.set_size_inches(20, 15)
    width = 0.25
    
    fig_axes[0, 0].set_xlabel("Network Nodes")
    fig_axes[0, 0].set_ylabel("# of Packets")
    fig_axes[0, 0].set_title("Successfully Sent Packets")
    fig_axes[0, 0].bar(X+0*width, ts_succSentPackets, width)
    fig_axes[0, 0].bar(X+1*width, ts_succSentDataPackets, width)
    fig_axes[0, 0].bar(X+2*width, ts_succSentControlPackets, width)
    fig_axes[0, 0].set_xticks(X+width)
    fig_axes[0, 0].set_xticklabels(ts_nodeIds)
    fig_axes[0, 0].legend(labels = ["Packets", "Data-Packets", "Control-Packets"])

    fig_axes[0, 1].set_xlabel("Network Nodes")
    fig_axes[0, 1].set_ylabel("# of Bytes")
    fig_axes[0, 1].set_title("Successfully Sent Bytes")
    fig_axes[0, 1].bar(X+0*width, ts_succSentBytes, width)
    fig_axes[0, 1].bar(X+1*width, ts_succSentDataBytes, width)
    fig_axes[0, 1].bar(X+2*width, ts_succSentControlBytes, width)
    fig_axes[0, 1].set_xticks(X+width)
    fig_axes[0, 1].set_xticklabels(ts_nodeIds)
    fig_axes[0, 1].legend(labels = ["Bytes", "Data-Bytes", "Control-Bytes"])

    fig_axes[1, 0].set_xlabel("Network Nodes")
    fig_axes[1, 0].set_ylabel("# of Packets")
    fig_axes[1, 0].set_title("Successfully Received Packets")
    fig_axes[1, 0].bar(X+0*width, ts_succRecPackets, width)
    fig_axes[1, 0].bar(X+1*width, ts_succRecDataPackets, width)
    fig_axes[1, 0].bar(X+2*width, ts_succSentControlPackets, width)
    fig_axes[1, 0].set_xticks(X+width)
    fig_axes[1, 0].set_xticklabels(ts_nodeIds)
    fig_axes[1, 0].legend(labels = ["Packets", "Data-Packets", "Control-Packets"])

    fig_axes[1, 1].set_xlabel("Network Nodes")
    fig_axes[1, 1].set_ylabel("# of Bytes")
    fig_axes[1, 1].set_title("Successfully Received Bytes")
    fig_axes[1, 1].bar(X+0*width, ts_succRecBytes, width)
    fig_axes[1, 1].bar(X+1*width, ts_succRecDataBytes, width)
    fig_axes[1, 1].bar(X+2*width, ts_succRecControlBytes, width)
    fig_axes[1, 1].set_xticks(X+width)
    fig_axes[1, 1].set_xticklabels(ts_nodeIds)
    fig_axes[1, 1].legend(labels = ["Bytes", "Data-Bytes", "Control-Bytes"])

    fig.savefig(outputfile + "_SuccessfullyStats.png") # .svg

if __name__ == "__main__":
   main(sys.argv[1:])