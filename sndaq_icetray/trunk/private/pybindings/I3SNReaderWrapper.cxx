#ifndef I3SNREADERWRAPPER_H_INCLUDED
#define I3SNREADERWRAPPER_H_INCLUDED

namespace
{

    I3SNDatagramMapPtr RebinPayload(I3SNPayloadPtr payload,
                                    I3SNConfigPtr config,
                                    I3SNRunStatus status
                                    )
    {

    I3SNDatagramPtr datagram(new I3SNDatagram());
    I3SNDatagramMapPtr out(new I3SNDatagramMap());

    // log_info("Getting data out of payload");

    I3VectorUShort scalers = payload->GetScalers();
    I3Time startTime = payload->GetTime();
    OMKey key = payload->GetOMKey();

    log_debug("Calculating new maxlength");

    // uint32_t maxLength = (scalers.size() + 2)*config->dataBinSize / config->timeSyncBinSize;

    log_debug("maxLength = %d", maxLength);

    I3VectorUShort timeSyncBins;

    // timeSyncBins->reserve(maxLength);

    uint32_t absIndex = 0;

    // log_info("size datagram = %lu", timeSyncBins.size());

    // log_info("Rebinning");
    // log_info("OMKey %s", key.str().c_str());

    // if ( !(DOMStatMap_->find(key) != DOMStatMap_->end()) )
    // {
    //     log_info("inserting domstatus");
    //     DOMStatMap_->insert(std::make_pair(key, I3SNDOMStatus()));
    // }

    // I3Time prevAbsTimeRightEdge;
    I3Time prevAbsTimeLeftEdge;

    // Create time-synced datagrams for payload and then proceed to file in map of time to histogram
    for (uint32_t scalerIndex = 0; scalerIndex < scalers.size(); scalerIndex++)
    {

        // log_info("startTime = %s", startTime.GetUTCString().c_str());
        // log_info("startTime = %10" PRId64 " x 0.1 ns", startTime.GetUTCDaqTime());
        // log_info("scalerindex = %d",scalerIndex );
        I3Time timeRightEdgeScaler = startTime + (scalerIndex * config->GetDataBinSize());
        I3Time timeLeftEdgeScaler = timeRightEdgeScaler + config->GetDataBinSize();

        // log_info("timeRightEdgeScaler = %10" PRId64 " x 0.1 ns", timeRightEdgeScaler.GetUTCDaqTime());
        // log_info("timeLeftEdgeScaler = %10" PRId64 " x 0.1 ns", timeLeftEdgeScaler.GetUTCDaqTime());

        // log_info("runStartTime_ = %s", runStartTime_.GetUTCString().c_str());
        // log_info("runStartTime_ = %10" PRId64 " x 0.1 ns", runStartTime_.GetUTCDaqTime());
    
        uint32_t timeSyncBinNumber = floor((timeRightEdgeScaler - runStartTime_) / config->GetTimeSyncBinSize());

        I3Time absTimeRightEdge = status->GetStartTime() + (timeSyncBinNumber * config->GetTimeSyncBinSize());
        I3Time absTimeLeftEdge = absTimeRightEdge + config->GetTimeSyncBinSize();
    
        // log_info("absTimeRightEdge = %10" PRId64 " x 0.1 ns", absTimeRightEdge.GetUTCDaqTime());
        // log_info("absTimeLeftEdge = %10" PRId64 " x 0.1 ns", absTimeLeftEdge.GetUTCDaqTime());
    
        if ((absIndex == 0) && (scalerIndex == 0))
        {
            datagram->SetStartTime(absTimeRightEdge);
            // prevAbsTimeRightEdge = absTimeRightEdge;
            prevAbsTimeLeftEdge = absTimeLeftEdge;
        }
     
        I3Time leftEdge = std::max(timeRightEdgeScaler, absTimeRightEdge);
        I3Time rightEdge = std::min(absTimeLeftEdge, timeLeftEdgeScaler);

        uint16_t rightSplit = ((rightEdge - leftEdge) / config->GetDataBinSize())*100;
        uint16_t leftSplit = (100 - rightSplit);
    
        // if (timeSyncBinNumber % 50 == 0 )
        // {
        //     log_info("scalerIndex = %d", scalerIndex);
        //     log_info("timeSyncBinNumber = %d", timeSyncBinNumber);
        //     log_info("timeRightEdgeScaler = %10" PRId64 " x 0.1 ns", timeRightEdgeScaler.GetUTCDaqTime());
        //     log_info("timeLeftEdgeScaler = %10" PRId64 " x 0.1 ns", timeLeftEdgeScaler.GetUTCDaqTime());
        //     log_info("absTimeRightEdge = %10" PRId64 " x 0.1 ns", absTimeRightEdge.GetUTCDaqTime());
        //     log_info("absTimeLeftEdge = %10" PRId64 " x 0.1 ns", absTimeLeftEdge.GetUTCDaqTime());
        //     log_info("rightSplit = %d", rightSplit);
        //     log_info("leftSplit = %d", leftSplit);
        //     log_info("---------------------");
        // }

        // if ((DOMStatMap_->find(key) != DOMStatMap_->end()))
        // {
        //     absIndex = timeSyncBinNumber - (*DOMStatMap_)[key].GetLastAnalysisBinFilled();
        // }
        // else
        // {
        //     absIndex = timeSyncBinNumber;
        // }
    
        // if (scalerIndex == 0)
        //     eventheader->SetStartTime(absTimeRightEdge + (config->GetTimeSyncBinSize()*));
    
        // absIndex = timeSyncBinNumber;

        // if (absIndex % 5) log_info("absIndex = %d", absIndex);
    
        // log_info("rightSplit = %d", rightSplit*scalers[scalerIndex]);
        // log_info("leftSplit = %d", leftSplit*scalers[scalerIndex]);

        // log_info("1 absIndex = %d", absIndex);
        // log_info("1 timeSyncBins->size() = %d", timeSyncBins.size());
    
        if (prevAbsTimeLeftEdge == absTimeRightEdge)
        {
            absIndex++;
            // prevAbsTimeRightEdge = absTimeRightEdge;
            prevAbsTimeLeftEdge = absTimeLeftEdge;
        }
    
        if (absIndex + 2 < timeSyncBins.size())
            log_fatal("BAD!");
    
        timeSyncBins.resize(absIndex + 2);

        // log_info("2 absIndex = %d", absIndex);
        // log_info("2 timeSyncBins->size() = %d", timeSyncBins.size());
        // log_info("scalers[scalerIndex] = %d", scalers[scalerIndex]);
        //
        timeSyncBins[absIndex] += rightSplit*scalers[scalerIndex];
        timeSyncBins[absIndex + 1] += leftSplit*scalers[scalerIndex];

    }


    // I3SNPayloadPtr I3SNFileReader::ReadPayload(std::ifstream& payloadFile)
    // {
    //
    //     // log_info("Read payload");
    //
    //     if (!payloadFile.good()) log_fatal("payload file has ended");
    //     int32_t payloadLength(0), payloadType(0);
    //     int16_t scalerBlockLength(0), formatID(0);
    //     int64_t timeTenthsNs(0), DOMHexID(0);
    //     char DOMClockCycles[6];
    //
    //     /*
    //      *  Reading in payload. The order is important because are reading in a
    //      *  binary file.
    //      */
    //
    //     /*
    //      * Reading pDAQ header
    //      */
    //
    //     // Reading length of binary file package
    //     payloadFile.read((char*)&payloadLength, 4);
    //     if (!payloadFile)
    //     {
    //        log_error("Could not read length of binary file package (payloadLength). Skipping remainder of file");
    //        // TODO: Deal with file... Rename, etc.
    //        // return;
    //     }
    //     // Reading payload type
    //     payloadFile.read((char*)&payloadType, 4);
    //     if (!payloadFile)
    //     {
    //        log_error("Could not read payload type (payloadType). Skipping remainder of file");
    //        // return;
    //     }
    //     // Reading payload time in 0.1 ns
    //     payloadFile.read((char*)&timeTenthsNs, 8);
    //     if (!payloadFile)
    //     {
    //        log_error("Could not read time in 0.1 ns (timeTenthsNs). Skipping remainder of file");
    //        // return;
    //     }
    //
    //
    //     /*
    //      * Reading DOM payload
    //      */
    //
    //     // Reading DOM Hex ID
    //     payloadFile.read((char*)&DOMHexID, 8);
    //     if (!payloadFile)
    //     {
    //        log_error("Could not read DOM mainboard hex id (DOMHexID). Skipping remainder of file");
    //        // return;
    //     }
    //     // Reading lenght of DOM payload
    //     payloadFile.read((char*)&scalerBlockLength, 2);
    //     if (!payloadFile)
    //     {
    //        log_error("Could not read length of scaler data (scalerBlockLength). Skipping remainder of file");
    //        // return;
    //     }
    //     // Reading format ID
    //     payloadFile.read((char*)&formatID, 2);
    //     if (!payloadFile)
    //     {
    //        log_error("Could not read format id (formatID). Skipping remainder of file");
    //        // return;
    //     }
    //     // Reading Number of DOM clock cycles
    //     payloadFile.read(DOMClockCycles, 6);
    //     if (!payloadFile)
    //     {
    //        log_error("Could not read DOM mainboard hex id (DOMHexID). Skipping remainder of file");
    //        // return;
    //     }
    //
    //     // Convert from big endian of ARM used on DOM
    //     // to little endian of X86 on the server
    //     I3SNHelpers::ByteSwap5(payloadLength);
    //     I3SNHelpers::ByteSwap5(payloadType);
    //     I3SNHelpers::ByteSwap5(scalerBlockLength);
    //     I3SNHelpers::ByteSwap5(formatID);
    //     I3SNHelpers::ByteSwap5(timeTenthsNs);
    //     I3SNHelpers::ByteSwap5(DOMHexID);
    //
    //     int64_t numDOMClockCycles = I3SNHelpers::ByteSwapDOMClock(DOMClockCycles);
    //
    //     log_debug("payloadLength = %d", payloadLength );
    //     log_debug("payloadType = %d", payloadType );
    //     log_debug("scalerBlockLength = %d", scalerBlockLength );
    //     log_debug("formatID = %d", formatID );
    //     log_debug("time in = %10" PRId64 " x 0.1 ns", timeTenthsNs);
    //
    //     if ( (payloadType != 16) || (formatID != 300) )
    //     {
    //        log_error("Wrong Payload type or Format id!");
    //        // return;
    //     }
    //
    //     OMKey key = omkey2mbid->GetOMKey(DOMHexID);
    //
    //     /// TODO: CHECK PAYLOADS... Jitter, etc.
    //
    //     /*
    //     * Reading scalers
    //     */
    //     // Estimating how long the scaler package is by removing the size of the header from
    //     // the payload length.
    //     uint32_t headerSize = sizeof(payloadLength) + sizeof(payloadType) + sizeof(scalerBlockLength) +
    //                           sizeof(formatID) + sizeof(DOMHexID) + sizeof(timeTenthsNs) +
    //                           sizeof(DOMClockCycles);
    //     uint32_t scalerArraySize = payloadLength - headerSize;
    //
    //     log_trace("headerSize = %d", headerSize);
    //     log_trace("scalerArraySize = %d", scalerArraySize);
    //
    //     I3VectorUShort scalers;
    //
    //     // Reading in scalers, one-by-one
    //     for (uint32_t i = 0 ; i < scalerArraySize; i++)
    //     {
    //        char tmpScaler;
    //        payloadFile.read(&tmpScaler, 1);
    //        log_trace("tmpScaler = %d", tmpScaler);
    //        if ( !payloadFile )
    //        {
    //            // Broken data array
    //            log_error("Could not read scaler data (temp). Skipping remainder of file");
    //            // return;
    //        }
    //
    //        if ((tmpScaler > 15 ) || ( tmpScaler < 0 ))
    //        {
    //            /// TODO: LOG MESSAGE and message to i3Live/email that something is
    //            ///       really really wrong
    //
    //
    //            //       Replace value with 15 or 0
    //            if ( tmpScaler > 15 )
    //            {
    //                log_warn("Scalers > 15!!!!!");
    //                scalers.push_back(15);
    //            }
    //            if ( tmpScaler < 0 )
    //            {
    //                log_warn("Scalers less than 0!!!!!");
    //                scalers.push_back(0);
    //            }
    //
    //        }
    //        else if ((tmpScaler < 15 ) && ( tmpScaler >= 0 ))
    //        {
    //            log_trace("Adding to vector");
    //            scalers.push_back(tmpScaler);
    //        }
    //        else
    //        {
    //            log_error("Weird scalers quantity. Moving to next file");
    //        }
    //     }
    //
    //     I3SNPayloadPtr payload(new I3SNPayload());
    //
    //     log_debug("Filling payload");
    //     payload->SetTime(I3Time(runStartYear_.GetUTCYear(), timeTenthsNs));
    //     payload->SetScalers(scalers);
    //     payload->SetOMKey(key);
    //
    //     if (nPayloads_ == 0 and fileNumber_ == 0)
    //     {
    //         runStartTime_ = payload->GetTime();
    //     }
    //     nPayloads_++;
    //
    //     return payload;
    // }

}


#endif //I3SNFILEREADER_H_INCLUDED
