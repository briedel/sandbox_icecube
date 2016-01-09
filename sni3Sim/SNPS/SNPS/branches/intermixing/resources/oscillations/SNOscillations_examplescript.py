#!/usr/bin/env python

import sys

from I3Tray import *

from icecube import icetray, dataclasses, dataio, phys_services, sim_services, hdfwriter, tableio
from icecube import SNPS

tray = I3Tray()

icetray.set_log_level(icetray.I3LogLevel.LOG_INFO)

tray.AddModule("I3Reader",
               "reader",
               FilenameList=sys.argv[2:])

tray.AddModule("SNOscillations", "Oscillations",
               )
               

tray.AddModule("I3NullSplitter", 'nullsplit')


tray.AddModule("SNPSDataCollector", "I3FileBooking",
               InputMCPESeriesName = "MCPESeriesMap",
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