#!/usr/bin/env python
# import os
# from optparse import OptionParser

# usage = "usage: %prog [options] inputfile"
# parser = OptionParser(usage)
# parser.add_option("-i", "--infile", default="testin.i3",
#                   dest="INFILE", nargs=3)
# parser.add_option("-o", "--outfile", default="testout.i3",
#                   dest="OUTFILE", help="Write output to OUTFILE (.i3{.gz} format)")
# # parser.add_option("-g", "--gcd",
# #                   default=os.environ["I3_BUILD"] + "/SNPS/resources/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz",
# #                   dest="GCDFILE", help="Read geometry from GCDFILE (.i3{.gz} format)")
# parser.add_option("-s", "--seed", type="int", default=54321,
#                   dest="SEED", help="Initial seed for the random number generator")
# parser.add_option("-r", "--runnumber", type="int", default=1,
#                   dest="RUNNUMBER", help="The run number for this simulation")
# parser.add_option("-n", "--numevents", type="int", default=10,
#                   dest="NUMEVENTS", help="The number of events per run")

# # parse cmd line args, bail out if anything is not understood
# (options, args) = parser.parse_args()
# if len(args) != 0:
#         crap = "Got undefined options:"
#         for a in args:
#                 crap += a
#                 crap += " "
#         parser.error(crap)

# ###############################

# print list(options.INFILE)

import sys

from I3Tray import *

from icecube import icetray, dataclasses, dataio, phys_services, sim_services, hdfwriter, tableio
from icecube import SNPS

tray = I3Tray()

icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)

tray.AddModule("I3Reader",
               "reader",
               FilenameList=sys.argv[2:])


tray.AddService("I3SPRNGRandomServiceFactory",
                "randomService",
                seed=54321,
                nstreams=10000,
                streamnum=1)

# tray.AddModule("Dump", "dump")

tray.AddModule("Delete", "cleanup", Keys=["I3MCTree_clsim"])

tray.AddModule("SNPickEventsQueue", "PickEvent",
               NumParticlesinFrame=1000,
               InjectionSpectrum="Gamma PDF",
               GenParamAlpha=3.0,
               GenParammE=15.0,
               Distance=10000.*icetray.I3Units.parsec)


tray.AddModule("I3NullSplitter", 'nullsplit')

tray.AddModule("SNPSDataCollector", "I3FileBooking",
               InputMCPESeriesName = "MCHitSeriesMap",
               InputPhotonSeriesName = "PhotonSeriesMap", 
               InputMCTreeName = "I3MCTree",
               InputEventHeaderName = "I3EventHeader",
               InputSNWeightName="SNWeightMap",
               OutputMCPEFilterName = "MCPE_Filtered",
               OutputPhotonFilterName = "PhotonSeriesMap_Filtered")

hdftable = hdfwriter.I3HDFTableService(sys.argv[1] + ".h5")
tray.AddModule(tableio.I3TableWriter, "writerHDF",
                tableservice = hdftable, 
                SubEventStreams = ['nullsplit'], 
                keys = ["MCPE_Filtered"]
                )

# write the results to disk
tray.AddModule("I3Writer",
               "writer",
               filename=sys.argv[1],
               Streams=[icetray.I3Frame.TrayInfo, icetray.I3Frame.DAQ])

tray.AddModule("TrashCan","trash")

tray.Execute()
tray.Finish()
