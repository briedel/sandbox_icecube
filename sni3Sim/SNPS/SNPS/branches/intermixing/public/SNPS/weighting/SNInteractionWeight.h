#ifndef SNINTERACTIONWEIGHT_H_INCLUDED
#define SNINTERACTIONWEIGHT_H_INCLUDED

#include <SNPS/weighting/SNWeightingBaseClass.h>

#include <SNPS/injection/SNInjectionBaseClass.h>

#include <dataclasses/I3Direction.h>
#include <dataclasses/I3Position.h>
#include <dataclasses/physics/I3Particle.h>

/**
 * Class that provides the functions to calculate the interaction weight
 */

class SNInteractionWeight : public SNWeightingBaseClass
{
public:

    SNInteractionWeight(const SNInjectionBaseClass::configuration & c) 
    {
      c_ = c;
    };

    virtual ~SNInteractionWeight() {};

    /**
     * Generate the interaction weight
     *
     * @params Reference to the primary particle
     */
    double GetInteractionWeight( const I3Particle & );

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
     * Taylor expansion of exp(x) up to third order. Needed because 
     * values of x in our case are pretty small and we don't it to default
     * to 1
     * 
     * @params  x value
     * @return  exp(x) or 1 + x + (0.5 * x^2) + (x^3)/6
     */
    double SmallNumExp(double);

private:

  SNInjectionBaseClass::configuration c_;


};



#endif // SNINTERACTIONWEIGHT_H_INCLUDED

