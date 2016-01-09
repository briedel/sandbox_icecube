#!/usr/bin/env python

import sys
import os

from icecube import icetray, dataclasses, dataio, phys_services, interfaces

from icecube import sndaq

from I3Tray import *

import time
now = time.time()

tray = I3Tray()

# print os.environ["I3_SRC"] + "/phys-services/resources/mainboard_ids.xml.gz"
# print os.path.join(os.environ["I3_SRC"], "/phys-services/resources/mainboard_ids.xml.gz")
#
# OMKey2MBIDXML = os.path.join(os.environ["I3_SRC"], "/phys-services/resources/mainboard_ids.xml.gz")
#
# tray.AddService("I3XMLOMKey2MBIDFactory", "OmKey2MbId", InFile=OMKey2MBIDXML)
#
# print OMKey2MBIDXML

# tray.AddModule("I3SNFileReader", "lets_see_what_happens",
#                InFile = "/Users/briedel/debugdata/sn_119378_0001_0000304075_0000608584.dat")

tray.AddModule(sndaq.I3SNFileReader.I3SNReader, "lets_see_what_happens",
               InFile = "/Users/briedel/debugdata/sn_119378_0001_0000304075_0000608584.dat")

# tray.AddModule("Dump")

tray.AddModule("I3Writer", "scribe",
               Filename="test_python_1.i3")

tray.AddModule("TrashCan", "trash")
tray.Execute()
tray.Finish()

after = time.time() - now
#
print after

# os.system("bzip2 test.i3")
