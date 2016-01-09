#ifndef I3SNDATACLASSES_H_INCLUDED
#define I3SNDATACLASSES_H_INCLUDED

#include <icetray/serialization.h>
#include <icetray/I3FrameObject.h>
#include <icetray/OMKey.h>
#include <icetray/I3Bool.h>


#include <dataclasses/I3Time.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/I3Vector.h>

#include <vector>


/*
 * Frame object for internal checking that a DOM
 * should be used in the analysis
 */
class I3SNDOMStatus: public I3FrameObject
{

public:

    I3SNDOMStatus()
    {
        expectedStartTime = I3Time(0,0);
        jitterCount = 0;
        expectedDOMClock = 0;
        isDead = false;
        // lastAnalysisBinFilled = 0;
    };

    /*
     * Getters
     */
    I3Time GetExpectedStartTime() const { return expectedStartTime; };
    uint32_t GetJitterCount() const { return jitterCount; };
    uint64_t GetExpectedDOMClock() const { return expectedDOMClock; };
    I3Bool GetIsDead() const { return isDead; };
    // uint32_t GetLastAnalysisBinFilled() { return lastAnalysisBinFilled; };

    /*
     * Setters
     */
    void SetExpectedStartTime(I3Time newExpectedStartTime) { expectedStartTime = newExpectedStartTime; };
    void SetJitterCount(uint32_t newJitterCount) { jitterCount = newJitterCount; };
    void SetExpectedDOMClock(uint64_t newExpectedDOMClock) { expectedDOMClock = newExpectedDOMClock; };
    void SetIsDead(I3Bool newIsDead) { isDead = newIsDead; };
    // void SetLastAnalysisBinFilled(uint32_t newLastAnalysisBinFilled) { lastAnalysisBinFilled = newLastAnalysisBinFilled; };

private:

    // Things needed to make sure that data is consistant
    I3Time expectedStartTime;
    // Check if DOM is Jittering
    uint32_t jitterCount;
    I3Bool isDead;
    uint64_t expectedDOMClock;
    // uint32_t lastAnalysisBinFilled;


    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, unsigned version);

};

std::ostream& operator<<(std::ostream& oss, const I3SNDOMStatus& d);

typedef I3Map<OMKey, I3SNDOMStatus > I3SNDOMStatusMap;

I3_POINTER_TYPEDEFS(I3SNDOMStatus);
I3_POINTER_TYPEDEFS(I3SNDOMStatusMap);

class I3SNConfig : public I3FrameObject
{

public:

    I3SNConfig(){};

    /*
     * Getters
     */
    uint64_t GetDataBinSize() const { return dataBinSize; };
    uint32_t GetClockCyclesPerScalers() const { return clockCyclesPerScalers; };
    uint64_t GetTimeSyncBinSize() const { return timeSyncBinSize; };
    I3VectorUInt64 GetAnalysisBins() const { return analysisBinSizes; };

    /*
     * Setters
     */
    void SetDataBinSize(uint64_t newDataBinSize) { dataBinSize = newDataBinSize; };
    void SetClockCyclesPerScalers(uint32_t newClockCyclesPerScalers) { clockCyclesPerScalers = newClockCyclesPerScalers; };
    void SetTimeSyncBinSize(uint64_t newTimeSyncBinSize) { timeSyncBinSize = newTimeSyncBinSize; };
    void SetAnalysisBins(I3VectorUInt64 newAnalysisBins) { analysisBinSizes = newAnalysisBins; };

private:

    uint64_t dataBinSize;
    uint32_t clockCyclesPerScalers;
    uint64_t timeSyncBinSize;
    I3VectorUInt64 analysisBinSizes;

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, unsigned version);
};

I3_POINTER_TYPEDEFS(I3SNConfig);

class I3SNRunStatus : public I3FrameObject
{

public:

    I3SNRunStatus(){};

   /*
    * Getters
    */
   uint64_t GetRunNumber() const { return runNumber; };
   I3Time GetStartTime() const { return startTime; };

   /*
    * Setters
    */
   void GetRunNumber(uint32_t newRunNumber) { runNumber = newRunNumber; };
   void GetStartTime(I3Time newStartTime) { startTime = newStartTime; };

private:

    uint32_t runNumber;
    I3Time startTime;

    friend class boost::serialization::access;

    template <class Archive> void serialize(Archive &ar, unsigned version);

};

class I3SNPayload : public I3FrameObject
{

public:

    I3SNPayload(){};

    /*
     * Getters
     */
    // uint32_t GetPayloadLength() const { return payloadLength };
    // uint32_t GetPayloadType() const { return payloadType };
    // int64_t GetDOMHexID() const { return DOMHexID };
    // int16_t GetScalerBlockLength() const { return scalerBlockLength};
    // int16_t GetFormatID() const {return formatID};
    // uint64_t GetDOMClockCycles() const {return DOMClockCycles};
    I3Time GetTime() const { return time; };
    I3VectorUShort GetScalers() const {return scalers;};
    OMKey GetOMKey() const {return key;};

    /*
     * Setters
     */
    void SetTime(I3Time newTime) { time = newTime; };
    void SetScalers(I3VectorUShort newScalers) { scalers = newScalers; };
    void SetOMKey(OMKey newKey) { key = newKey; };

private:

    /// For details see
    /// https://wiki.icecube.wisc.edu/index.php/Payloads#Supernova_Payloads_.28Type_16.29

    // pDAQ Header
    // uint32_t payloadLength;       // 4 byte, length of the overall payload
    // uint32_t payloadType;         // 4 byte, pDAQ magic number for SN payloads = 16
    // uint64_t time; // 8 byte, time in 0.1 ns since the beginning of the year
    I3Time time;      // 8 byte, time in 0.1 ns since the beginning of the year

