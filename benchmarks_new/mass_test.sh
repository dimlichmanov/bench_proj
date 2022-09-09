#!/bin/bash

export PYTHONPATH=/home/vadim/_scratch/software/plotly/lib/python3.6/site-packages/
module add gcc/9.1

ITER=1
CONST_DIR_PREFIX="RESULTS" # this should NOT be changed
DIRNAME="14thr_test" # this should be changed
DATE=`date +'%d_%m_%Y'`
OUTDIR=$CONST_DIR_PREFIX"/"${DATE}-$DIRNAME

rm time.txt

if [ ! -d "$OUTDIR" ]; then
  echo "Creating directory $OUTDIR"
  mkdir $OUTDIR
fi

for (( i=1; i<=$ITER; i++ ))
do
  echo "Started iter "$i
#  time python3 run_tests.py -m
#  time python3 run_tests_28threads.py -m
  time python3 run_tests_nthreads.py -m
  mv output $OUTDIR/$i
  mkdir output
  mv time.txt $OUTDIR/$i
done
