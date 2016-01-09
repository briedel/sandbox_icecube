#!/usr/bin/env python
import os
from os.path import expandvars
from optparse import OptionParser

usage = "usage: %prog [options] inputfile"
parser = OptionParser(usage)
parser.add_option("-o", "--outfile",default="testSNPSinjection_out.i3",
                  dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz, bz2} format)")
parser.add_option("-g", "--gcd",default=os.environ["I3_BUILD"] + "/SNPS/resources/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz",
                  dest="GCDFILE", help="Read geometry from GCDFILE (.i3{.gz} format)")
parser.add_option("-s", "--seed",type="int",default=54321,
                  dest="SEED", help="Initial seed for the random number generator")
parser.add_option("-r", "--runnumber", type="int", default=1,
                  dest="RUNNUMBER", help="The run number for this simulation")
parser.add_option("-n", "--numevents", type="int", default=1000,
                  dest="NUMEVENTS", help="The number of events per run")
parser.add_option("-p","--particles", type=int, default=1000,
                  dest="NUMPARTICLES", help="Number of particles per frame")

# parse cmd line args, bail out if anything is not understood
(options,args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)

###############################

mchitseriesmapname = "MCPESeriesMap"
mctreename = "I3MCTree"


from I3Tray import *

from icecube import icetray, dataclasses, dataio
from icecube import SNPS

import math, numpy

tray = I3Tray()

#icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)


tray.AddSegment(SNPS.SNPSInjection, "Injection",
                GCD=options.GCDFILE,
                seed=options.SEED,
                RunNumber=options.RUNNUMBER,
                NumberParticles=options.NUMPARTICLES,
                DoNotParallelize=False,
                # MonoEnergy = True,
                # NuEnergy = 100*icetray.I3Units.MeV
                )

# write the results to disk
tray.AddModule("I3Writer", "writer",
               filename=options.OUTFILE,
               Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.TrayInfo])

tray.AddModule("TrashCan","trash")

tray.Execute(options.NUMEVENTS+4)
tray.Finish()
