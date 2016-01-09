#include <icetray/python/dataclass_suite.hpp>
#include <sndaq/I3SNDataclasses.h>

using namespace boost::python;

void register_I3SNDatagramMap()
{
  class_<I3SNDatagramMap, bases<I3FrameObject>, I3SNDatagramMapPtr>("I3SNDatagramMap")
    .def(dataclass_suite<I3SNDatagramMap>());

  register_pointer_conversions<I3SNDatagramMap>();
}
