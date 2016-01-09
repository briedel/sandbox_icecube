from icecube import icetray, dataclasses, dataio, phys_services, sim_services, simclasses, clsim,  hdfwriter, tableio

class PoissonIntermixer(icetray.I3Module):
    """PoissonIntermixer is a I3Module that takes a stream of Q frames from
       supernova sample and adds CORSIKA muons to it given a certain rate"""
    def __init__(self, context):

        icetray.I3Module.__init__(self,context)
        
        self.AddParameter("BackGroundFiles", "Files to inject events from",[])
        # self.AddParameter("MCPESeriesName", "Name of I3MCPESeriesMap object in frame","MCHitSeriesMap")
        # self.AddParameter("MCTreeName", "Name of I3MCTree object in frame", "I3MCTree")
        # self.AddParameter("MMCTrackName", "Name of MMCTrackList object in frame", "MMCTrackList")
        # self.AddParameter("Weights", "Name of WeightMap for injected events", "I3MCWeightDict")
        # self.AddParameter("BGWeights", "Name of WeightMap for base simulation", "CorsikaWeightMap")
        self.AddParameter("Rate", "Event rate (zero if diplopia should get this from summary service)", 0.0);
        self.AddParameter("Seed", "RNG Seed", 0)
        self.AddParameter("RandomService", "RNG Service (superceeds Seed)", None)
        
        self.AddOutBox("OutBox")

    def Configure(self):

        bgfilename = self.GetParameter("BackGroundFiles")
        # self.namemcpe = self.GetParameter("MCPESeriesName")
        # self.namemctree = self.GetParameter("MCTreeName")
        # self.namemmctrack = self.GetParameter("MMCTrackName")
        # self.nameweightsname = self.GetParameter("Weights")
        # self.namebgweights = self.GetParameter("BGWeights")
        self.rate = self.GetParameter("Rate")
        self.seed = self.GetParameter("Seed")
        self.rng = self.GetParameter("RandomService")

        self.i = 0

        if bgfilename == None or bgfilename == "":
            raise RuntimeError("No Background File!")

        self.file = dataio.I3File(bgfilename[self.i],'r')

        self.framemerger = SNPS.framemerger()

    def DAQ(self, frame):

      if (frame.Has("I3EventHeader") and 
          frame.Has("I3MCTree") and 
          frame.Has("MCHitSeriesMap") and
          frame.Has("SNWeightMap")):

            dt = self.framemerger.GetTimeRange(frame["I3EventHeader"])
            numevents = self.rng.poisson( self.rate * dt )
            tbmframe = frame

            for i in range(numevents):
                bgframe = self.file.pop_frame(icetray.I3Frame.DAQ)
                if not bgframe: # If we run out of background before signal, this is bad!
                    self.file.close()
                    self.i += 1
                    if self.i > len(bgfilename):
                        raise RuntimeError("No more background files")
                    self.file = dataio.I3File(bgfilename[self.i],'r')
                    if not self.file:
                        raise RuntimeError("No more background files")
                    bgframe = self.file.pop_frame(icetray.I3Frame.DAQ)
                    if not bgframe:
                        raise RuntimeError("Out of background events!!!!")
                self.framemerger.IntermixingSNCORSIKA(tbmframe, bgframe, self.rng, dt)

            self.PushFrame(tbmframe)
      else:
          raise RuntimeError("Required frame objects not in Supernova file")

    def Finish(self):
        self.file.close()