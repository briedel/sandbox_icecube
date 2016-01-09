#ifndef FRAMEOBJECTS_H_INCLUDED
#define FRAMEOBJECTS_H_INCLUDED

#include <icetray/serialization.h>
#include <icetray/I3FrameObject.h>

#include <dataclasses/I3Map.h>
#include <dataclasses/physics/I3ParticleID.h>

/**
 * New frameobject that holds the weights
 */

class SNWeight : public I3FrameObject
{
	
public:

    double InteractionWeight; // Prob. of interactions
    double GenerationWeight; // Prob. of actually being produced by given model
    double OscillationWeight; // Prob. of surviving travel through earth
    double TotalWeight; // Total prob of being detected
    double GenOscWeight; // Gen * Osc weight

private:

    friend class boost::serialization::access;

    template <class Archive> void serialize(Archive &ar, unsigned version);
};

// Creating map to hold the weights for a given I3Particle
typedef I3Map<I3ParticleID, SNWeight > SNWeightMap;
I3_POINTER_TYPEDEFS(SNWeightMap);

#endif //FRAMEOBJECTS_H_INCLUDED
