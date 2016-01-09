#!/usr/bin/env python
from icecube import icetray, dataclasses, dataio, phys_services, clsim
import math
randomService = phys_services.I3SPRNGRandomService(
    seed = 2,
    nstreams = 10000,
    streamnum = 3)

class I3SimpleSNNeutrinoInteractionGenerator(icetray.I3ConditionalModule):
    def __init__(self, context):
        icetray.I3ConditionalModule.__init__(self, context)
        self.AddParameter("I3MCTreeName",
                          "Name of the MCTree",
                          "I3MCTree")
        self.AddParameter("RandomService",
                          "Random Service to be used",
                          randomService)
        self.AddParameter("NumParticles",
                          "Number of particles per frame",
                          1000)
        self.AddParameter("NeutrinoEnergy",
                          "Energy of primary neutrinos",
                          10.*icetray.I3Units.MeV)
        self.AddParameter("CylinderRadius",
                          "Radius of cylinder surrounding IceCube",
                          800*icetray.I3Units.meter)
        self.AddParameter("CylinderHeight",
                          "Height of cylinder surrounding IceCube",
                          1600*icetray.I3Units.meter)
        self.AddParameter("TimeRange",
                          "Time range over which the neutrino interactions will be injected",
                          2.*icetray.I3Units.second)
        self.AddOutBox("OutBox")
        
        
        
    def Configure(self):
        from icecube import SNPS
        self.mctree = self.GetParameter("I3MCTreeName")
        self.rng = self.GetParameter("RandomService")
        self.nu_eng = self.GetParameter("NeutrinoEnergy")
        self.num_particles = self.GetParameter("NumParticles")
        self.cylinder_radius = self.GetParameter("CylinderRadius")
        self.cylinder_height = self.GetParameter("CylinderHeight")
        self.particle_time_range = self.GetParameter("TimeRange")
        self.ibd = SNPS.IBD()


    def DAQ(self, frame):
        mctree = dataclasses.I3MCTree()
        if self.mctree in frame: del frame[self.mctree]
        for i in range(self.num_particles):
            
            theta_circle = self.rng.uniform(0,2.*math.pi)
            radius_circle = math.sqrt(self.rng.uniform(0,1.))*self.cylinder_radius
            x = radius_circle *  math.cos(theta_circle)
            y = radius_circle * math.sin(theta_circle)
            z = self.rng.uniform(-self.cylinder_height/2., self.cylinder_height/2. )
            
            dir_primary_azimuth = self.rng.uniform(0., 2.*math.pi)
            dir_primary_cos_zenith = self.rng.uniform(-1., 1.)
            dir_primary_zenith = math.acos(dir_primary_cos_zenith)
            
            particle_time = self.rng.uniform(0, self.particle_time_range)
            
            primary_neutrino = dataclasses.I3Particle()
            primary_neutrino.location_type = dataclasses.I3Particle.LocationType.Anywhere
            primary_neutrino.shape = dataclasses.I3Particle.ParticleShape.Primary
            primary_neutrino.type = dataclasses.I3Particle.ParticleType.NuEBar
            primary_neutrino.energy = self.nu_eng
            primary_neutrino.time = particle_time
            primary_neutrino.pos = dataclasses.I3Position(x, y, z)
            primary_neutrino.dir = dataclasses.I3Direction(dir_primary_zenith, dir_primary_azimuth)
            mctree.add_primary(primary_neutrino)
            
            dir_product1_azimuth = self.rng.uniform(0., 2.*math.pi)
            dir_product1_cos_zenith = self.rng.uniform(-1., 1.)
            dir_product1_zenith = math.acos(dir_product1_cos_zenith)
            
            e_product1 = self.ibd.GetE1(self.nu_eng, dir_product1_cos_zenith)
            ke_product1 = self.ibd.GetKE1(e_product1)
            
            product1 = dataclasses.I3Particle()
            product1.location_type = dataclasses.I3Particle.LocationType.InIce
            product1.shape = dataclasses.I3Particle.ParticleShape.StartingTrack
            product1.type = dataclasses.I3Particle.ParticleType.EPlus
            product1.energy = ke_product1
            product1.time = particle_time
            product1.pos = dataclasses.I3Position(x, y, z)
            product1.dir = dataclasses.I3Direction(dir_product1_zenith, dir_product1_azimuth)
            mctree.append_child(primary_neutrino,product1)
            
            dir_product2_azimuth = self.rng.uniform(0., 2.*math.pi)
            dir_product2_cos_zenith = self.rng.uniform(-1., 1.)
            dir_product2_zenith = math.acos(dir_product2_cos_zenith)
            
            e_product2 = self.ibd.GetE2(self.nu_eng, dir_product1_cos_zenith)
            ke_product2 = self.ibd.GetKE2(e_product2)
            
            product2 = dataclasses.I3Particle()
            product2.location_type = dataclasses.I3Particle.LocationType.InIce
            product2.shape = dataclasses.I3Particle.ParticleShape.StartingTrack
            product2.type = dataclasses.I3Particle.ParticleType.Neutron
            product2.energy = ke_product2
            product2.time = particle_time
            product2.pos = dataclasses.I3Position(x, y, z)
            product2.dir = dataclasses.I3Direction(dir_product2_zenith, dir_product2_azimuth)
            mctree.append_child(primary_neutrino, product2)
            
        frame[self.mctree] = mctree
        self.PushFrame(frame)

