from makesubmitfile import makesubmitfile, makesubmitfileCLSimOnly

def makedagfile(options):
  DAGFILE = open(options.DAGFILE, "w")
  if options.RUNNUMBER == None:
    for i in range(options.NUMFILES):
        JOBID = "EFG." + str(i)
        DAGFILE.write("JOB %s %s\n" % (str(JOBID), options.SUBMITFILE) )
        DAGFILE.write("VARS %s Filenum=\"%s\"\n" % (JOBID, str(i)) )
  else:
    for i in range(options.NUMFILES):
      JOBID = "EFG." + str(i)
      DAGFILE.write("JOB %s %s\n" % (str(JOBID), options.SUBMITFILE) )
  DAGFILE.close()

def makedagfileCLSim(files, DAGNAME, SUBMITNAME):
  DAGFILE = open(DAGNAME, "w")
  i = 1
  for f in files:
    JOBID = "EFG." + str(i)
    DAGFILE.write("JOB %s %s\n" % (str(JOBID), SUBMITNAME))
    DAGFILE.write("VARS %s INFILE=\"%s\" OUTFILE=\"%s\" STREAMNUM=\"%d\"\n" % (str(JOBID), f, f[:-7] + "out.i3.bz2", i) )
    i += 1
  DAGFILE.close()

if __name__ == "__main__":
    makesubmitfile.makesubmitfile(options)
    makedagfile(options)
