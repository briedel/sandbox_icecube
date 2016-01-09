#!/usr/bin/env python
import os
from os.path import expandvars
from optparse import OptionParser

from makesubmitfile import makesubmitfile
from builddag import makedagfile

usage = "usage: %prog [options] inputfile"
parser = OptionParser(usage)
parser.add_option("-f", "--submitfile",default="SNPS.submit",
                  dest="SUBMITFILE", help="Write condor submit file")

parser.add_option("-r", "--runnumber", type="int", default=None,
                  dest="RUNNUMBER", help="The run number for this simulation")

parser.add_option("-n", "--numevents", type="int", default=100,
                  dest="NUMEVENTS", help="The number of events per run")

parser.add_option("-p","--particles", type=int, default=10000,
                  dest="NUMPARTICLES", help="Number of particles per frame")

parser.add_option("-j", "--jobname", default="MakeSNI3MCTrees",
                  dest="JOBNAME", help="Condor name of job")

parser.add_option("-s", "--shell", default="/home/briedel/Software/icesim/trunk2/build/env-shell.sh",
                  dest="ENVSHELL", help="Icetray enviroment shell")

parser.add_option("-e", "--executable", default="/home/briedel/Software/icesim/trunk2/build/SNPS/resoources/SNInjection_examplescript.py",
                  dest="FEXECUTABLE", help="Executable file")

parser.add_option("-l", "--logdir", default="/net/user/briedel/logs/",
                 dest="LOGGINGDIR", help="Logging Directory")

parser.add_option("-m", "--memory", default=4096, 
                  dest="MEMORY", help="Amount of memory requested in [MB]")

parser.add_option("-c", "--CPU", default=1, 
                  dest="CPU", help="Number of CPU requested")

parser.add_option("-D", "--disk", default=11534336, 
                  dest="DISK", help="Amount of disk space in [KB]")

parser.add_option("-d", "--dag", default="SNPS.dag",
                  dest="DAGFILE", help="Name of DAG file")

parser.add_option("-N", "--numfiles", type=int, default=100,
                  dest="NUMFILES", help="Number of jobs inside the DAG")

parser.add_option("-J", "--numjobs", type=int, default=50,
                  dest="NUMJOBS", help="Maximum number of jobs at a time")

parser.add_option("-o", "--outfile",default="MCTreeCLSimSample.i3.bz2",
                  dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz, bz2} format)")

parser.add_option("-i", "--infile", default="MCTreeSample.i3.bz2",
                  dest="INFILE", help="Write input file to INFILE (.i3{.gz, bz2} format)")

parser.add_option("-g", "--gcd",default=expandvars("$I3_SRC/SNPS/resources/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz")
                  dest="GCDFILE", help="Read geometry from GCDFILE (.i3{.gz} format)")

# parse cmd line args, bail out if anything is not understood
(options,args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)

###############################

makesubmitfile(options)
makedagfile(options)

os.system("condor_submit_dag -config dagman.config " + options.DAGFILE)
