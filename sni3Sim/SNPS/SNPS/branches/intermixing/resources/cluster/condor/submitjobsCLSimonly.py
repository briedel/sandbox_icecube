#!/usr/bin/env python

import sys
import os
from os.path import expandvars

from makesubmitfile import makesubmitfileCLSimOnly
from builddag import makedagfileCLSim

fname = "runCLSim"

makedagfileCLSim(sys.argv[1:], fname + ".dag", fname + ".submit")
makesubmitfileCLSimOnly(fname + ".submit", "/net/user/briedel/Software/icesim/trunk/build_npx2/SNPS/resources/RunCLSim.py",  "/net/user/briedel/Software/icesim/trunk/build_npx2/env-shell.sh", fname, "/net/user/briedel/logs")

os.system("condor_submit_dag -config dagman.config " + fname + ".dag")
