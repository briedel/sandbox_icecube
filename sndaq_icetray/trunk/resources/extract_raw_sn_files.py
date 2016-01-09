#!/usr/bin/env python
import tarfile
import sys
import os

outdir = "/data/sim/IceCube/2013/generated/CORSIKA-in-ice/photo-electrons/briedel/sn_raw/"

for original_fn in sys.argv[1:]:
    print original_fn
    if tarfile.is_tarfile(original_fn):
        with tarfile.open(original_fn) as first_tar_file:
            for fn_first_tar in first_tar_file.getnames():
                if ".tar" in fn_first_tar:
                    first_tar_file.extract(fn_first_tar, outdir)
                    with tarfile.open(os.path.join(outdir, fn_first_tar)) as second_tar_file:
                        for fn_second_tar in second_tar_file.getnames():
                            if "sn_" in fn_second_tar:
                                second_tar_file.extract(fn_second_tar, outdir)
                    os.remove(os.path.join(outdir, fn_first_tar))
