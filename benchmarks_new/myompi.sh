#!/bin/sh
#
#SBATCH -C nomonitoring

#srun --resv-ports "$@"
#exit $?

[ x"$TMPDIR" == x"" ] && TMPDIR=/tmp
HOSTFILE=${TMPDIR}/hostfile.${SLURM_JOB_ID}
srun hostname -s|sort|uniq -c|awk '{print $2" slots="$1}' > $HOSTFILE || { rm -f $HOSTFILE; exit 255; }



##### PAPI #####
## version for default dimmon
#srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/default_dimmon.sh

### OLD ###
## version for dimmon with new papi
#srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/new_dimmon.sh
## version for dimmon with manual multiplex
#srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/manual_papi_dimmon.sh
### END OF OLD ###




##### LIKWID #####
### OLD ###
## version for likwid with daemon
#srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/likwid_5.2.1_daemon.sh
## version for likwid with direct access
#srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/likwid_4.3.4_direct.sh
### END OF OLD ###

## version for likwid with direct access
#srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/likwid_5.2.1_daemon-zhum.sh
## production version for likwid with direct access
srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/prod-likwid_5.2.1_daemon-zhum.sh


mpirun --hostfile $HOSTFILE "$@"
rc=$?
rm -f $HOSTFILE

exit $rc

