from icecube.icetray import traysegment
import math, numpy

@traysegment
def SNPSInjection(tray, name,
                  GCD="$I3_SRC/SNPS/resources/GCD/IC86_2013.i3.gz",
                  seed=1,
                  RunNumber=1,
                  Year=2012,
                  DAQTime=158100000000000000,
                  NumberParticles=1000,
                  Interaction="IBD",
                  GenerationSpectrum="Gamma PDF",
                  I3MCPESeriesMapName="MCPESeriesMap",
                  I3MCTreeName="I3MCTree",
                  I3PhotonSeriesName="I3PhotonSeriesMap",
                  ParallelEvents=100,
                  DoNotParallelize=True,
                  UseGPUs=False,
                  UseCPUs=True,
                  Spectrum=True,
                  MonoEnergy=False,
                  NuEnergy = 10.*icetray.I3Units.MeV ):
    
    from icecube import icetray, dataclasses, dataio, phys_services, sim_services
    from icecube import clsim
    from icecube import SNPS
    from os.path import expandvars
    #icetray.set_log_level(icetray.I3LogLevel.LOG_DEBUG)

    tray.AddService("I3SPRNGRandomServiceFactory", "randomService",
                    seed=RunNumber,
                    nstreams=10000,
                    streamnum=seed)

    # make a stream of events
    tray.AddModule("I3InfiniteSource","streams",
                   Prefix=GCD,
                   Stream=icetray.I3Frame.DAQ)

    tray.AddModule("I3MCEventHeaderGenerator","gen_header",
                   Year=Year,
                   DAQTime=DAQTime,
                   RunNumber=RunNumber,
                   EventID=1,
                   IncrementEventID=True)

    tray.AddModule("Delete", "cleanup", Keys=["MCTimeIncEventID"])

    if MonoEnergy and not Spectrum:
        tray.AddModule(SNPS.I3SimpleSNNeutrinoInteractionGenerator,"simpleinjector",
                       NumParticles=NumberParticles,
                       NeutrinoEnergy=NuEnergy)
    else:
        tray.AddModule("SNInjection", "Injector",
                       NumParticlesInFrame=NumberParticles,
                       Interaction=Interaction,
                       GenerationSpectrum=GenerationSpectrum)

    tray.AddSegment(clsim.I3CLSimMakeHits, "makeCLSimHits",
                    ParallelEvents = ParallelEvents, # this assumes low energies, you will probably run out of memory if your energies are too high
                    UseGPUs=UseGPUs,
                    UseCPUs=UseCPUs, 
                    UseGeant4=True,         # use Geant4 (i.e. no parameterizations)
                    DoNotParallelize=DoNotParallelize,
                    DOMOversizeFactor=1., #DOM Pancake Factor
                    UnweightedPhotons=True, 
                    PhotonSeriesName=I3PhotonSeriesName,
                    MMCTrackListName=None,  # no MMC!
                    IceModelLocation=expandvars("$I3_SRC/clsim/resources/ice/spice_mie"), #Ice Model to be used
                    )

    #Getting ride of copy 
    tray.AddModule("Delete", "cleanup_CLSim", Keys=["I3MCTree_clsim"]) 

    tray.AddModule("SNFilePruner", "Pruner",
                   NumPrimariesPerFrame=NumberParticles,
                   InputMCPESeriesMap=I3MCPESeriesMapName)


