#!/usr/bin/env python
import os
from optparse import OptionParser

usage = "usage: %prog [options] inputfile"
parser = OptionParser(usage)
parser.add_option("-i", "--infile", default="testin.i3",
                  dest="INFILE", nargs=3)
parser.add_option("-o", "--outfile", default="testout.i3",
                  dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz} format)")
# parser.add_option("-g", "--gcd",
#                   default=os.environ["I3_BUILD"] + "/SNPS/resources/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz",
#                   dest="GCDFILE", help="Read geometry from GCDFILE (.i3{.gz} format)")
parser.add_option("-s", "--seed", type="int", default=54321,
                  dest="SEED", help="Initial seed for the random number generator")
parser.add_option("-r", "--runnumber", type="int", default=1,
                  dest="RUNNUMBER", help="The run number for this simulation")
parser.add_option("-n", "--numevents", type="int", default=10,
                  dest="NUMEVENTS", help="The number of events per run")

# parse cmd line args, bail out if anything is not understood
(options, args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)

###############################

print list(options.INFILE)

from I3Tray import *

from icecube import icetray, dataclasses, dataio, phys_services, sim_services
from icecube import clsim
from icecube import SNPS

tray = I3Tray()

icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)

tray.AddModule("I3Reader",
               "reader",
               FilenameList=list(options.INFILE))


tray.AddService("I3SPRNGRandomServiceFactory",
                "randomService",
                seed=options.SEED,
                nstreams=10000,
                streamnum=options.RUNNUMBER)

tray.AddModule("Dump", "dump")

tray.AddModule("Delete", "cleanup", Keys=["I3MCTree_clsim"])

# tray.AddModule("I3MPIDispatch", "i3mpi",
#               script )

# write the results to disk
tray.AddModule("I3Writer",
               "writer",
               filename=options.OUTFILE)

tray.AddModule("TrashCan","trash")

tray.Execute()
tray.Finish()