def CLSim(tray, name, InputMCTree='I3MCTree', DropMCTree=100, OutputPESeriesMapName='I3MCPESeriesMap',
    IceModel='SpiceLea', UseGPUs=False, MaxParallelEvents=100,
    UnshadowedFractions=(0.9,0.99,1.08), HybridMode=False, DOMOversizeFactor=5):
    """
    Propagate photons to DOMs, prescaling to multiple DOM efficiencies along the way.
    
    :param DropMCTree: Delete the propagated I3MCTree, leaving only the original particles
                       without secondary losses. If this parameter is set to an integer N, drop
                       on all but 1 out of every N frames, otherwise drop unconditionally.
    """
    from icecube import icetray, clsim
    from os.path import expandvars

    RandomService = tray.context['I3RandomService']

    table_base = expandvars('$I3_DATA/photon-tables/splines/')
    if IceModel == "Spice1":
        clsimIceModel = expandvars("$I3_SRC/clsim/resources/ice/spice_1")
        table_base += "ems_spice1_z20_a10.%s.fits"
    elif IceModel == "SpiceMie":
        clsimIceModel = expandvars("$I3_SRC/clsim/resources/ice/spice_mie")
        table_base += "ems_mie_z20_a10.%s.fits"
    elif IceModel == "SpiceLea":
        clsimIceModel = expandvars("$I3_SRC/clsim/resources/ice/spice_lea")
        table_base += "ems_lea_z20_a10.%s.fits"
    else:
        raise RuntimeError("Unknown ice model: %s", IceModel)

    
    # Intermediate objects to be deleted at the end of the segment
    temporaries = []
    
    if HybridMode:
        tray.AddModule("I3MCTreeHybridSimulationSplitter", name+"_splitMCTree",
            InputMCTreeName=InputMCTree,
            OutputMCTreeNameTracks=InputMCTree+"Tracks",
            OutputMCTreeNameCascades=InputMCTree+"Cascades")
        temporaries += [InputMCTree+"Tracks", InputMCTree+"Cascades"]
        CLSimMCTree = InputMCTree+"Tracks"
    else:
        CLSimMCTree = InputMCTree
    
    # AMD's OpenCL implemenation starts one thread for each core. If taskset is
    # being used to pin the parent process to a specific CPU, then the Linux
    # scheduler may in some circumstances schedule all threads on the same core,
    # resulting in 100%/N CPU usage rather than 100%. Start a background
    # thread that will reset the CPU affinity once the OpenCL threads are
    # spawned (1 minute should be enough).
    if not UseGPUs:
        from icecube.simprod.segments.HybridPhotonicsCLSim import tasksetInUse, resetTasksetThreads
        from threading import Thread
        if tasksetInUse():
            Thread(target=resetTasksetThreads,args=(os.getpid(),)).start()
    
    # simulate tracks (with clsim)
    tray.AddSegment(clsim.I3CLSimMakePhotons, name+"_makeCLSimHits",
        PhotonSeriesName=name+"_intermediatePhotons",
        MCTreeName = CLSimMCTree,
        OutputMCTreeName=CLSimMCTree+"_sliced",
        # MCPESeriesName = OutputPESeriesMapName,
        MMCTrackListName = "MMCTrackList",
        ParallelEvents = MaxParallelEvents,
        RandomService = RandomService,
        UnshadowedFraction=max(UnshadowedFractions),
        DoNotParallelize=not UseGPUs, # you may need to turn this on for clusters that assume "1 job == 1 core"
        UseGeant4=False, # never use this with Geant4!
        UseGPUs=UseGPUs,
        UseCPUs=not UseGPUs,
        IceModelLocation=clsimIceModel,
        DisableTilt=HybridMode,
        DOMOversizeFactor=DOMOversizeFactor
        )
    temporaries.append(name+"_intermediatePhotons")
    temporaries.append(CLSimMCTree+"_sliced")
    
    # now, prescale photons to make MCPEs for each DOM efficiency
    outputs = []
    for eff in UnshadowedFractions:
        label = "%s_%.3f" % (OutputPESeriesMapName, eff)
        outputs.append(label)
        tray.AddSegment(clsim.I3CLSimMakeHitsFromPhotons, name+"_makePhotons_%.3f" % (eff),
            MCTreeName=CLSimMCTree+"_sliced", PhotonSeriesName=name+"_intermediatePhotons",
            MCPESeriesName=label, RandomService=RandomService, UnshadowedFraction=eff)
    
    # draw cascade photons from spline tables
    if HybridMode:
        from icecube import photonics_service
        cascade_service = photonics_service.I3PhotoSplineService(
            table_base % "abs", table_base % "prob", 0.)
        for eff, hitlabel in zip(UnshadowedFractions, outputs):
            tray.AddModule("I3PhotonicsHitMaker", name+"_hitsFromTheTable_%f" % eff,
                CascadeService = cascade_service,
                TrackService = None, # tracks are handled by clsim
                UnshadowedFraction = eff,
                Input = InputMCTree+"Cascades",
                Output = hitlabel+"Cascades",
                RandomService = RandomService
                )
            temporaries.append(hitlabel+"Cascades")
            tray.Add("Rename", keys=[hitlabel, hitlabel+"Tracks"])
            tray.AddModule("I3CombineMCPE", name+"_combine_pes_%f" % eff,
                InputResponses = [hitlabel+"Tracks", hitlabel+"Cascades"],
                OutputResponse = hitlabel)
    
    tray.AddModule("Delete", name+"_cleanup",
        Keys = temporaries)
    
    if DropMCTree:
        if isinstance(DropMCTree, int):
            prescale = lambda frame: frame.Stop == frame.DAQ and RandomService.uniform(0, DropMCTree) > 1
        else:
            prescale = None
        tray.AddModule("Delete", name+"_mctree_cleanup", Keys=[InputMCTree, "MMCTrackList"], If=prescale)
    
    return outputs