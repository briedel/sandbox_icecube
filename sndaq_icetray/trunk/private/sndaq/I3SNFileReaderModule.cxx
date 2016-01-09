#include <sndaq/I3SNFileReaderModule.h>
#include <sndaq/I3SNDataclasses.h>
#include <sndaq/I3SNHelpers.h>

#include <icetray/I3Units.h>
#include <icetray/I3Frame.h>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <ctime>
#include <iostream>

// #include <boost/date_time/posix_time/posix_time.hpp>

I3_MODULE(I3SNFileReader);

I3SNFileReader::I3SNFileReader(const I3Context& context) : I3ConditionalModule(context),
nPayloads_(0),
fileNumber_(0),
runNumber_(0),
nFrames_(0),
inDir_(""),
infileName_(""),
GDFileName_(""),
I3GeometryName_("I3Geometry"),
badDOMsListName_("BadDomsList")
{

    config = I3SNConfigPtr(new I3SNConfig());

    AddParameter("InDirectory",
                 "Directory where input files can be found",
                 inDir_);

    AddParameter("InFile",
                 "One input file",
                 infileName_);

    // uint32_t defaultDataBinSize = 1.6384*I3Units::millisecond;
    AddParameter("DataBinSize",
                 "Bin size of data scalers",
                //  defaultDataBinSize);
                (uint32_t)1.6384*I3Units::millisecond);

    // uint32_t defaultTimeSyncedBinSize = 2.*I3Units::millisecond;
    AddParameter("TimeSyncedBinSize",
                 "Bin size of time synchronized scalers",
                //  defaultTimeSyncedBinSize);
                (uint32_t)2.*I3Units::millisecond);

    AddParameter("Year",
                 "Year to be used",
                 0);

    uint32_t defaultFrameLength = 0.5*I3Units::second;
    AddParameter("FrameLength",
                 "How long a given frame should be",
                 defaultFrameLength);

    AddParameter("I3GeometryName",
                 "Name of I3Geometry",
                 I3GeometryName_);

    AddParameter("BadDOMListName",
                 "Name of BadDOMList",
                 badDOMsListName_);

     // AddParameter("GeometryBadDOMListFile",
     //              "File with the I3Geometry and BadDOMList",
     //              )

    AddOutBox("OutBox");
}

void I3SNFileReader::Configure()
{

    log_info("Config SNDAQ");

    uint32_t tmpdatabinsize;
    uint32_t tmptimesyncbinsize;
    uint32_t year;

    GetParameter("InDirectory", inDir_);
    GetParameter("InFile", infileName_);
    // GetParameter("Year", year);
    GetParameter("DataBinSize", tmpdatabinsize);
    GetParameter("TimeSyncedBinSize", tmptimesyncbinsize);
    GetParameter("Year", year);
    GetParameter("FrameLength", frameLength_);
    GetParameter("I3GeometryName", I3GeometryName_);
    GetParameter("BadDOMListName", badDOMsListName_);

    config->SetDataBinSize(tmpdatabinsize);
    config->SetTimeSyncBinSize(tmptimesyncbinsize);

    if ((inDir_.empty()) && (infileName_.empty())) log_fatal("Need to provide an in directory or file");
    if (!(inDir_.empty()) && !(infileName_.empty())) log_fatal("You can only provide an in directory or an input file not both");

    log_info("DataBinSize = %10" PRId64 " ns", config->GetDataBinSize());
    log_info("TimeSyncedBinSize = %10" PRId64 " ns", config->GetTimeSyncBinSize());
    log_info("InFile = %s", infileName_.c_str());

    OpenFiles();

    if (year == 0)
    {
        log_info("Getting run start time from local time");

        time_t now = time(0);
        tm *ltm = localtime(&now);
        runStartYear_ = I3Time(1900 + ltm->tm_year, 0);

    }
    else
    {
        runStartYear_ = I3Time(year, 0);
    }

    log_info("runStartYear_ = %s", runStartYear_.GetUTCString().c_str());

    omkey2mbid = context_.Get<boost::shared_ptr<I3OMKey2MBID> >();

    DOMStatMap_ = I3SNDOMStatusMapPtr(new I3SNDOMStatusMap());

    log_info("Done configuring");

}

// uint32_t I3SNFileReader::GetYear()
// {
//     time_t now = time(0);
//     tm *ltm = localtime(&now);
//     return ltm->tm_year + 1900;
// }

