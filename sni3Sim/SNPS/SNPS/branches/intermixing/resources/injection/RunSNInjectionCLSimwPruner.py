#!/usr/bin/env python
import os
from os.path import expandvars
from optparse import OptionParser

usage = "usage: %prog [options] inputfile"
parser = OptionParser(usage)
parser.add_option("-o", "--outfile",default="test2.i3",
                  dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz, bz2} format)")
parser.add_option("-g", "--gcd",default=os.environ["I3_BUILD"] + "/SNPS/resources/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz",
                  dest="GCDFILE", help="Read geometry from GCDFILE (.i3{.gz} format)")
parser.add_option("-s", "--seed",type="int",default=54321,
                  dest="SEED", help="Initial seed for the random number generator")
parser.add_option("-r", "--runnumber", type="int", default=1,
                  dest="RUNNUMBER", help="The run number for this simulation")
parser.add_option("-n", "--numevents", type="int", default=100000,
                  dest="NUMEVENTS", help="The number of events per run")
parser.add_option("-p","--particles", type=int, default=10000,
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

from icecube import icetray, dataclasses, dataio, phys_services, sim_services
# from icecube import clsim
from icecube import SNPS

import math, numpy

tray = I3Tray()

#icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)

tray.AddService("I3SPRNGRandomServiceFactory", "randomService",
                seed=options.SEED,
                nstreams=10000,
                streamnum=options.RUNNUMBER)

# make a stream of events
tray.AddModule("I3InfiniteSource","streams",
               Prefix=options.GCDFILE,
               Stream=icetray.I3Frame.DAQ)

tray.AddModule("I3MCEventHeaderGenerator","gen_header",
               Year=2009,
               DAQTime=158100000000000000,
               RunNumber=options.RUNNUMBER,
               EventID=1,
               IncrementEventID=True)

tray.AddModule("Delete", "cleanup", Keys=["MCTimeIncEventID"])

tray.AddModule("SNInjection", "Injector",
               NumParticlesInFrame=options.NUMPARTICLES,
               Interaction="IBD",
               GenerationSpectrum="Gamma PDF")

tray.AddSegment(clsim.I3CLSimMakeHits, "makeCLSimHits",
                ParallelEvents = 100,     # this assumes low energies, you will probably run out of memory if your energies are too high
                UseGPUs=False,
                UseCPUs=True, 
                UseGeant4=True,         # use Geant4 (i.e. no parameterizations)
                DoNotParallelize=True,
                DOMOversizeFactor=1., #DOM Pancake Factor
                UnweightedPhotons=True, 
                PhotonSeriesName="PhotonSeriesMap",
                MMCTrackListName=None,  # no MMC!
                IceModelLocation=expandvars("$I3_SRC/clsim/resources/ice/spice_mie"), #Ice Model to be used
                )

#Getting ride of copy 
tray.AddModule("Delete", "cleanup_CLSim", Keys=["I3MCTree_clsim"]) 

tray.AddModule("SNFilePruner", "Pruner",
               NumPrimariesPerFrame=options.NUMPARTICLES,
               InputMCHitSeriesMap=mchitseriesmapname)

# write the results to disk
tray.AddModule("I3Writer", "writer",
               filename=options.OUTFILE,
               Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.TrayInfo])

tray.AddModule("TrashCan","trash")

tray.Execute(options.NUMEVENTS+3)
tray.Finish()
