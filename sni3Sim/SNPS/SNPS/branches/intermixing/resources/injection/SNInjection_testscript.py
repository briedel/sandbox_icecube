#!/usr/bin/env python

import sys
from I3Tray import *
from icecube import icetray, dataclasses, dataio, phys_services, sim_services, tableio, hdfwriter

tray = I3Tray()

tray.AddModule("I3Reader",
               "reader",
               FilenameList=sys.argv[2:])

tray.AddModule("I3NullSplitter", 'nullsplit')

hdftable = hdfwriter.I3HDFTableService(sys.argv[1] + ".h5")

tray.AddModule(tableio.I3TableWriter, "writerHDF",
        tableservice = hdftable, 
        SubEventStreams = ['nullsplit'], 
        keys = ["I3MCTree"])

tray.AddModule("TrashCan","trash")

tray.Execute()
tray.Finish()