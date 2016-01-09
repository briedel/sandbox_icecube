#ifndef SNFRAMEOBJECTCOMBINER_H_INCLUDED
#define SNFRAMEOBJECTCOMBINER_H_INCLUDED

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Frame.h>

#include <SNPS/baseclass/SNPSBaseClass.h>
#include <SNPS/helpers/frameobjectcombiners.h>

/**
 * Icetray Module to combine frames together
 * Question if actually needed
 */

class SNFrameobjectCombiner : public I3ConditionalModule, public SNPSBaseClass
{
public:

    SNFrameobjectCombiner(const I3Context &ctx);

    virtual ~SNFrameobjectCombiner() {};

    void Configure();

    void DAQ(I3FramePtr);

    void Finish();

private:

    std::string inputI3MCTreeName_;
    std::string inputSNWeightMapName_;
    std::string inputMCHitSeriesMapName_;
    std::string inputPhotonSeriesMapName_;
    std::string inputEventHeaderName_;
    std::string outputI3MCTreeName_;
    std::string outputSNWeightMapName_;
    std::string outputMCHitSeriesMapName_;
    std::string outputPhotonSeriesMapName_;
    std::string outputEventHeaderName_;

    frameobjectcombiners instanceFOC;

    SNPSBaseClass::newframeobjects nfo;

};

#endif //SNFRAMEOBJECTCOMBINER_H_INCLUDED
