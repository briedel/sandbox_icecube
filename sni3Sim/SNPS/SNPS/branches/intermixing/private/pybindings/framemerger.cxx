#include <icetray/python/list_indexing_suite.hpp>
#include <icetray/python/stream_to_string.hpp>

#include <dataclasses/ostream_overloads.hpp>

#include <SNPS/intermixing/framemerger.h>

using namespace boost::python;

void register_framemerger()
{
    class_<framemerger>("framemerger")
    .def("IntermixingSNCORSIKA", &framemerger::IntermixingSNCORSIKA)
    .def("GetTimeRange", &framemerger::GetTimeRange);
}

