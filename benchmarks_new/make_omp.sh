#!/bin/bash
#export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/usr/local/lib64:/usr/lib64

while [ $# -gt 0 ]; do
  case "$1" in
    --prog=*)
      PROG_NAME="${1#*=}"
      ;;
    --mode=*)
      MODE="${1#*=}"
      ;;
    --length=*)
      LENGTH="${1#*=}"
      ;;
    --radius=*)
      ELEMS="${1#*=}"
      ;;
    --compiler=*)
      COMPILER="${1#*=}"
      ;;
    --threads=*)
      EXP_THREADS="${1#*=}"
      ;;
    --repeats=*)
      REPEATS="${1#*=}"
      ;;
    --no_run=*)
      NO_RUN="${1#*=}"
      ;;
    --metrics=*)
      METRICS="${1#*=}"
      ;;
    --metrics-run)
      METRICS_RUN="FORCE_METRIC_RUN=1"
      ;;
    --events=*)
      EVENTS="${1#*=}"
      ;;
    --output=*)
      OUTPUT="${1#*=}"
      ;;
    --redundant=*)
      REDUNDANT="${1#*=}"
      ;;
    --vtune)
      VTUNING="vtune_enabled"
      ;;
    --affinity=*)
        AFFINITY="${1#*=}"
      ;;
    *)

      printf "***************************\n"
      printf "* Error: Invalid argument.*\n"
      printf "***************************\n"
      exit 1
  esac
  shift
done

cd ./"$PROG_NAME"

if [ ! -f dont-recompile ]; then 
    rm -r bin
    if [[ $METRICS = "false" ]]; then
        if [[ $REPEATS = "" ]]; then
          make ELEMS=$ELEMS LENGTH=$LENGTH MODE=$MODE COMPILER=$COMPILER METRIC_FLAG=NULL THREADS=$EXP_THREADS $METRICS_RUN
        else
          make ELEMS=$ELEMS LENGTH=$LENGTH MODE=$MODE COMPILER=$COMPILER METRIC_FLAG=NULL THREADS=$EXP_THREADS $METRICS_RUN REPEAT=$REPEATS
        fi
    fi

    if [[ $METRICS = "true" ]]; then
      if [[ $REPEATS = "" ]]; then
        make ELEMS=$ELEMS LENGTH=$LENGTH MODE=$MODE COMPILER=$COMPILER METRIC_FLAG=METRIC_RUN THREADS=$EXP_THREADS $METRICS_RUN
      else
        make ELEMS=$ELEMS LENGTH=$LENGTH MODE=$MODE COMPILER=$COMPILER METRIC_FLAG=METRIC_RUN THREADS=$EXP_THREADS $METRICS_RUN REPEAT=$REPEATS
      fi

    fi
fi

rm $OUTPUT

if [[ $AFFINITY != "None" ]]; then
    export OMP_PLACES=$AFFINITY
fi

TIME_FILE="../time.txt"
if [ $NO_RUN = "false" ]; then
    #export OMP_NUM_THREADS=$EXP_THREADS
    #export OMP_PROC_BIND=true
    #export OMP_PROC_BIND=close
    export LD_LIBRARY_PATH=/opt/intel/oneapi/compiler/2021.1.2/linux/compiler/lib/intel64_lin/:$LD_LIBRARY_PATH
    if [[ $METRICS = "true" ]]; then
#        perf stat -o $OUTPUT -a -e $EVENTS ./bin/omp_$PROG_NAME""_np_STD
        echo "$PROG_NAME" >> $TIME_FILE; { time perf stat -o $OUTPUT -a -e $EVENTS ./bin/omp_$PROG_NAME""_np_STD ; } 2>> $TIME_FILE
    fi
    if [[ $METRICS = "false" ]]; then
# next 4 rows should be commented when VTune is not needed
        if [[ $VTUNING = "vtune_enabled" ]]; then
          source /opt/intel/oneapi/vtune/2021.1.2/amplxe-vars.sh
          vtune -r vtune_${PROG_NAME} -collect uarch-exploration -- ./bin/omp_${PROG_NAME}_np_STD > result
          rm -rf vtune_${PROG_NAME}
          mv result ../result
        else
          echo "$PROG_NAME" >> $TIME_FILE; { time ./bin/omp_$PROG_NAME""_np_STD > $OUTPUT ; } 2>> $TIME_FILE
          #./bin/omp_$PROG_NAME""_np_STD > $OUTPUT
        fi
# this was the default version! Return to it when finish with VTune
#        echo "$PROG_NAME" >> $TIME_FILE; { time ./bin/omp_$PROG_NAME""_np_STD > $OUTPUT ; } 2>> $TIME_FILE
###        ./bin/omp_$PROG_NAME""_np_STD > $OUTPUT
        cp $OUTPUT ./test.txt
    fi

fi

cd ../


