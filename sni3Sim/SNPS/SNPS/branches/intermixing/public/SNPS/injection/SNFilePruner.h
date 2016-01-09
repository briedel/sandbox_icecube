#ifndef SNFILEPRUNER_H_INCLUDED
#define SNFILEPRUNER_H_INCLUDED

#include <SNPS/injection/SNInjectionBaseClass.h>

#include <SNPS/helpers/frameobjecthelpers.h>
#include <SNPS/helpers/frameobjects.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3EventHeader.h>

#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>

/**
 * IceTray Module to remove neutrinos from MCtree that did not produce a hit on a DOM
 */

class SNFilePruner : public I3Module, public SNInjectionBaseClass
{
public:

    SNFilePruner(const I3Context &ctx);

    virtual ~SNFilePruner() {};

    void Configure();

    void DAQ( I3FramePtr );

private:

    std::string inputI3MCTreeName_;
    std::string inputSNWeightMapName_;
    std::string inputMCHitSeriesMapName_;
    std::string inputPhotonSeriesMapName_;
    std::string inputEventHeaderName_;

    bool noPhotonHitSeriesMap;
    bool noSNWeightMap;

    int numPrimaries;
    
    SET_LOGGER("SNFilePruner");

};


#endif // SNFILEPRUNER_H_INCLUDED