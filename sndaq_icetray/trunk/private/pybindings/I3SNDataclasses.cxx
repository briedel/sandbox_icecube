#include <icetray/python/dataclass_suite.hpp>
#include <sndaq/I3SNDataclasses.h>
#include <dataclasses/ostream_overloads.hpp>
#include <icetray/python/dataclass_suite.hpp>
using namespace boost::python;

// void register_I3SNPayload()
// {
//     class_<I3SNPayload, bases<I3FrameObject>, boost::shared_ptr<I3SNPayload> > payload =
//       class_<I3SNPayload, bases<I3FrameObject>, boost::shared_ptr<I3SNPayload> >("I3SNPayload")
//
//     .add_property("time", make_function( (I3Time (I3SNPayload::*)()) &I3SNPayload::GetTime ),
//                  (void (I3SNPayload::*)(const I3Time&)) &I3SNPayload::SetTime )
//     .add_property("scalers", make_function( (I3VectorUShort (I3SNPayload::*)()) &I3SNPayload::GetScalers ),
//                  (void (I3SNPayload::*)(const I3VectorUShort&)) &I3SNPayload::SetScalers )
//     .add_property("omkey", make_function( (OMKey (I3SNPayload::*)()) &I3SNPayload::GetOMKey ),
//                  (void (I3SNPayload::*)(const OMKey&)) &I3SNPayload::SetOMKey );
// }

void register_I3SNPayload()
{
    class_<I3SNPayload, bases<I3FrameObject>, boost::shared_ptr<I3SNPayload> > payload =
      class_<I3SNPayload, bases<I3FrameObject>, boost::shared_ptr<I3SNPayload> >("I3SNPayload")

    .add_property("time", &I3SNPayload::GetTime, &I3SNPayload::SetTime )
    .add_property("scalers", &I3SNPayload::GetScalers, &I3SNPayload::SetScalers )
    .add_property("omkey", &I3SNPayload::GetOMKey, &I3SNPayload::SetOMKey );
}

// void register_I3SNDatagram()
// {
//     class_<I3SNDatagram, bases<I3FrameObject>, boost::shared_ptr<I3SNDatagram> > datagram =
//       class_<I3SNDatagram, bases<I3FrameObject>, boost::shared_ptr<I3SNDatagram> >("I3SNDatagram")
//
//     .add_property("time", make_function( (I3Time (I3SNDatagram::*)()) &I3SNDatagram::GetStartTime ),
//                  (void (I3SNDatagram::*)(const I3Time&)) &I3SNDatagram::SetStartTime )
//     .add_property("timesyncedscalers", make_function( (I3VectorUShort (I3SNDatagram::*)()) &I3SNDatagram::GetTimeSyncScalers ),
//                  (void (I3SNDatagram::*)(const I3VectorUShort&)) &I3SNDatagram::SetTimeSyncScalers );
// }

void register_I3SNDatagram()
{
    class_<I3SNDatagram, bases<I3FrameObject>, boost::shared_ptr<I3SNDatagram> > datagram =
      class_<I3SNDatagram, bases<I3FrameObject>, boost::shared_ptr<I3SNDatagram> >("I3SNDatagram")

    .add_property("time", &I3SNDatagram::GetStartTime, &I3SNDatagram::SetStartTime )
    .add_property("timesyncedscalers", &I3SNDatagram::GetTimeSyncScalers, &I3SNDatagram::SetTimeSyncScalers );
}

