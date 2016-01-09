tray.AddModule("I3MPIReceive")

tray.AddModule("SNReweight", "Reweighting",
               NoParticlesinFrame=10000,
               InjectionSpectrum="Gamma PDF",
               GenParamAlpha=3.0,
               GenParammE=15.0,
               )

tray.AddModule("I3MPIReply")