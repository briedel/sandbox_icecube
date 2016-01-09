#ifndef I3SNTIMESYNCBINNER_H_INCLUDED
#define I3SNTIMESYNCBINNER_H_INCLUDED

#include <sndaq/I3SNTimeSyncBinner.h>

I3_MODULE(I3SNTimeSyncBinner);

I3SNTimeSyncBinner::I3SNTimeSyncBinner(const I3Context& context) : I3ConditionalModule(context),
previousRunNumber(0),
nPayloads_(0),
config_(I3SNConfigPtr(new I3SNConfig()))
{
    
    AddOutBox("OutBox");
    
    AddParameter("I3SNPayloadInputName",
                 "Input name of I3SNPayload object",
                 "I3SNPayload");
    
    AddParameter("I3SNDatagramOutputName",
                 "Output name if I3SNDatagram object",
                 "I3SNDatagram");
    
    AddParameter("DataBinSize",
                 "Bin size of data scalers",
                 (uint32_t)1.6384*I3Units::millisecond);

    AddParameter("TimeSyncedBinSize",
                 "Bin size of time synchronized scalers",
                 (uint32_t)2.*I3Units::millisecond);
    
    AddParameter("FrameLength",
                 "How long a given frame should be",
                 (uint32_t)0.5*I3Units::second);
}

void I3SNTimeSyncBinner::Configure()
{
    uint32_t tmpdatabinsize;
    uint32_t tmptimesyncbinsize;
    
    GetParameter("DataBinSize", tmpdatabinsize);
    GetParameter("TimeSyncedBinSize", tmptimesyncbinsize);
    GetParameter("FrameLength", frameLength_);
    
}

void I3SNTimeSyncBinner::DAQ(I3FramePtr frame)
{
    I3SNPayloadConstPtr payload = frame->Get<I3SNPayloadConstPtr>("I3SNPayload");
    I3EventHeaderConstPtr eventheader = frame->Get<I3EventHeaderConstPtr>("I3EventHeader");
    nPayloads_++;
    
    RebinPayload(payload, eventheader);
    if (nPayloads_ % 12000 == 0)
    {
        I3FramePtr newFrame(new I3Frame(I3Frame::DAQ));
        FillPushOutFrame(newFrame);
    }
}

void I3SNTimeSyncBinner::RebinPayload(I3SNPayloadConstPtr payload, I3EventHeaderConstPtr eventheader)
{
    I3VectorUShort scalers = payload->GetScalers();
    I3Time startTime = payload->GetTime();
    OMKey key = payload->GetOMKey();
    
    I3Time runStartTime = eventheader->GetStartTime();
    
    boost::shared_ptr<std::deque<uint16_t> > timeSyncBins = tempQueueMap_[key];
    
    // log_info("size before timeSyncBins = %lu", timeSyncBins->size());
    // log_info("size scalers = %lu", scalers.size());
    
    // uint32_t absIndex = 0;
    
    I3Time prevAbsTimeLeftEdge;
    
    for (uint32_t scalerIndex = 0; scalerIndex < scalers.size(); scalerIndex++)
    {
        I3Time timeRightEdgeScaler = startTime + (scalerIndex * config->GetDataBinSize());
        I3Time timeLeftEdgeScaler = timeRightEdgeScaler + config->GetDataBinSize();

        uint32_t timeSyncBinNumber = floor((timeRightEdgeScaler - runStartTime) / config->GetTimeSyncBinSize());

        I3Time absTimeRightEdge = runStartTime_ + (timeSyncBinNumber * config->GetTimeSyncBinSize());
        I3Time absTimeLeftEdge = absTimeRightEdge + config->GetTimeSyncBinSize();

        if (scalerIndex == 0) prevAbsTimeLeftEdge = absTimeLeftEdge;

        I3Time leftEdge = std::max(timeRightEdgeScaler, absTimeRightEdge);
        I3Time rightEdge = std::min(absTimeLeftEdge, timeLeftEdgeScaler);

        uint16_t rightSplit = ((rightEdge - leftEdge) / config->GetDataBinSize())*100;
        uint16_t leftSplit = (100 - rightSplit);

        if (prevAbsTimeLeftEdge == absTimeRightEdge) prevAbsTimeLeftEdge = absTimeLeftEdge;

        uint32_t correctedTimeSyncBinNumber = timeSyncBinNumber - 
                                              (nFrames_ * frameLength_ / config->GetTimeSyncBinSize());

        timeSyncBins->resize(correctedTimeSyncBinNumber + 2);

        (*timeSyncBins)[correctedTimeSyncBinNumber] += rightSplit*scalers[scalerIndex];
        (*timeSyncBins)[correctedTimeSyncBinNumber + 1] += leftSplit*scalers[scalerIndex];
    }

}
             
#endif //I3SNTIMESYNCBINNER_H_INCLUDED
