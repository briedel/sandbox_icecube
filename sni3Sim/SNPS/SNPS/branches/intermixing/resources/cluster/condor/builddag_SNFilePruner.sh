#!/bin/bash
i=0
for f in /data/uwa/briedel/SNsample/tempsampledone/RunSNInjectionCLSim_*.i3.bz2
do
    JOBID=EFG.$i
    echo JOB $JOBID RunSNFilePruner.submit
    echo VARS $JOBID INFILE=\"`printf %s $f`\"
    ((i++))
done