#ifndef SNINJECTION_H_INCLUDED
#define SNINJECTION_H_INCLUDED

#include <SNPS/injection/SNInjectionBaseClass.h>

#include <SNPS/weighting/SNInteractionWeight.h>

#include <SNPS/interactions/interaction.h>
#include <SNPS/helpers/frameobjects.h>

#include <icetray/I3Frame.h>
#include <icetray/I3Units.h>
#include <icetray/I3Module.h>

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Position.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3Particle.h>

#include <phys-services/I3RandomService.h>

/**
 * IceTray Module to inject low energy neutrinos with a given spectrum
 */

class SNInjection : public I3Module, public SNInjectionBaseClass
{

public:

    SNInjection(const I3Context &ctx);

    virtual ~SNInjection() {};

    void Configure();

    /**
     * Standard implementation to work on the on DAQ frames in 
     * icetray. Generates the empty I3MCTree and SNWeightMap and
     * calls GenerateParticles() to fill both
     *
     * @params  Empty frame to be written to
     */
    void DAQ(I3FramePtr);

    /**
     * Function that fills the MCTree with particles to be considered
     * by CLSim
     *
     * @params  Reference to an empty I3MCTree to be filled
     * @params  Reference to an mempty SNWeightMap
     */
    void GenerateParticles(I3MCTreePtr &, boost::shared_ptr<SNWeightMap> &);

    /**
     * Generate a primary particle within the selected detector volume
     *
     * @params  Reference to empty I3Particle
     */
    void GeneratePrimary(I3Particle & );

    /**
     * Generate the interaction weight
     *
     * @params Reference to the primary particle
     */
    double GetInteractionWeight( I3Particle & );

    /**
     * Generates the products for a given interaction and fills the
     * respective I3Particles
     * 
     * @params  Reference to the primary particle
     * @return  Vector of products of interaction
     */
    std::vector<I3Particle> GenerateProducts( I3Particle & );

    /**
     * Function to rotate the direction of the outgoing products from
     * the target frame to the IceCube direction
     *
     * @params  Direction of neutrino in IceCube frame
     * @params  Direction of products in the target frame
     * @return  Direction of products in IceCube frame
     */
    I3Direction RotateToIceCubeFrame(I3Direction , I3Direction );

    /**
     * Function to calculate the distance from point of interaction
     * to points of intersection with the injection volume (assumed
     * to be a cylinder).
     * std::pair<double, double> = <distance to entrance, distance to exit>
     * Since we chose the interaction point and provide this to the
     * function as the I3Position, the distance to the entrance into
     * the volume should be negative.
     * Code borrowed from J. van Santen MuonGun
     * Adapted from inside D. Chirkin's MMC:
     * http://code.icecube.wisc.edu/svn/projects/mmc/trunk/src/tfa/Amanda.java
     *
     * @params  Position were particle is being injected
     * @params  Direction of the injected particle
     * @return  <Distance to the point of entrance, Distance to point of exit>
     */
    std::pair<double, double> GetIntersection(const I3Position &, const I3Direction &) const;


    /**
     * Function to calculate the distance from the endcaps of a particle.
     * This is needed to simplify the weight calculation, as otherwise
     * the weight would have an angular dependence that would should be
     * reflected in the direction of the neutrino sample.
     * Thanks Kotoyo Hoshina for the discussion
     *
     * @params  Position were particle is being injected
     * @return  <Distance to the point of entrance, Distance to point of exit>
     */
    std::pair<double, double> GetIntersectionCylinderRotated(const I3Position &) const;

    /**
     * Function that calculates the interaction probability given the distance
     * traveled inside the forced interaction volume and the energy of the 
     * neutrino
     * 
     * w = s * n * total cross-section * exp(-n * total cross-section * d)\
     *      1 - exp(-n * total cross-section * s)
     * 
     * @params  Neutrino energy
     * @params  Distance to the entrance and exit points
     * @return  Interaction weight
     */
    double InteractionProb(double, std::pair<double, double>);

    /**
     * Taylor expansion of exp(x) up to third order. Needed because 
     * values of x in our case are pretty small and we don't it to default
     * to 1
     * 
     * @params  x value
     * @return  exp(x) or 1 + x + (0.5 * x^2) + (x^3)/6
     */
    double SmallNumExp(double);



private:

    /// No. of particles per frame
    int nopart_;

    /// Pointer to random number generator
    I3RandomServicePtr rng_;

    /// Interaction type to be considered
    std::string interaction_;
    
    /// Name of output I3MCTree
    std::string outputI3MCTreeName_;
    
    /// Name of output SNWeightMap
    std::string outputSNWeightMapName_;

    SNInjectionBaseClass::configuration c_;

    boost::shared_ptr<SNInteractionWeight> intweight;

    SET_LOGGER("SNInjector");

};

#endif //SNINJECTION_H_INCLUDED
