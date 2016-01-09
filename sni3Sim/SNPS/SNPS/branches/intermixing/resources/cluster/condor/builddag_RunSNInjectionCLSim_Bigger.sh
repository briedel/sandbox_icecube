#!/bin/bash
for i in $(seq 10000)
do
    JOBID=EFG.$i
    echo JOB $JOBID RunSNInjectionCLSim.submit 
    #pad out to 3 digits long:
    echo VARS $JOBID Filenum=\"`printf %d $i`\"
done