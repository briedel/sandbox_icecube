#include <icetray/serialization.h>

#include <sndaq/I3SNDataclasses.h>

template <class Archive>
void I3SNPayload::serialize (Archive &ar, unsigned version)
{
    ar &make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar &make_nvp("Time", time);
    ar &make_nvp("Scalers", scalers);
    ar &make_nvp("OMKey", key);
}
I3_SERIALIZABLE(I3SNPayload);
I3_SERIALIZABLE(I3SNPayloadMap);

template <class Archive>
void I3SNConfig::serialize (Archive &ar, unsigned version)
{
    ar &make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar &make_nvp("DataBinSize", dataBinSize);
    ar &make_nvp("ClockCyclesPerScalers", clockCyclesPerScalers);
    ar &make_nvp("TimeSyncBinSize", timeSyncBinSize);
    ar &make_nvp("AnalysisBinSizes", analysisBinSizes);
}
I3_SERIALIZABLE(I3SNConfig);

template <class Archive>
void I3SNDOMStatus::serialize (Archive &ar, unsigned version)
{
    ar &make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar &make_nvp("ExpectedStartTime", expectedStartTime);
    ar &make_nvp("JitterCount", jitterCount);
    ar &make_nvp("IsDead", isDead);
    ar &make_nvp("ExpectedDOMClock", expectedDOMClock);
    // ar &make_nvp("LastAnalysisBinFilled", lastAnalysisBinFilled);
}
I3_SERIALIZABLE(I3SNDOMStatus);
I3_SERIALIZABLE(I3SNDOMStatusMap);

std::ostream& operator<<(std::ostream& oss, const I3SNDOMStatus& DOMStatus)
{
    oss << "[ I3SNDOMStatus ExpectedStartTime : " << DOMStatus.GetExpectedStartTime() << std::endl
        << "                      JitterCount : " << DOMStatus.GetJitterCount() << std::endl
        << "                 ExpectedDOMClock : " << DOMStatus.GetExpectedDOMClock()  << std::endl
        << "]";
    return oss;
}

template <class Archive>
void I3SNDatagram::serialize (Archive &ar, unsigned version)
{
    ar &make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar &make_nvp("TimeSyncScalers", timeSyncScalers);
    ar &make_nvp("StartTimeDatagram", startTimeDatagram);
}
I3_SERIALIZABLE(I3SNDatagram);
I3_SERIALIZABLE(I3SNDatagramMap);

template <class Archive>
void I3SNRunStatus::serialize (Archive &ar, unsigned version)
{
    ar &make_nvp("RunNumber", runNumber);
    ar &make_nvp("StartTime", startTime);
}
I3_SERIALIZABLE(I3SNRunStatus);
