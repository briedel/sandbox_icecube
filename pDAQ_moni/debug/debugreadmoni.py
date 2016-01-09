#!/usr/bin/env python
from readmonifile import MonitorFile
import sys
if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise SystemExit("Please specify a file to process")

    for f in sys.argv[1:]:
        mf = MonitorFile(f)
        mf.dump_data()
        print mf.dump_data().keys()
      
        #print len(mf.dump_data().keys())