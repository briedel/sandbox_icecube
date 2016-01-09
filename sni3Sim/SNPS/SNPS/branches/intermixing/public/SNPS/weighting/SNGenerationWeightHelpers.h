#ifndef SNGENERATIONWEIGHTHELPERS_H_INCLUDED
#define SNGENERATIONWEIGHTHELPERS_H_INCLUDED

#include <SNPS/weighting/SNGenerationWeight.h>
#include <SNPS/eventselection/SNEventSelectionBaseClass.h>

class SNGenerationWeightHelpers : public SNGenerationWeight, public SNEventSelectionBaseClass
{

public:

    SNGenerationWeightHelpers() {};

    virtual ~SNGenerationWeightHelpers() {};


    /**
     * Get total number of interacted neutrinso for a given model and interaction
     * separated by time bins
     * Assuming a cylindrical volume
     *
     * @params name of model
     * @params distance
     * @params radius of detector volume
     * @params height of detector volume
     * @params maximum energy
     * @params interaction type
     * @return I3Vector of doubles with total number of interacted neutrinos for model
     */
    I3VectorDouble GetTotalNumInteractedNeutrinoTimeBin(std::string, double, \
                                                       double, double, \
                                                       double, std::string);


    /**
	 * Wrapper around GetTotalNumInteractedNeutrinoTimeBin to get total number of 
	 * interacted neutrinos
     * Assuming a cylindrical volume
     *
     * @params name of model
     * @params distance
     * @params radius of detector volume
     * @params height of detector volume
     * @params maximum energy
     * @params interaction type
     * @return I3Vector of doubles with total number of interacted neutrinos for model
     */
    double GetTotalNumInteractedNeutrinoModel(std::string, double, \
                                             double, double, \
                                             double, std::string);

private:

  SNEventSelectionBaseClass::modelintconfiguration c;
};


#endif // SNGENERATIONWEIGHTHELPERS_H_INCLUDED  