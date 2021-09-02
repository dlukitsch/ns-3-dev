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
    outname = ''
    diagtype = ''
    diagvalue = ''
    title = ''
    xdes = ''
    ydes = ''
    fileList = []
    legendList = []

    ts_nodeIdsInt = []
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

    ts_recNewPackets = []
    ts_minDelay = []
    ts_maxDelay = []
    ts_medianDelay = []
    ts_varianceDelay = []

    ts_doubleRecPackets = []
    ts_doubleRecBytes = []
    ts_doubleRecDataPackets = []
    ts_doubleRecDataBytes = []
    ts_doubleRecControlPackets = []
    ts_doubleRecControlBytes = []

    ts_txOff = []
    ts_txOn = []
    ts_txBusy = []
    ts_rxOff = []
    ts_rxOn = []
    ts_rxBusy = []
    ts_idle = []
    ts_txRxOff = []
    ts_busy = []

    try:
        opts, args = getopt.getopt(argv,"n:t:v:a:b:c:d:e:f:g:h:i:j:k:l:x:y:o:")
    except getopt.GetoptError:
        print ('Wrong Usage!')
        sys.exit(2)
    for opt, arg in opts:
        if opt == "-n":
            outname = arg
        elif opt == "-t":
            diagtype = arg
        elif opt == "-v":
            diagvalue = arg
        elif opt == "-a" or opt == "-b" or opt == "-c" or opt == "-d" or opt == "-e" or opt == "-f":
            fileList.append(arg+"_mean.csv")
            fileList.append(arg+"_median.csv")
            fileList.append(arg+"_variance.csv")
        elif opt == "-g" or opt == "-h" or opt == "-i" or opt == "-j" or opt == "-k" or opt == "-l":
            legendList.append(arg)
        elif opt == "-o":
            title = arg
        elif opt == "-x":
            xdes = arg
        elif opt == "-y":
            ydes = arg

#////////////////////////////////////////////////////////////////////////////////////////////
    # parse all the files into lists of lists
    for i in fileList:
        # data imports
        dataset = pd.read_csv(i,skipinitialspace=True) #loading the data
        dataset.apply (pd.to_numeric, errors='coerce')
        dataset = dataset.dropna() #replace empty days with 0 packets

        ts_nodeIdsInt.append(dataset.iloc[:,0].tolist())
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

    ts_nodeIds = ["Node " + str(int) for int in ts_nodeIdsInt[0]]
    X = np.arange(len(ts_nodeIdsInt[0]))
    rects = []    
#////////////////////////////////////////////////////////////////////////////////////////////
    #parsing done now we can create the diagram
    fig, axs = plt.subplots()

    width = 1.0/(len(fileList)/3+1)

    axs.set_xlabel(xdes)
    axs.set_ylabel(ydes)
    axs.set_title(title)

    #cases to set the x-ticks in the middle of the according data
    if len(fileList)//3 == 2:
        axs.set_xticks(X+width/2)
    else:
        axs.set_xticks(X+width)

    axs.set_xticklabels(ts_nodeIds)

    if diagtype == "Bar":
        for i in range(0,len(fileList)//3):
            rects.append(axs.bar(X+i*width, (locals()[diagvalue])[i*3+1], width))

        for i in rects:
            axs.bar_label(i)
    else:
        print("Error: Unsupported Diagram Type selected!")
        sys.exit(1)

    axs.legend(labels = legendList)
    fig.savefig(outname)

if __name__ == "__main__":
   main(sys.argv[1:])