void I3SNFileReader::Geometry(I3FramePtr frame)
{
    log_info("Geometry frame");
    geometry = frame->Get<I3GeometryConstPtr>(I3GeometryName_);
    // Initialize the queues when
    ResetQueueMap();
    ResetDOMStatusMap();
}

void I3SNFileReader::DetectorStatus(I3FramePtr frame)
{
    log_info("DetectorStatus frame");
    badDOMList = frame->Get<I3VectorOMKeyConstPtr>(badDOMsListName_);
}

void I3SNFileReader::DAQ(I3FramePtr frame)
{
    // log_info("DAQ frame");
    ReadRebinPayloads(frame);
    if ( infile_.eof() ) RequestSuspension();

    // if (!infile_) log_fatal("File is not open or broken");
    //
    // while( !infile_.eof() )
    // {
}

// void I3SNFileReader::Setup()
// {
//
//
//
// }

// void I3SNFileReader::Process()
// {
//     ReadRebinPayloads();
//     if ( infile_.eof() ) RequestSuspension();
// }

void I3SNFileReader::ResetQueueMap()
{
    BOOST_FOREACH(const I3OMGeoMap::value_type& geo_pair, geometry->omgeo)
    {
        if (tempQueueMap_.find(geo_pair.first) != tempQueueMap_.end())
        {
            tempQueueMap_[geo_pair.first]->clear();
        }
        else
        {
            tempQueueMap_[geo_pair.first] = boost::shared_ptr<std::deque<uint16_t> >(new std::deque<uint16_t>());
        }


    }
}

void I3SNFileReader::ResetDOMStatusMap()
{
    BOOST_FOREACH(const I3OMGeoMap::value_type& geo_pair, geometry->omgeo)
    {
        (*DOMStatMap_)[geo_pair.first] = I3SNDOMStatus();
    }
}

void I3SNFileReader::OpenFiles()
{
    // if (!inDir_.empty())
    // {
    //     /// TODO: Create here to glob the in file for certain file patterns,
    //               if files are not found or already processed then wait
    //               and continue again
    //     bool newFile;
    //     while (!newFile)
    //     {
    //         boost::filesystem::exists();
    //     }
    // }
    if (!infileName_.empty())
    {
        infile_.open(infileName_.c_str(), std::ios::in | std::ios::binary);
    }

}

bool I3SNFileReader::IsPayloadUseable(I3SNPayloadPtr payload)
{
    
}

void I3SNFileReader::ReadPayloads(I3FramePtr frame)
{
    do
    {
        I3SNPayloadPtr payload = ReadPayload(infile_);
        if (!(IsPayloadUseable(payload))) continue;
        frame->Put("I3SNPayload", payload);
        PushFrame(frame);

    } while ( infile_.peek() == EOF );
}

void I3SNFileReader::ReadRebinPayloads(I3FramePtr frame)
{

    // log_info("Got Frame.....");

    if (!infile_) log_fatal("File is not open or broken");

    // infile_.peek();

    if (infile_.peek() == EOF) log_info("file at end?");

    do
    {
        I3SNPayloadPtr payload = ReadPayload(infile_);
        if (!(IsPayloadUseable(payload))) continue;
        // frame->Put("I3SNPayload", payload);
        // PushFrame(frame);

        RebinPayload(payload);
        if (nPayloads_ % 12000 == 0)
        {
            FillPushOutFrame(frame);
        }
    } while ( infile_.peek() == EOF );

    // PushFrame(frame);

    // I3FramePtr frame(new I3Frame(I3Frame::DAQ));

    // I3EventHeaderPtr header(new I3EventHeader());



    // I3SNDatagramMapPtr datagram = RebinPayload(payload);
    //
    // log_info("setting up Header");
    // header->SetStartTime((*datagram)[payload->GetOMKey()].GetTime());
    // header->SetEndTime((*datagram)[payload->GetOMKey()].GetTime() +
    //                    config->timeSyncBinSize*
    //                    (*datagram)[payload->GetOMKey()].GetTimeSyncScalers().size());
    // log_info("setting up Header");
    // frame->Put("I3EventHeader", header);
    // frame->Put("I3SNDatagramMap", datagram);
    // frame->Put("I3SNPayload", payload);
    // log_info("Push frame");
    // PushFrame(frame);
    // log_info("Frame Pushed");
}

