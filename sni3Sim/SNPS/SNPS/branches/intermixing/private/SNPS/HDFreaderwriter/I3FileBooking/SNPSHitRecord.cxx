#define GRANULAR_GEOMETRY_SUPPORT

#include <SNPS/HDFreaderwriter/I3FileBooking/SNPSHitRecord.h>

#include <icetray/serialization.h>
#include <icetray/OMKey.h>

#include <dataclasses/I3Time.h>

#include <clsim/I3Photon.h>

template <class Archive>
void SNPSHitRecord::serialize(Archive &ar, unsigned version)
{
    ar &make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
    ar &make_nvp("Particle", part);
    ar &make_nvp("Primary", prim);
    ar &make_nvp("MCPE", mcpe);
    // ar & make_nvp("Photon", phot);
    ar &make_nvp("Time", dtime);
    ar &make_nvp("OMKey", k);
    ar &make_nvp("Weights", w);
}

bool operator==(const SNPSHitRecord &r1, const SNPSHitRecord &r2)
{
    return (r1.part == r2.part && r1.prim == r2.prim && r1.k == r2.k);
}

I3_SERIALIZABLE(SNPSHitRecord);
I3_POINTER_TYPEDEFS(SNPSHitRecord);