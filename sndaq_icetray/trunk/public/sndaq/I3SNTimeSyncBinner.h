#ifndef I3SNDAQFILEREADER_H_INCLUDED
#define I3SNDAQFILEREADER_H_INCLUDED

#include <stdio.h>
#include <fstream>

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Logging.h>

#include <sndaq/I3SNDataclasses.h>

#include <dataclasses/physics/I3EventHeader.h>

class I3SNTimeSyncBinner : public I3ConditionalModule
{
public:

    I3SNTimeSyncBinner(const I3Context&);
    
    virtual ~I3SNTimeSyncBinner(){};
    
    void RebinPayload(I3SNPayloadConstPtr, I3EventHeaderConstPtr);
    
    void FillPushOutFrame(I3FramePtr);
    
    void Configure();
    
    void DAQ(I3FramePtr frame);
    
    void Finish();

private:

    std::map<OMKey, boost::shared_ptr<std::deque<uint16_t> > > tempQueueMap_;
    
    I3SNConfigPtr config_;
    
    uint32_t nPayloads_;
    
    uint32_t previousRunNumber;
    
    uint64_t frameLength_;
    
    
    

};

#endif //I3SNDAQFILEREADER_H_INCLUDED