I3SNPayloadPtr I3SNFileReader::ReadPayload(std::ifstream& payloadFile)
{

    // log_info("Read payload");

    if (!payloadFile.good()) log_fatal("payload file has ended");
    int32_t payloadLength(0), payloadType(0);
    int16_t scalerBlockLength(0), formatID(0);
    int64_t timeTenthsNs(0), DOMHexID(0);
    char DOMClockCycles[6];

    /*
     *  Reading in payload. The order is important because are reading in a
     *  binary file.
     */

    /*
     * Reading pDAQ header
     */

    // Reading length of binary file package
    payloadFile.read((char*)&payloadLength, 4);
    if (!payloadFile)
    {
       log_error("Could not read length of binary file package (payloadLength). Skipping remainder of file");
       // TODO: Deal with file... Rename, etc.
       // return;
    }
    // Reading payload type
    payloadFile.read((char*)&payloadType, 4);
    if (!payloadFile)
    {
       log_error("Could not read payload type (payloadType). Skipping remainder of file");
       // return;
    }
    // Reading payload time in 0.1 ns
    payloadFile.read((char*)&timeTenthsNs, 8);
    if (!payloadFile)
    {
       log_error("Could not read time in 0.1 ns (timeTenthsNs). Skipping remainder of file");
       // return;
    }


    /*
     * Reading DOM payload
     */

    // Reading DOM Hex ID
    payloadFile.read((char*)&DOMHexID, 8);
    if (!payloadFile)
    {
       log_error("Could not read DOM mainboard hex id (DOMHexID). Skipping remainder of file");
       // return;
    }
    // Reading lenght of DOM payload
    payloadFile.read((char*)&scalerBlockLength, 2);
    if (!payloadFile)
    {
       log_error("Could not read length of scaler data (scalerBlockLength). Skipping remainder of file");
       // return;
    }
    // Reading format ID
    payloadFile.read((char*)&formatID, 2);
    if (!payloadFile)
    {
       log_error("Could not read format id (formatID). Skipping remainder of file");
       // return;
    }
    // Reading Number of DOM clock cycles
    payloadFile.read(DOMClockCycles, 6);
    if (!payloadFile)
    {
       log_error("Could not read DOM mainboard hex id (DOMHexID). Skipping remainder of file");
       // return;
    }

    // Convert from big endian of ARM used on DOM
    // to little endian of X86 on the server
    I3SNHelpers::ByteSwap5(payloadLength);
    I3SNHelpers::ByteSwap5(payloadType);
    I3SNHelpers::ByteSwap5(scalerBlockLength);
    I3SNHelpers::ByteSwap5(formatID);
    I3SNHelpers::ByteSwap5(timeTenthsNs);
    I3SNHelpers::ByteSwap5(DOMHexID);

    int64_t numDOMClockCycles = I3SNHelpers::ByteSwapDOMClock(DOMClockCycles);

    log_debug("payloadLength = %d", payloadLength );
    log_debug("payloadType = %d", payloadType );
    log_debug("scalerBlockLength = %d", scalerBlockLength );
    log_debug("formatID = %d", formatID );
    log_debug("time in = %10" PRId64 " x 0.1 ns", timeTenthsNs);

    if ( (payloadType != 16) || (formatID != 300) )
    {
       log_error("Wrong Payload type or Format id!");
       // return;
    }

    OMKey key = omkey2mbid->GetOMKey(DOMHexID);

    /// TODO: CHECK PAYLOADS... Jitter, etc.

    /*
    * Reading scalers
    */
    // Estimating how long the scaler package is by removing the size of the header from
    // the payload length.
    uint32_t headerSize = sizeof(payloadLength) + sizeof(payloadType) + sizeof(scalerBlockLength) +
                          sizeof(formatID) + sizeof(DOMHexID) + sizeof(timeTenthsNs) +
                          sizeof(DOMClockCycles);
    uint32_t scalerArraySize = payloadLength - headerSize;

    log_trace("headerSize = %d", headerSize);
    log_trace("scalerArraySize = %d", scalerArraySize);

    I3VectorUShort scalers;

    // Reading in scalers, one-by-one
    for (uint32_t i = 0 ; i < scalerArraySize; i++)
    {
       char tmpScaler;
       payloadFile.read(&tmpScaler, 1);
       log_trace("tmpScaler = %d", tmpScaler);
       if ( !payloadFile )
       {
           // Broken data array
           log_error("Could not read scaler data (temp). Skipping remainder of file");
           // return;
       }

       if ((tmpScaler > 15 ) || ( tmpScaler < 0 ))
       {
           /// TODO: LOG MESSAGE and message to i3Live/email that something is
           ///       really really wrong


           //       Replace value with 15 or 0
           if ( tmpScaler > 15 )
           {
               log_warn("Scalers > 15!!!!!");
               scalers.push_back(15);
           }
           if ( tmpScaler < 0 )
           {
               log_warn("Scalers less than 0!!!!!");
               scalers.push_back(0);
           }

       }
       else if ((tmpScaler < 15 ) && ( tmpScaler >= 0 ))
       {
           log_trace("Adding to vector");
           scalers.push_back(tmpScaler);
       }
       else
       {
           log_error("Weird scalers quantity. Moving to next file");
       }
    }

    I3SNPayloadPtr payload(new I3SNPayload());

    log_debug("Filling payload");
    payload->SetTime(I3Time(runStartYear_.GetUTCYear(), timeTenthsNs));
    payload->SetScalers(scalers);
    payload->SetOMKey(key);

    if (nPayloads_ == 0 and fileNumber_ == 0)
    {
        runStartTime_ = payload->GetTime();
    }
    nPayloads_++;

    return payload;
}

