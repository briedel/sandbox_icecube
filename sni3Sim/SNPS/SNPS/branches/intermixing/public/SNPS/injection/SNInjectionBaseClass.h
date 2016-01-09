#ifndef SNINJECTIONBASECLASS_H_INCLUDED
#define SNINJECTIONBASECLASS_H_INCLUDED

#include <SNPS/interactions/interaction.h>

#include <icetray/I3Logging.h>

#include <dataclasses/physics/I3Particle.h>

#include <phys-services/I3RandomService.h>

/**
 * BaseClass of IceTray Module to inject low energy neutrinos with a given spectrum
 */

class SNInjectionBaseClass
{

public:

    SNInjectionBaseClass() {};

    virtual ~SNInjectionBaseClass() {};

    // Struct for configuration of injection module
    struct configuration
    {
        double height_;
        double radius_;
        I3RandomServicePtr rng_;
        std::string genspec_;
        I3Position center_;
        double minE_;
        double maxE_;
        double garchingmeanE_;
        double garchingalpha_;
        double gammameanE_;
        double gammaalpha_;
        double gaussianmean_;
        double gaussianstddev_;
        I3Particle::ParticleType primarytype;
        I3Particle::ParticleType product1type;
        I3Particle::ParticleType product2type;
        I3Particle::ParticleShape primaryshape;
        I3Particle::ParticleShape product1shape;
        I3Particle::ParticleShape product2shape;
        boost::shared_ptr<Interaction> inter;
    };

private:

    SET_LOGGER("SNInjectorBaseClass");

};

#endif //SNINJECTIONBASECLASS_H_INCLUDED