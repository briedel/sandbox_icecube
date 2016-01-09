#ifndef SNPICKEVENTSQUEUE_H_INCLUDED
#define SNPICKEVENTSQUEUE_H_INCLUDED

#include <SNPS/eventselection/SNEventSelectionBaseClass.h>
#include <SNPS/weighting/SNWeightingBaseClass.h>
#include <SNPS/weighting/SNGenerationWeight.h>
#include <SNPS/timing/SNCorrectTime.h>

#include <SNPS/HDFreaderwriter/inputfilereader.h>

#include <SNPS/interactions/IBD.h>

#include <icetray/I3Module.h>
#include <icetray/I3Frame.h>

#include <phys-services/I3RandomService.h>

#include <queue>

/**
 * Icetray Module to contruct supernova event out of sample with known size (per frame)
 * and spectrum. This model calculates the number of frames needed per model time bin, 
 * concates them and applies the time shift according to the input model. It uses the 
 * SNGenerationWeight class to calculate per neutrino weight.
 */

class SNPickEventsQueue : public I3Module, public SNEventSelectionBaseClass
{

public:

    SNPickEventsQueue(const I3Context &ctx);

    virtual ~SNPickEventsQueue() {};


    /**
     * Standard function for IceTray module
     */
    void Configure();

    void Process();

    /**
     * Concating frames and apply proper time shifts according to model
     *
     * @params: Queue of I3FramePtr
     * @params: Set of new frameobjects that will be put into the frame
     */
    void ConcatFOApplyWeightsTimeShift(std::queue<I3FramePtr> &, SNPSBaseClass::newframeobjects &);

private:

    // Absolute time
    double t;

    // Number of frames that are needed for specific time bin
    unsigned int numFramesNeeded;

    // Input parameters
    std::string model_;
    std::string hierarchy_;
    std::string shocktype_;
    std::string interactions_;
    std::string modelfile_;
    std::string inputspectrum_;
    std::string physicsspectrum_;
    std::string inputI3MCTreeName_;
    std::string inputSNWeightMapName_;
    std::string inputMCHitSeriesMapName_;
    std::string inputPhotonSeriesMapName_;
    std::string inputEventHeaderName_;

    double distance_;
    double nopart_;
    double radius_;
    double height_;
    double maxE_;
    double t_;
    double area_;
    double oversamplingfactor_;

    bool ehcorr;

    I3RandomServicePtr rng_;

    // Debugging stuff for counting frames that have passed
    int noIFrames;
    int noGFrames;
    int noCFrames;
    int noDFrames;
    int noQFrames;
    int noPFrames;

    // Instance of generation weight class to calculate weights
    SNGenerationWeight instancegenweight;

    // Instance of time correction class
    SNCorrectTime instancecortime;

    // Model config struct instance
    SNPSBaseClass::modelintconfiguration c;

    // Iterator for vector of bin widths
    std::vector<double>::const_iterator vecitdt;

    // Queue for frame pointers
    std::queue<I3FramePtr> frameQueue;
};


#endif // SNPICKEVENTSQUEUE_H_INCLUDED


