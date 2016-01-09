#ifndef SNFRAMEMERGER_H_INCLUDED
#define SNFRAMEMERGER_H_INCLUDED

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Frame.h>
#include <icetray/I3Units.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3EventHeader.h>

#include <simclasses/I3MCPE.h>

#include <SNPS/baseclass/SNPSBaseClass.h>
#include <SNPS/helpers/frameobjects.h>

#include <deque>



/**
 * IceTray module to merge frames to larger size
 */

class SNFrameMerger : public I3ConditionalModule, public SNPSBaseClass
{

public:

    SNFrameMerger(const I3Context &ctx);

    virtual ~SNFrameMerger() {};

    void Configure();

    void DAQ(I3FramePtr);
    
    void Finish();
    
    void CreateNewFrameProcessDequeue(std::deque<I3FramePtr>&);
    
    void ConcatenateFrames( std::deque<I3FramePtr>& , SNPSBaseClass::newframeobjects& );
    
    void ShiftMCTree(I3MCTreePtr, double );
    
    void MergeMCTree(I3MCTreePtr, I3MCTreePtr);
    
    void ShiftMCPESeriesMap(I3MCPESeriesMapPtr, double);
    
    void MergeMCPESeriesMap(I3MCPESeriesMapPtr, I3MCPESeriesMapPtr);
    
    void SNWeightMapCombiner(SNWeightMapPtr, SNWeightMapConstPtr);
    

private:

    std::string inputI3MCTreeName_;
    std::string inputSNWeightMapName_;
    std::string inputMCPESeriesMapName_;
    std::string inputEventHeaderName_;

    double fullFrameLength;
    double queueLength;
    double totalLength;
    double frameLength;
    
    bool ehcorr;
    
    I3Time absolutestarttime;
    
    std::deque<I3FramePtr> frameDeque;
    
    
};


#endif // SNFRAMEMERGER_H_INCLUDED


