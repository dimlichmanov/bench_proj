#!/bin/bash

##############################################
### SLURM PARAMS
PARTITION=test
NUM_NODES=1
TASKS_PER_NODE=1
#NODELIST="n[54116,54127-54131,54210,54213,54231,54317-54320,54422,54521-54522]" # 16 nodes
#NODELIST="n[50003-50008,50011-50012]" # 8 nodes
NODELIST="n[50003]" # 4 nodes
#NODELIST="n[50012-50015]" # 2 nodes 
#NODELIST="n[48621-48625,50003-50004,50007-50015]" # 16 nodes 
#NODELIST="n50011" # 1 node 
#EXCLUDE_NODES="-x n50001"
#MYSCRIPT=./myrun.sh
MYSCRIPT=run
OUTFILE="slurm-%j.out"

### ITER PARAMS
NUM_ITERS=5
BENCH_LIST="npb-bt"
#BENCH_LIST="matrix_mult npb-bt stencil_1D"
#BENCH_LIST="npb-lu hpcg npb-cg"

### NPB PARAMS

### SERVICE
#DATE=`date +'%d_%m_%Y'`
#OUTDIR="RESULTS/${DATE}-${NUM_NODES}-${TASKS_PER_NODE}"
#############################################

# check that directory doesn't exist
#if [[ -d "$OUTDIR" ]]
#then
#    echo "$OUTDIR already exists! Deleting it"
#    rm -rf $OUTDIR
#fi
#mkdir $OUTDIR

### Needed for VTune
#source /opt/intel/oneapi/vtune/2021.1.2/amplxe-vars.sh

# start main loop
for (( iter=1; iter<=$NUM_ITERS; iter++ ))
do
    printf "************\nStarting iter ${iter}\n************\n"
#    mkdir $iter
#    cd $iter
    for bench in $BENCH_LIST
    do
        echo "--- Starting bench ${bench} ---"
        EXEC="./mass_test_wparam.sh"
        OUTFILE="slurm-${bench}-%j.out"
        ### default part
        echo "sbatch -p $PARTITION -N $NUM_NODES --ntasks-per-node $TASKS_PER_NODE -w $NODELIST -o $OUTFILE $MYSCRIPT $EXEC $bench $iter"
        until sbatch -p $PARTITION -N $NUM_NODES --ntasks-per-node $TASKS_PER_NODE -w $NODELIST -o $OUTFILE $MYSCRIPT $EXEC $bench $iter 2> /dev/null
        ### VTune part 
#        echo "sbatch -C nomonitoring -p $PARTITION -N $NUM_NODES --ntasks-per-node $TASKS_PER_NODE -w $NODELIST -o $OUTFILE run vtune -r vtune_${bench} -collect uarch-exploration -- $EXEC $bench $iter"
#        until sbatch -C nomonitoring -p $PARTITION -N $NUM_NODES --ntasks-per-node $TASKS_PER_NODE -w $NODELIST -o $OUTFILE run vtune -r vtune_${bench} -collect uarch-exploration -- $EXEC $bench $iter 2> /dev/null
        do
            echo -n "."
            sleep 10
        done
        echo ""
    done
#    cd ..
done

printf "Ended submitting jobs!\n"

# wait for jobs to complete (otherwise slurm output files won't appear)
printf "start waiting for submitted jobs to complete"
until [ `squeue -p compute_prio,test,compute -u vadim | wc -l` -eq 1 ] ;
do
   echo -n "."
   sleep 10
done
echo ""

# move all results to needed directory
#for (( iter=1; iter<=$NUM_ITERS; iter++ ))
#do
#    mv $iter ${OUTDIR}/
#done
