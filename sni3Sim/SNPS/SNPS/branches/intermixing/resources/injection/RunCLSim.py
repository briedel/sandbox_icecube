#!/usr/bin/env python
from os.path import expandvars
from optparse import OptionParser

usage = "usage: %prog [options] inputfile"
parser = OptionParser(usage)
parser.add_option("-o", "--outfile",default="MCTreeCLSimSample.i3.bz2",
                  dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz, bz2} format)")
parser.add_option("-i", "--infile", default="MCTreeSample.i3.bz2",
                  dest="INFILE", help="Write input file to INFILE (.i3{.gz, bz2} format)")
parser.add_option("-g", "--gcd",default=expandvars("$I3_SRC/SNPS/resources/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz"),
                  dest="GCDFILE", help="Read geometry from GCDFILE (.i3{.gz} format)")
parser.add_option("-s", "--seed",type="int",default=54321,
                  dest="SEED", help="Initial seed for the random number generator")
parser.add_option("-r", "--runnumber", type="int", default=1,
                  dest="RUNNUMBER", help="The run number for this simulation")


# parse cmd line args, bail out if anything is not understood
(options,args) = parser.parse_args()
if len(args) != 0:
        crap = "Got undefined options:"
        for a in args:
                crap += a
                crap += " "
        parser.error(crap)

###############################

from I3Tray import *

from icecube import icetray, dataclasses, dataio, phys_services, sim_services
from icecube import clsim

tray = I3Tray()

# icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)

tray.AddService("I3SPRNGRandomServiceFactory", "randomService",
                seed=options.SEED,
                nstreams=10000,
                streamnum=options.RUNNUMBER)

tray.AddModule("I3Reader","reader",
               FilenameList=[options.GCDFILE, options.INFILE]
               )

tray.AddSegment(clsim.I3CLSimMakeHits, "makeCLSimHits",
                ParallelEvents = 100,     # this assumes low energies, you will probably run out of memory if your energies are too high
                DoNotParallelize = True,
                UseGPUs=False,
                UseCPUs=True, 
                UseGeant4=True,         # use Geant4 (i.e. no parameterizations)
                DOMOversizeFactor=1.,
                UnweightedPhotons=True,
                PhotonSeriesName="PhotonSeriesMap",
                MMCTrackListName=None,  # no MMC!
                IceModelLocation=expandvars("$I3_SRC/clsim/resources/ice/spice_mie"),
                )

tray.AddModule("Delete", "cleanup", Keys=["I3MCTree_clsim"])

# write the results to disk
tray.AddModule("I3Writer", "writer",
               filename=options.OUTFILE,
               Streams=[icetray.I3Frame.DAQ, icetray.I3Frame.TrayInfo]
               )

tray.AddModule("TrashCan","trash")

tray.Execute()
tray.Finish()
