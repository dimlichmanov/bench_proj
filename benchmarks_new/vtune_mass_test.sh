#!/bin/bash

export PYTHONPATH=/home/vadim/_scratch/software/plotly/lib/python3.6/site-packages/
module add gcc/9.1

ITER=3
CONST_DIR_PREFIX="RESULTS" # this should NOT be changed
DIRNAME="28thr_likwid_vtune" # this should be changed
OUTDIR=$CONST_DIR_PREFIX"/"$DIRNAME
BENCH_LIST="triada stencil_1D matrix_mult random_access"

rm time.txt

if [ ! -d "$OUTDIR" ]; then
  echo "Creating directory $OUTDIR"
  mkdir $OUTDIR
fi

for (( i=1; i<=$ITER; i++ ))
do
  echo "Started iter "$i
  for bench in $BENCH_LIST
  do
    time python3 run_tests_28threads.py -m -b $bench
    mv result $OUTDIR/result_${bench}_${i}
    mv output $OUTDIR/$i
    mkdir output
    mv time.txt $OUTDIR/$i
  done
done
