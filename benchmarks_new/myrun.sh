#!/bin/sh
srun --ntasks-per-node=1 -n ${SLURM_JOB_NUM_NODES} /home/vadim/_scratch/rnf/npb_tests/prod-likwid_5.2.1_daemon-zhum.sh
srun "$@"