void I3SNFileReader::RebinPayload(I3SNPayloadPtr payload)
{
    I3VectorUShort scalers = payload->GetScalers();
    I3Time startTime = payload->GetTime();
    OMKey key = payload->GetOMKey();

    boost::shared_ptr<std::deque<uint16_t> > timeSyncBins = tempQueueMap_[key];

    // log_info("size before timeSyncBins = %lu", timeSyncBins->size());
    // log_info("size scalers = %lu", scalers.size());

    // uint32_t absIndex = 0;

    I3Time prevAbsTimeLeftEdge;

    for (uint32_t scalerIndex = 0; scalerIndex < scalers.size(); scalerIndex++)
    {
        I3Time timeRightEdgeScaler = startTime + (scalerIndex * config->GetDataBinSize());
        I3Time timeLeftEdgeScaler = timeRightEdgeScaler + config->GetDataBinSize();

        uint32_t timeSyncBinNumber = floor((timeRightEdgeScaler - runStartTime_) / config->GetTimeSyncBinSize());

        I3Time absTimeRightEdge = runStartTime_ + (timeSyncBinNumber * config->GetTimeSyncBinSize());
        I3Time absTimeLeftEdge = absTimeRightEdge + config->GetTimeSyncBinSize();

        if (scalerIndex == 0) prevAbsTimeLeftEdge = absTimeLeftEdge;

        I3Time leftEdge = std::max(timeRightEdgeScaler, absTimeRightEdge);
        I3Time rightEdge = std::min(absTimeLeftEdge, timeLeftEdgeScaler);

        uint16_t rightSplit = ((rightEdge - leftEdge) / config->GetDataBinSize())*100;
        uint16_t leftSplit = (100 - rightSplit);

        if (prevAbsTimeLeftEdge == absTimeRightEdge) prevAbsTimeLeftEdge = absTimeLeftEdge;

        uint32_t correctedTimeSyncBinNumber = timeSyncBinNumber - (nFrames_ * frameLength_ / config->GetTimeSyncBinSize());

        timeSyncBins->resize(correctedTimeSyncBinNumber + 2);

        (*timeSyncBins)[correctedTimeSyncBinNumber] += rightSplit*scalers[scalerIndex];
        (*timeSyncBins)[correctedTimeSyncBinNumber + 1] += leftSplit*scalers[scalerIndex];
    }

    // log_info("size after timeSyncBins = %lu", timeSyncBins->size());
    DOMsReadIn_.insert(key);
    // log_info("number of DOMs read in = %lu", DOMsReadIn.size());

}

