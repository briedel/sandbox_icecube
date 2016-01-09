#!/usr/bin/env python
import os
import sys
from readmonifile import MonitorFile
from sortmonifile import sort
from stackexample import selection

if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise SystemExit("Please specify a file to process")

    for f in sys.argv[1:]:
        data=sort(f).out()
        print sorted(data['stringHub']['1001'].keys()),data['stringHub'].keys()
        print data.keys()
    
    print 'test'
    app = selection(data)
    app.mainloop()
    print 'testend'