// void register_I3SNDOMStatus()
// {
//     class_<I3SNDOMStatus, bases<I3FrameObject>, boost::shared_ptr<I3SNDOMStatus> > domstatus =
//       class_<I3SNDOMStatus, bases<I3FrameObject>, boost::shared_ptr<I3SNDOMStatus> >("I3SNDOMStatus")
//
//     .add_property("expectedstarttime", make_function( (I3Time (I3SNDOMStatus::*)()) &I3SNDOMStatus::GetExpectedStartTime ),
//                  (void (I3SNDOMStatus::*)(const I3Time&)) &I3SNDOMStatus::SetExpectedStartTime )
//     .add_property("jittercount", make_function( (uint32_t (I3SNDOMStatus::*)()) &I3SNDOMStatus::GetJitterCount ),
//                  (void (I3SNDOMStatus::*)(const uint32_t&)) &I3SNDOMStatus::SetJitterCount)
//     .add_property("dead", make_function( (I3Bool (I3SNDOMStatus::*)()) &I3SNDOMStatus::GetIsDead ),
//                  (void (I3SNDOMStatus::*)(const I3Bool&)) &I3SNDOMStatus::SetIsDead)
//     .add_property("expectedDOMclock", make_function( (uint64_t (I3SNDOMStatus::*)()) &I3SNDOMStatus::GetExpectedDOMClock ),
//                  (void (I3SNDOMStatus::*)(const uint64_t&)) &I3SNDOMStatus::SetExpectedDOMClock );
//
// }

void register_I3SNDOMStatus()
{
   class_<I3SNDOMStatus, bases<I3FrameObject>, boost::shared_ptr<I3SNDOMStatus> > domstatus =
     class_<I3SNDOMStatus, bases<I3FrameObject>, boost::shared_ptr<I3SNDOMStatus> >("I3SNDOMStatus")

   .add_property("expectedstarttime", &I3SNDOMStatus::GetExpectedStartTime, &I3SNDOMStatus::SetExpectedStartTime )
   .add_property("jittercount", &I3SNDOMStatus::GetJitterCount, &I3SNDOMStatus::SetJitterCount)
   .add_property("dead",  &I3SNDOMStatus::GetIsDead, &I3SNDOMStatus::SetIsDead)
   .add_property("expectedDOMclock", &I3SNDOMStatus::GetExpectedDOMClock, &I3SNDOMStatus::SetExpectedDOMClock );

}

// void register_I3SNConfig()
// {
//     class_<I3SNConfig, bases<I3FrameObject>, boost::shared_ptr<I3SNConfig> > snconfig =
//       class_<I3SNConfig, bases<I3FrameObject>, boost::shared_ptr<I3SNConfig> >("I3SNConfig")
//
//     .add_property("databinsize", make_function( (uint64_t (I3SNConfig::*)()) &I3SNConfig::GetDataBinSize ),
//                  (void (I3SNConfig::*)(uint64_t)) &I3SNConfig::SetDataBinSize )
//     .add_property("timesyncbinsize", make_function( (uint64_t (I3SNConfig::*)()) &I3SNConfig::GetTimeSyncBinSize ),
//                  (void (I3SNConfig::*)(uint64_t)) &I3SNConfig::SetTimeSyncBinSize)
//     .add_property("clockcyclesperscalers", make_function( (uint32_t (I3SNConfig::*)()) &I3SNConfig::GetClockCyclesPerScalers ),
//                  (void (I3SNConfig::*)(uint32_t)) &I3SNConfig::SetClockCyclesPerScalers)
//     .add_property("analysisbins", make_function( (I3VectorUInt64 (I3SNConfig::*)()) &I3SNConfig::GetAnalysisBins ),
//                  (void (I3SNConfig::*)(const I3VectorUInt64&)) &I3SNConfig::SetAnalysisBins);
// }

void register_I3SNConfig()
{
    class_<I3SNConfig, bases<I3FrameObject>, boost::shared_ptr<I3SNConfig> > snconfig =
      class_<I3SNConfig, bases<I3FrameObject>, boost::shared_ptr<I3SNConfig> >("I3SNConfig")

    .add_property("databinsize", &I3SNConfig::GetDataBinSize, &I3SNConfig::SetDataBinSize )
    .add_property("timesyncbinsize", &I3SNConfig::GetTimeSyncBinSize, &I3SNConfig::SetTimeSyncBinSize)
    .add_property("clockcyclesperscalers", &I3SNConfig::GetClockCyclesPerScalers, &I3SNConfig::SetClockCyclesPerScalers)
    .add_property("analysisbins", &I3SNConfig::GetAnalysisBins, &I3SNConfig::SetAnalysisBins);
}
