from icecube.load_pybindings import load_pybindings
import icecube.icetray

# icecube.icetray.load('sndaq', False)

load_pybindings(__name__,__path__)

import I3SNFileReader
# from sn_payload_reader import sn_payload_reader, sn_data

del icecube