void I3SNFileReader::FillPushOutFrame(I3FramePtr frame)
{
    I3SNDatagramMapPtr out(new I3SNDatagramMap());

    std::set<OMKey>::const_iterator omkey;
    for (omkey = DOMsReadIn_.begin(); omkey != DOMsReadIn_.end(); omkey++)
    {
        I3SNDatagram datagram;
        uint32_t numBinsDump = frameLength_ / config->GetTimeSyncBinSize();
        I3VectorUShort timeSyncBins(numBinsDump);
        log_info("size before timeSyncBins = %lu", (tempQueueMap_[*omkey])->size());
        for (int i = 0; i < numBinsDump; i++)
        {

            timeSyncBins.push_back((tempQueueMap_[*omkey])->front());
            (tempQueueMap_[*omkey])->pop_front();
        }
        log_info("size after timeSyncBins = %lu", (tempQueueMap_[*omkey])->size());
        datagram.SetTimeSyncScalers(timeSyncBins);
        datagram.SetStartTime(runStartTime_ + (nFrames_*frameLength_));
        out->insert(std::make_pair(*omkey, datagram));
    }

    frame->Put("I3SNDatagramMap", out);
    PushFrame(frame);
    nFrames_++;
}

// I3SNFileReader::RebinPayload(I3SNPayloadPtr payload, I3SNDatagramMapPtr tempDatagram)
// {
//
//     I3SNDatagramPtr datagram(new I3SNDatagram());
//     // I3SNDatagramMapPtr out(new I3SNDatagramMap());
//
//     // log_info("Getting data out of payload");
//
//     I3VectorUShort scalers = payload->GetScalers();
//     I3Time startTime = payload->GetTime();
//     OMKey key = payload->GetOMKey();
//
//     log_debug("Calculating new maxlength");
//
//     // uint32_t maxLength = (scalers.size() + 2)*config->dataBinSize / config->timeSyncBinSize;
//
//     log_debug("maxLength = %d", maxLength);
//
//     I3VectorUShort timeSyncBins;
//
//     // timeSyncBins->reserve(maxLength);
//
//     uint32_t absIndex = 0;
//
//     // log_info("size datagram = %lu", timeSyncBins.size());
//
//     // log_info("Rebinning");
//     // log_info("OMKey %s", key.str().c_str());
//
//     // if ( !(DOMStatMap_->find(key) != DOMStatMap_->end()) )
//     // {
//     //     log_info("inserting domstatus");
//     //     DOMStatMap_->insert(std::make_pair(key, I3SNDOMStatus()));
//     // }
//
//     // I3Time prevAbsTimeRightEdge;
//     I3Time prevAbsTimeLeftEdge;
//
//     // Create time-synced datagrams for payload and then proceed to file in map of time to histogram
//     for (uint32_t scalerIndex = 0; scalerIndex < scalers.size(); scalerIndex++)
//     {
//
//         // log_info("startTime = %s", startTime.GetUTCString().c_str());
//         // log_info("startTime = %10" PRId64 " x 0.1 ns", startTime.GetUTCDaqTime());
//         // log_info("scalerindex = %d",scalerIndex );
//         I3Time timeRightEdgeScaler = startTime + (scalerIndex * config->GetDataBinSize());
//         I3Time timeLeftEdgeScaler = timeRightEdgeScaler + config->GetDataBinSize();
//
//         // log_info("timeRightEdgeScaler = %10" PRId64 " x 0.1 ns", timeRightEdgeScaler.GetUTCDaqTime());
//         // log_info("timeLeftEdgeScaler = %10" PRId64 " x 0.1 ns", timeLeftEdgeScaler.GetUTCDaqTime());
//
//         // log_info("runStartTime_ = %s", runStartTime_.GetUTCString().c_str());
//         // log_info("runStartTime_ = %10" PRId64 " x 0.1 ns", runStartTime_.GetUTCDaqTime());
//
//         uint32_t timeSyncBinNumber = floor((timeRightEdgeScaler - runStartTime_) / config->GetTimeSyncBinSize());
//
//         I3Time absTimeRightEdge = runStartTime_ + (timeSyncBinNumber * config->GetTimeSyncBinSize());
//         I3Time absTimeLeftEdge = absTimeRightEdge + config->GetTimeSyncBinSize();
//
//         // log_info("absTimeRightEdge = %10" PRId64 " x 0.1 ns", absTimeRightEdge.GetUTCDaqTime());
//         // log_info("absTimeLeftEdge = %10" PRId64 " x 0.1 ns", absTimeLeftEdge.GetUTCDaqTime());
//
//         if ((absIndex == 0) && (scalerIndex == 0))
//         {
//             datagram->SetStartTime(absTimeRightEdge);
//             // prevAbsTimeRightEdge = absTimeRightEdge;
//             prevAbsTimeLeftEdge = absTimeLeftEdge;
//         }
//
//         I3Time leftEdge = std::max(timeRightEdgeScaler, absTimeRightEdge);
//         I3Time rightEdge = std::min(absTimeLeftEdge, timeLeftEdgeScaler);
//
//         uint16_t rightSplit = ((rightEdge - leftEdge) / config->GetDataBinSize())*100;
//         uint16_t leftSplit = (100 - rightSplit);
//
//         // if (timeSyncBinNumber % 50 == 0 )
//         // {
//         //     log_info("scalerIndex = %d", scalerIndex);
//         //     log_info("timeSyncBinNumber = %d", timeSyncBinNumber);
//         //     log_info("timeRightEdgeScaler = %10" PRId64 " x 0.1 ns", timeRightEdgeScaler.GetUTCDaqTime());
//         //     log_info("timeLeftEdgeScaler = %10" PRId64 " x 0.1 ns", timeLeftEdgeScaler.GetUTCDaqTime());
//         //     log_info("absTimeRightEdge = %10" PRId64 " x 0.1 ns", absTimeRightEdge.GetUTCDaqTime());
//         //     log_info("absTimeLeftEdge = %10" PRId64 " x 0.1 ns", absTimeLeftEdge.GetUTCDaqTime());
//         //     log_info("rightSplit = %d", rightSplit);
//         //     log_info("leftSplit = %d", leftSplit);
//         //     log_info("---------------------");
//         // }
//
//         // if ((DOMStatMap_->find(key) != DOMStatMap_->end()))
//         // {
//         //     absIndex = timeSyncBinNumber - (*DOMStatMap_)[key].GetLastAnalysisBinFilled();
//         // }
//         // else
//         // {
//         //     absIndex = timeSyncBinNumber;
//         // }
//
//         // if (scalerIndex == 0)
//         //     eventheader->SetStartTime(absTimeRightEdge + (config->GetTimeSyncBinSize()*));
//
//         // absIndex = timeSyncBinNumber;
//
//         // if (absIndex % 5) log_info("absIndex = %d", absIndex);
//
//         // log_info("rightSplit = %d", rightSplit*scalers[scalerIndex]);
//         // log_info("leftSplit = %d", leftSplit*scalers[scalerIndex]);
//
//         // log_info("1 absIndex = %d", absIndex);
//         // log_info("1 timeSyncBins->size() = %d", timeSyncBins.size());
//
//         if (prevAbsTimeLeftEdge == absTimeRightEdge)
//         {
//             absIndex++;
//             // prevAbsTimeRightEdge = absTimeRightEdge;
//             prevAbsTimeLeftEdge = absTimeLeftEdge;
//         }
//
//         if (absIndex + 2 < timeSyncBins.size())
//             log_fatal("BAD!");
//
//         timeSyncBins.resize(absIndex + 2);
//
//         // log_info("2 absIndex = %d", absIndex);
//         // log_info("2 timeSyncBins->size() = %d", timeSyncBins.size());
//         // log_info("scalers[scalerIndex] = %d", scalers[scalerIndex]);
//         //
//         timeSyncBins[absIndex] += rightSplit*scalers[scalerIndex];
//         timeSyncBins[absIndex + 1] += leftSplit*scalers[scalerIndex];
//
//     }
//
//     // log_info("Done Rebinning");
//
//     // (*DOMStatMap_)[key].SetLastAnalysisBinFilled((*DOMStatMap_)[key].GetLastAnalysisBinFilled() + timeSyncBins.size());
//
//     // log_info("(*DOMStatMap_)[key].lastAnalysisBinFilled = %d", (*DOMStatMap_)[key].GetLastAnalysisBinFilled());
//
//     datagram->SetTimeSyncScalers(timeSyncBins);
//
//     // log_info("datagram->GetTime().GetUTCYear() = %d", datagram->GetStartTime().GetUTCYear());
//
//     // log_info("datagram->GetTimeSyncScalers().size() = %d", datagram->GetTimeSyncScalers().size());
//
//     out->insert(std::make_pair(key, *datagram));
//
//     // log_info("returning.....");
//
//     return out;
//
// }
