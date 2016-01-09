#ifndef SNOSCILLATIONWEIGHT_H_INCLUDED
#define SNOSCILLATIONWEIGHT_H_INCLUDED

#include <SNPS/weighting/SNWeightingBaseClass.h>

#include <dataclasses/physics/I3Particle.h>

/**
 * Class that provides the means to calculate the oscillation weight
 * Currently UNTESTED. NEEDS SERIOUS DEBUGGING
 */ 


class SNOscillationWeight : public SNWeightingBaseClass
{

public:

    SNOscillationWeight(SNPSBaseClass::modelintconfiguration & c)
    {
        c_ = c;
        timebefore = 0.;
        lumin_nuinit = 0;
        lumin_nufinal= 0;
        alpha_nuinit= 0;
        alpha_nufinal= 0;
        mEnu_nuinit= 0;
        mEnu_nufinal= 0;
        timecorr = 0;
    };

    virtual ~SNOscillationWeight() {};

    // Struct that holds the oscillation parameters
    struct oscparams
    {
        std::string hierarchy;
        double sintheta12;
        double sintheta13;
        double sintheta23;
        double deltamsq12;
        double deltamsq23;
        double deltamsq13;
        double deltacp;

    };


    /**
     * Function that provides outside access to oscillation weight caluclation
     * 
     * @params Oscillation parameters struct
     * @params I3PArticle for which to calculate the oscillation weight
     * @params Start time of event
     * @params absolute time
     * @return oscillation weight
     */
    double GetOscillationWeight(oscparams, \
                                I3Particle, \
                                I3Time, I3Time);


    /**
     * Setting the way the oscillation probability is calculated 
     * for a given mass hierarchy  
     *
     * @params Oscillation parametere
     */
    void SetOscillationProbability(oscparams);


    /**
     * Setting oscillation probabilty given the particle type
     *
     * @params oscillation parameters
     * @params I3Particle for which to calculate the oscillation weight
     */
    double SetFlavorOscillationProb(oscparams, I3Particle);

    /**
     * Calculate the ratio between fluxes for the respective oscillation
     * patners
     *
     * @params I3Particle for which to calculate the oscillation weight
     * @params start time
     * @params absolute time
     * @return flux ratio
     */
    double GetInitialFluxesRatio(I3Particle, I3Time, I3Time);


    /**
     * Get the probability for a given spectrum
     *
     * @params neutrino energy 
     * @params spectral pinching parameters
     * @params mean energy of spectrum
     * @return probability of having neutrino energy
     */
    double GetSpectralProbability(double, double, double);

    /**
     * Putting it all together and calculating the weight
     *
     * @params oscillation probability
     * @params flux ratio
     * @return oscillation weight
     */
    double CalculateWeight(double, double);

private:

    double p;
    double pbar;

    double timebefore;
    
    double lumin_nuinit;
    double lumin_nufinal;
    double alpha_nuinit;
    double alpha_nufinal;
    double mEnu_nuinit;
    double mEnu_nufinal;

    double timecorr ;

    SNPSBaseClass::modelintconfiguration c_;

};


#endif // SNOSCILLATIONWEIGHT_H_INCLUDED