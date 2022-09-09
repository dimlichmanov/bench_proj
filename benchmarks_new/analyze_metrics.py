
import os
from collections import defaultdict
import numpy as np, scipy.stats as st

from pprint import pprint

METRICS_FILE = 'intel_xeon_profile_metrics.csv'
TIME_FILE = 'intel_xeon_timings.csv'

# NB: this variable need to be changed for each new experiment with mass_test.sh (if output dir name changed of course)
# seems that nothing else should be changed
DATADIR = "8counters_perf"

OUT_METRICS_FILE = 'metrics_results_'+DATADIR+'.csv'
OUT_TIME_FILE = 'time_results_'+DATADIR+'.csv'

DERIVATIVES_LIST = ['mean', 'stdev', 'coeff_var', 'ci', 'median']

def my_cv(x):
    return np.std(x, ddof=1) / np.mean(x) * 100

def my_ci(x, conf = 0.95):
    if not isinstance(x, list): 
        print('Error calculating CI for: '+str(x))
        exit(0)
    # check if all array elems are identical, which leads to an error while calculating CI
    if len(set(x)) <= 1: 
        return (x[0], x[0])
    return st.t.interval(conf, len(x)-1, loc=np.mean(x), scale=st.sem(x))

DERIVATIVES_FUNCTIONS = {'mean': np.mean, 'stdev': np.std, 'coeff_var': my_cv, 'ci': my_ci, 'median': np.median}


def calculate_derivatives(data_for_derivatives):
    derivatives = {}
    for name in data_for_derivatives:
        derivatives[name] = {}
        for deriv_type in DERIVATIVES_FUNCTIONS:
            derivatives[name][deriv_type] = defaultdict(float)
            for metric in data_for_derivatives[name]:
                derivatives[name][deriv_type][metric] = DERIVATIVES_FUNCTIONS[deriv_type](data_for_derivatives[name][metric])

    return derivatives



def process_file(ifile, ofile):
    data = defaultdict(list)
    data_for_derivatives = {}

    # read data from csvs provided by benchmark suite
    dirs = [f for f in os.listdir(DATADIR)]
    toprow = ""
    for curr_dir in dirs:
        f = DATADIR+'/'+curr_dir+'/'+ifile
        with open(f, 'r') as fin:
            contents = fin.readlines()
            toprow = contents[0].split(',', 1)[1]
            data_for_derivatives
            for line in contents[1:]:
                line = line.strip()
                if not line: continue
                try:
                    # save to data
                    name, metrics_data = line.split(',', 1)
                    data[name].append(metrics_data)
                    # save to data_for_derivatives
                    count = 0
                    values = metrics_data.split(',')
                    if name not in data_for_derivatives:
                        data_for_derivatives[name] = defaultdict(list)
                    for metric in toprow.split(',')[:-1]:
                        data_for_derivatives[name][metric].append(float(values[count]))
                        count += 1
                except:
                    print('Invalid data format: '.format(line))


    # calculate all needed derivatives: mean, standard deviation, coefficient of variation, confidence intervals, median
    #pprint(data_for_derivatives)
    derivatives = calculate_derivatives(data_for_derivatives)

    # write collected data to integral csv
    with open(ofile, 'w') as fout:
        for name in data:
            iter_num = 1
            fout.write(name+'\n')
            fout.write("num_iter,"+toprow)
            # save raw data to file
            for elem in data[name]:
                fout.write("iter"+str(iter_num)+","+elem+'\n')
                iter_num += 1
            # save derivatives to file
            for deriv_type in derivatives[name]:
                fout.write(deriv_type+",")
                for metric in toprow.split(',')[:-1]:
                    fout.write(str(derivatives[name][deriv_type][metric])+",")
                fout.write("\n")
            fout.write('\n')
            

def main():
    # process metrics file
    process_file(METRICS_FILE, OUT_METRICS_FILE)
    # process time file
    process_file(TIME_FILE, OUT_TIME_FILE)


if __name__ == '__main__':
    main()
