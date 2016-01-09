#ifndef SNOSCILLATIONS_H_INCLUDED
#define SNOSCILLATIONS_H_INCLUDED


#include <SNPS/weighting/SNWeightingBaseClass.h>
#include <SNPS/weighting/SNOscillationWeight.h>

#include <SNPS/eventselection/SNEventSelectionBaseClass.h>

#include <SNPS/HDFreaderwriter/inputfilereader.h>

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Frame.h>

#include <phys-services/I3RandomService.h>

/**
 * IceTray module to calculate the oscillation weight for a given
 * interacted neutrino
 */

class SNOscillations : public I3ConditionalModule, public SNEventSelectionBaseClass
{

public:

    SNOscillations(const I3Context &ctx);

    virtual ~SNOscillations() {};

    void Configure();

    void DAQ(I3FramePtr);

private:

    SNPSBaseClass::modelintconfiguration c_;
    std::string inputI3MCTreeName_;
    std::string inputSNWeightMapName_;
    std::string inputMCHitSeriesMapName_;
    std::string inputEventHeaderName_;

    bool ehcorr;

    SNOscillationWeight::oscparams params;

    I3Time abstime;
    I3Time starttime;

    boost::shared_ptr<SNOscillationWeight> oscweight;
    
};


#endif // SNOSCILLATIONS_H_INCLUDED


