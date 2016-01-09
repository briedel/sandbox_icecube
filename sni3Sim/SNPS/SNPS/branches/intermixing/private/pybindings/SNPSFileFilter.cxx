#include <icetray/python/list_indexing_suite.hpp>
#include <icetray/python/stream_to_string.hpp>

#include <dataclasses/ostream_overloads.hpp>

#include <tableio/converter/pybindings.h>
#include <tableio/converter/I3VectorConverter.h>

#include <SNPS/HDFreaderwriter/I3FileBooking/converter/SNPSConverter.h>
#include <SNPS/HDFreaderwriter/I3FileBooking/SNPSHitRecord.h>

using namespace boost::python;

// BOOST_PYTHON_MODULE(SNPSFileFilter)
void register_SNPSFileFilter()
{
  // load_project("libSNPSFileFilter", false);
  
  class_<SNPSHitRecord>("SNPSHitRecord")
  .add_property("I3Particle_InIceParticle", &SNPSHitRecord::part)
  .add_property("I3Particle_Primary", &SNPSHitRecord::prim)
  .add_property("Weights", &SNPSHitRecord::w)
  .add_property("I3Time", &SNPSHitRecord::dtime)
  .add_property("OMKey", &SNPSHitRecord::k);
  
  class_<I3Vector<SNPSHitRecord>, bases<I3FrameObject>, boost::shared_ptr<I3Vector<SNPSHitRecord> > >("I3VectorSNPSHitRecord")  
  .def(list_indexing_suite<I3Vector<SNPSHitRecord> >());
   
  register_pointer_conversions<I3Vector<SNPSHitRecord> >();
  I3CONVERTER_NAMESPACE(SNPS);
  I3CONVERTER_EXPORT_DEFAULT(SNPSConverter,"Writes SN particle, primary, and photon information to a table");
  typedef I3VectorConverter<SNPSConverter> I3VectorSNPSConverter;
  I3CONVERTER_EXPORT_DEFAULT(I3VectorSNPSConverter, "Dumps an I3Vector<SNPSHitRecord>");
}