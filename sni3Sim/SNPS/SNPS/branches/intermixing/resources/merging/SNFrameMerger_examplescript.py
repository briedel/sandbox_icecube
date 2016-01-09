#!/usr/bin/env python
import sys

from I3Tray import *

from icecube import icetray, dataclasses, dataio, phys_services, sim_services, hdfwriter, tableio
from icecube import SNPS

tray = I3Tray()

icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)

tray.AddModule("I3Reader",
               "reader",
               FilenameList=sys.argv[1:])


# tray.AddModule("dump")

tray.AddModule("SNFrameMerger",
               "merger",
               FrameLength = 2.*icetray.I3Units.millisecond)
               
tray.AddModule("Dump")
               

# write the results to disk
tray.AddModule("I3Writer",
               "writer",
               filename=sys.argv[1][:-7] + "_merged_2.i3.bz2",
               Streams=[icetray.I3Frame.TrayInfo, icetray.I3Frame.DAQ])

tray.AddModule("TrashCan","trash")

tray.Execute()
tray.Finish()
