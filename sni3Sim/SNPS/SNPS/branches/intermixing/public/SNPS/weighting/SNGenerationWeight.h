#ifndef SNGENERATIONWEIGHT_H_INCLUDED
#define SNGENERATIONWEIGHT_H_INCLUDED

#include <SNPS/weighting/SNWeightingBaseClass.h>
#include <SNPS/helpers/integrationhelpers.h>
#include <SNPS/interactions/interaction.h>
#include <SNPS/interactions/IBD.h>

#include <map>

/**
 * Class that provides the means to calculate the generation weight
 */ 

class SNGenerationWeight : public SNWeightingBaseClass
{

public:

    SNGenerationWeight()
    {
        integrate = boost::shared_ptr<gsl_integration_wrapper> (new gsl_integration_wrapper);
    };

    virtual ~SNGenerationWeight() {};

    // Struct that holds information about the physics spectrum
    struct weightedphysspec
    {
        std::string physicsspectrum;
        std::string interaction;
        double mEnu;
        double alpha;
    };

    /**
     * Get generation weight from for particle
     *
     * @param Struct with physics information
     * @return Generation weight
     */
    double GetGenWeight( SNPSBaseClass::genweigtinfo & );

    /**
     * Get the total number of neutrinos that will be inside the detector
     *
     * @param Neutrino luminosity in [MeV/s]
     * @param Distance to source in [m]
     * @param Average neutrino energy [MeV]
     * @param Bin size
     * @param Injection area
     * @return Total number of neutrinos in detector
     */
    double GetTotalNoNeutrinos(double, double, double, \
                               double, double);


    /**
     * Get the total number of interacted neutrinos that will be
     * inside the detector for a given time bin
     *
     * @param Neutrino luminosity in [MeV/s]
     * @param Distance to source in [m]
     * @param Average neutrino energy [MeV]
     * @param Time Bin size
     * @param Injection area
     * @param Height of injection volume
     * @param Alpha or pinching parameter
     * @param Maximum energy to be considered
     * @param Interaction type
     * @param Physics spectrum of the supernova model
     * @return Total number of neutrinos in detector
     */
    double GetTotalNoInteractedNeutrinos( double, double , double , \
                                          double, double , double,  \
                                          double, double, \
                                          std::string, std::string);


    /**
     * Get the total number of interacted neutrinos that will be
     * inside the detector for a given time bin
     *
     * @param Interaction type
     * @param Physics spectrum of the supernova model
     * @param Average neutrino energy [MeV]
     * @param Alpha or pinching parameter
     * @param Maximum energy to be considered
     * @return Integral of cross-section weighted energy spectrum
     */
    double GetIntegralCrossSectionSpectrum(std::string, std::string, double, double, double);


    /**
     * Function generator for cross-section weighted energy spectrum
     *
     * @param Physics spectrum of the supernova model
     * @param Interaction type
     * @param Average neutrino energy [MeV]
     * @param Alpha or pinching parameter
     * @return Integral of cross-section weighted energy spectrum
     */
    boost::function<double (double)> GetWeightedPhysSpectrum(std::string, std::string, double, double);


    /**
     * Get value cross-section weighted energy spectrum
     *
     * @param Physics spectrum of the supernova model
     * @param Interaction type
     * @param Energy for which to return the value
     * @param Average neutrino energy [MeV]
     * @param Alpha or pinching parameter
     * @return Value cross-section weighted energy spectrum at given energy
     */
    double GetWeightedPhysSpecValue(std::string, std::string, double, double, double);


    /**
     * Get the detector area. At the moment only the area of a circle
     *
     * TODO: Make it a function of angle, i.e. stationary cylinder.
     *       Code should be in muon gun.
     *
     * @param Cylinder Radius [m]
     * @return Area of circle [m^2]
     */
    double GetGenArea(double);

    /**
     * Get physics probability of neutrino being generated
     *
     * @param Physics spectrum associated with model
     * @param Neutrino Energy
     * @param Average neutrino energy
     * @param Spectral pinch parameter
     * @return Physics probability of neutrino
     */
    double GetPhysicsProb(std::string, double, double, double);

    /**
     * Get value cross-section weighted energy spectrum
     *
     * @param Physics spectrum of the supernova model
     * @param Interaction type
     * @param Average neutrino energy [MeV]
     * @param Alpha or pinching parameter
     * @return Probability of from cross-section weighted energy spectrum
     */
    double GetWeightedPhysProb(std::string, std::string, double, double, double, double);

    /**
     * Get generation probability of neutrino being produced
     *
     * @param Input spectrum associated with sample
     * @param Maximum energy considered in the sample
     * @param Neutrino Energy
     * @param Average neutrino energy
     * @param Spectral pinch parameter
     * @return Physics probability of neutrino
     */
    double GetGenProb(std::string, std::string, double, double, double, double);

    /**
     * Get minimum energy for an interaction
     *
     * @return Minimum neutrino energy considered
     */
    double GetEmin(std::string);

    /**
     * Getting the pointer to the total cross-section function
     *
     * @params Interaction type
     * @return Pointer to total cross-section function for the given interaction
     */
    boost::shared_ptr<Interaction> GetInteractionPtr(std::string interaction);


    // I3VectorDouble GetTotalNoInteractedNeutrinoTimeBin(std::string, std::string, double, double, double, double, std::string);

    // double GetTotalNoInteractedNeutrinoModel(std::string, double, double, double, double, std::string);


private:

    boost::shared_ptr<gsl_integration_wrapper> integrate;

    struct multiply
    {
        typedef boost::function<double (double)> func_t;
        multiply(func_t f, func_t g) : f_(f), g_(g) {};
        func_t f_, g_;
        typedef double result_type;
        inline double operator()(double x) const
        {
            return f_(x) * g_(x);
        }
    };


    typedef std::map<std::pair<double, double>, double> imap;
    imap integralmap;

    typedef std::map< weightedphysspec, boost::function<double (double)> > fmap;
    fmap functionmap;

    SNGenerationWeight::modelintconfiguration c;

    weightedphysspec wps;

};


/**
 * == and < operator overloading to compare weightedphysspec struct 
 * to be able to use it as a key in a map
 *
 * @params First weightedphysspec
 * @params Second weightedphysspec
 * @return Boolean for condition
 */
bool operator==(const SNGenerationWeight::weightedphysspec &, const SNGenerationWeight::weightedphysspec &);

bool operator<(const SNGenerationWeight::weightedphysspec &, const SNGenerationWeight::weightedphysspec &);

#endif // SNGENERATIONWEIGHT_H_INCLUDED