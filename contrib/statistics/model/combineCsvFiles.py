import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdate
import time
import sys, getopt
import os
import statistics

def main(argv):
    folderpath = ''
    mode = ''
    try:
        opts, args = getopt.getopt(argv,"hf:")
    except getopt.GetoptError:
        print ('combineCsvFiles.py -f <folderpath>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print ('combineCsvFiles.py -f <folderpath>')
            sys.exit()
        elif opt in ("-f", "--fpath"):
            folderpath = arg

    #combine single run files to a combined dataset --> so they can be used by the other python-scripts
    filepaths  = [os.path.join(folderpath, name) for name in os.listdir(folderpath)]

    result_mean = {}
    result_median = {}
    result_variance = {}
    datasets = []

    for filepath in filepaths:
        dataset = pd.read_csv(filepath,skipinitialspace=True) #loading the data
        dataset.apply (pd.to_numeric, errors='coerce')
        dataset = dataset.dropna() #replace empty days with 0 packets
        datasets.append(dataset)

    numRows = len(datasets[0])
    all_files = pd.concat(datasets)

    for i in range(numRows):
        result_mean[i] = all_files[i::len(dataset)].mean()
        result_median[i] = all_files[i::len(dataset)].median()
        result_variance[i] = np.var(all_files[i::len(dataset)])

    path = os.path.abspath(os.path.join(filepaths[0] ,"../.."))
    filename = os.path.basename(filepaths[0])

    filename.rsplit('_', 1)
    strippedFilename = filename.rpartition('_')

    result_mean = pd.DataFrame(result_mean).transpose()
    result_median = pd.DataFrame(result_median).transpose()
    result_variance = pd.DataFrame(result_variance).transpose()
    
    nodeIds = datasets[0]['Node ID']

    result_mean['Node ID'] = nodeIds
    result_median['Node ID'] = nodeIds
    result_variance['Node ID'] = nodeIds

    result_mean.to_csv(path + "/" + strippedFilename[0] + "_mean.csv", index=False)
    result_median.to_csv(path + "/" + strippedFilename[0] + "_median.csv", index=False)
    result_variance.to_csv(path + "/" + strippedFilename[0] + "_variance.csv", index=False)

#////////////////////////////////////////////////////////////////////////////////////////////

if __name__ == "__main__":
   main(sys.argv[1:])