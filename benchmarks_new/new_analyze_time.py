#!/usr/bin/python


import sys, os, getopt
from collections import defaultdict
from pprint import pprint



def convert_time(time_str):
    tmp1 = time_str.split('s')[0]
    tmp2_arr = tmp1.split('m')
    return int(tmp2_arr[0])*60 + float(tmp2_arr[1])

def read_file(fin):
    data = {}
    curr_test = ''
    for line in fin:
        line = line.strip()
        if "Segmentation fault" in line:
            continue
        # means we have a test name
        if line and ('\t' not in line):
            curr_test = line
        # means we have the time for a test name
        if ('\t' in line) and ('real' in line):
           time = convert_time(line.split('\t')[1])
           data[curr_test] = time
    return data


def main(argv):

    INDIR = ""
    INPUTFILE = "time.txt" # should not be specified in parameters
    OUTFILE = ""

    try:
       opts, args = getopt.getopt(argv,"i:o:",["idir=","ofile="])
    except getopt.GetoptError:
      print('python new_analyze_time.py -i <inputdir> -o <outputfile>')
      sys.exit(2)
    for opt, arg in opts:
      if opt in ("-i", "--ifile"):
          INDIR = arg
      elif opt in ("-o", "--ofile"):
          OUTFILE = arg

    if not INDIR:
       print("Need to specify input directory with -i option!")
       sys.exit(2)
    if not OUTFILE:
       OUTFILE = INDIR + "_time_results.csv"

    all_data = defaultdict(list)
    dirs = [f for f in os.listdir(INDIR)]
    for cur_dir in dirs:
         filename = INDIR+'/'+cur_dir+'/'+INPUTFILE
         print(filename)
         with open(filename,'r') as fin:
             data = read_file(fin)
             for name in data:
                 all_data[name].append(data[name])

    pprint(all_data)


    NUM_ITER = 10
    with open(OUTFILE,'w') as fout:
        tmp = ""
        for i in range(NUM_ITER):
            tmp += ",iter"+str(i+1)
        fout.write('name'+tmp+'\n')
        for name in all_data:
            fout.write(name+','+','.join(map(str, all_data[name]))+'\n')
            


if __name__ == "__main__":
     main(sys.argv[1:])
