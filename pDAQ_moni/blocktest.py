#!/usr/bin/env python
from itertools import groupby

def read_blocks(filename):
    """read a file as a sequence of blocks of rstripped non blank lines"""
    with open(filename) as ifh:
        data = ((bool(x), x) for x in (line.rstrip() for line in ifh))
        for b, group in groupby(data, key=lambda z: z[0]):
            if b:
                yield (z[1] for z in group)
                
                
if __name__ == "__main__":
    for block in read_blocks("logfile.txt"):
        print list(block)