def makesubmitfile(options):
    if options.RUNNUMBER != None:
        streamnum = str(options.RUNNUMBER)
    else:
        streamnum = "$(Filenum)"
    SUBMITFILE = open(options.SUBMITFILE,"w")
    SUBMITFILE.write("Fexecutable= /usr/bin/python " + options.FEXECUTABLE + "\n")
    SUBMITFILE.write("executable=" + options.ENVSHELL + "\n")
    SUBMITFILE.write("Jobname=" + options.JOBNAME + "\n")
    SUBMITFILE.write("\n")
    SUBMITFILE.write("logdir=" + options.LOGGINGDIR + "\n")
    SUBMITFILE.write("output=$(Jobname)$(Cluster).out\n")
    SUBMITFILE.write("error=$(Jobname)$(Cluster).err\n")
    SUBMITFILE.write("\n")
    SUBMITFILE.write("universe=vanilla\n")
    SUBMITFILE.write("JobNotification=never\n")
    # SUBMITFILE.write("getenv=true")
    # SUBMITFILE.write("request_cpus=" + str(options.CPU) + "\n")
    # SUBMITFILE.write("request_memory=" + str(options.MEMORY) + "\n")
    # SUBMITFILE.write("request_disk=" + str(options.DISK) + "\n")
    SUBMITFILE.write("\n")
    SUBMITFILE.write("Arguments=$(Fexecutable) -p " + str(options.NUMPARTICLES) + " -n " + str(options.NUMEVENTS) + " -r " + streamnum + " -s $(Cluster) -o /net/user/briedel/cluster/$(Jobname)_$(Cluster).i3.bz2\n" )
    SUBMITFILE.write("Queue")
    SUBMITFILE.close()

def makesubmitfileCLSimOnly(SUBMITFILE, FEXECUTABLE, ENVSHELL, JOBNAME, LOGGINGDIR):
    SUBMITFILE = open(SUBMITFILE,"w")
    SUBMITFILE.write("Fexecutable=" + FEXECUTABLE + "\n")
    SUBMITFILE.write("executable=/usr/bin/python\n")
    SUBMITFILE.write("Jobname=" + JOBNAME + "\n")
    SUBMITFILE.write("\n")
    SUBMITFILE.write("logdir=" + LOGGINGDIR + "\n")
    SUBMITFILE.write("output=$(Jobname)$(Cluster).out\n")
    SUBMITFILE.write("error=$(Jobname)$(Cluster).err\n")
    SUBMITFILE.write("\n")
    SUBMITFILE.write("universe=vanilla\n")
    SUBMITFILE.write("JobNotification=never\n")
    SUBMITFILE.write("getenv=true\n")
    # SUBMITFILE.write("getenv=true")
    # SUBMITFILE.write("request_cpus=" + str(options.CPU) + "\n")
    # SUBMITFILE.write("request_memory=" + str(options.MEMORY) + "\n")
    # SUBMITFILE.write("request_disk=" + str(options.DISK) + "\n")
    SUBMITFILE.write("\n")
    SUBMITFILE.write("Arguments=$(Fexecutable) -o $(OUTFILE) -i $(INFILE) -r $(STREAMNUM) -s $(Cluster)\n" )
    SUBMITFILE.write("Queue")
    SUBMITFILE.close()

# if __name__ == "__main__":
#     makesubmitfile(options)