    // DOM header
    // int64_t DOMHexID;          // 8 byte, hex id of DOM mainboard
    // int16_t scalerBlockLength; // 2 byte, length of supernova record
    // int16_t formatID;           // 2 byte, pDAQ magic number for SN payloads = 300
    // uint64_t DOMClockCycles;     // 6 byte, 6 least signifcant bits of DOM clock count (1 count = 25 ns)

    // Payload data
    I3VectorUShort scalers;  // bins byte, contain sn scaler counts
                                 // inside the DOM its type is unsigned half byte
                                 // The scaler only counts up to 15, so it fits into a char
                                 // There might be bit flips that cause the scaler > 15, < 0

    // Add OMKey now to so we dont have calculate again later
    OMKey key;

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, unsigned version);
};

typedef I3Map<OMKey, I3SNPayload > I3SNPayloadMap;
I3_POINTER_TYPEDEFS(I3SNPayload);
I3_POINTER_TYPEDEFS(I3SNPayloadMap);

class I3SNDatagram : public I3FrameObject
{

public:


    I3SNDatagram(){};
    I3SNDatagram(const uint32_t & length): timeSyncScalers(length) {};

    /*
     * Getters
     */
    I3Time GetStartTime() const { return startTimeDatagram; };
    I3VectorUShort GetTimeSyncScalers() const { return timeSyncScalers; };

    /*
     * Setters
     */
    void SetStartTime(I3Time newtime){ startTimeDatagram = newtime; };
    void SetTimeSyncScalers(I3VectorUShort newscalers) { timeSyncScalers = newscalers; };


private:

    I3VectorUShort timeSyncScalers;
    I3Time startTimeDatagram;

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, unsigned version);

};

typedef I3Map<OMKey, I3SNDatagram > I3SNDatagramMap;
// typedef I3Map<OMKey, I3VectorUShort > I3SNDatagramMap;
// I3_POINTER_TYPEDEFS(I3VectorUShort);
I3_POINTER_TYPEDEFS(I3SNDatagram);
I3_POINTER_TYPEDEFS(I3SNDatagramMap);

class I3SNDAQRecord : public I3FrameObject
{

public:

    I3SNDAQRecord(){};

    /*
     * Getters
     */
    uint32_t GetPayloadLength() const { return payloadLength; };
    uint32_t GetPayloadType() const { return payloadType; };
    uint64_t GetDOMHexID() const { return DOMHexID; };
    uint16_t GetScalerBlockLength() const { return scalerBlockLength; };
    uint16_t GetFormatID() const { return formatID; };
    uint64_t GetDOMClockCycles() const { return DOMClockCycles; };
    uint64_t GetTime() const { return time; };
    std::vector<uint16_t> GetScalers() const { return scalers; };

    /*
     * Setters
     */
    void SetPayloadLength( uint32_t newPayloadLength ) { newPayloadLength = payloadLength; };
    void SetPayloadType( uint32_t newPayloadType ) { newPayloadType = payloadType; };
    void SetDOMHexID( uint64_t newDOMHexID ) { newDOMHexID = DOMHexID; };
    void SetScalerBlockLength( uint16_t newScalerBlockLength ) { newScalerBlockLength = scalerBlockLength; };
    void SetFormatID( uint16_t newFormatID ) { newFormatID = formatID; };
    void SetDOMClockCycles( uint64_t newDOMClockCycles ) { newDOMClockCycles = DOMClockCycles; };
    void SetScalers( std::vector<uint16_t> newScalers ) { newScalers = scalers; };

private:

    /// For details see
    /// https://wiki.icecube.wisc.edu/index.php/Payloads#Supernova_Payloads_.28Type_16.29

    // pDAQ Header
    uint32_t payloadLength;       // 4 byte, length of the overall payload
    uint32_t payloadType;         // 4 byte, pDAQ magic number for SN payloads = 16
    uint64_t time; // 8 byte, time in 0.1 ns since the beginning of the year
    // I3Time time;      // 8 byte, time in 0.1 ns since the beginning of the year

    // DOM header
    int64_t DOMHexID;          // 8 byte, hex id of DOM mainboard
    int16_t scalerBlockLength; // 2 byte, length of supernova record
    int16_t formatID;           // 2 byte, pDAQ magic number for SN payloads = 300
    uint64_t DOMClockCycles;     // 6 byte, 6 least signifcant bits of DOM clock count (1 count = 25 ns)

    // Payload data
    std::vector<uint16_t> scalers;  // bins byte, contain sn scaler counts
                                 // inside the DOM its type is unsigned half byte
                                 // The scaler only counts up to 15, so it fits into a char
                                 // There might be bit flips that cause the scaler > 15, < 0

    // Add OMKey now to so we dont have calculate again later
    // OMKey key;

    friend class boost::serialization::access;
    template <class Archive> void serialize(Archive &ar, unsigned version);
};

typedef I3Map<OMKey, I3SNDAQRecord > I3SNDAQRecordMap;
I3_POINTER_TYPEDEFS(I3SNDAQRecord);
I3_POINTER_TYPEDEFS(I3SNDAQRecordMap);


// class I3SNQueue
// {
//
// public:
//
// private:
//
//
// };
//
// typedef I3Map<OMKey, I3SNQueue > I3SNQueueMap;
//
//
// I3_POINTER_TYPEDEFS(I3SNQueue);
// I3_POINTER_TYPEDEFS(I3SNQueueMap);

#endif //I3SNDATACLASSES_H_INCLUDED
