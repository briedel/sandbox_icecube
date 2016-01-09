#ifndef SNPSHITRECORD_H_INCLUDED
#define SNPSHITRECORD_H_INCLUDED

#define GRANULAR_GEOMETRY_SUPPORT

#include <limits>
#include <boost/shared_ptr.hpp>

// #include <clsim/I3Photon.h>

#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Time.h>

#include <icetray/OMKey.h>
#include <icetray/I3FrameObject.h>

#include <simclasses/I3MCPE.h>

#include <SNPS/helpers/frameobjects.h>

/**
 * New frame object that is read by the data collector
 * that contains all the requested information about the events
 */

class SNPSHitRecord;

class SNPSHitRecord : public I3FrameObject
{

public:

    SNPSHitRecord(){};

    // SNPSHitRecord(I3Particle pa, I3Particle pr, I3Photon ph, I3Time t, OMKey ke,
    //               double intweight, double genweight) :
    //     part(pa),
    //     prim(pr),
    //     phot(ph),
    //     dtime(t),
    //     k(ke),
    //     iw(intweight),
    //     gw(genweight)
    // {};

    SNPSHitRecord(I3Particle pa, I3Particle pr, I3MCPE pe, I3Time t, OMKey ke,
                  SNWeight weights) : part(pa), prim(pr), mcpe(pe), dtime(t), k(ke), w(weights) {};

    virtual ~SNPSHitRecord() {};

    friend class SNPSConverter;

    // Desired quantities

    //I3Particle creating I3Photon
    I3Particle part;
    I3Particle prim;
    I3MCPE mcpe;
    //I3Photon creating the hit
    // I3Photon phot;
    I3Time dtime;
    OMKey k;
    SNWeight w;

private:

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, unsigned version);

};

bool operator==(const SNPSHitRecord &r1, const SNPSHitRecord &r2);

#endif //SNPSHITRECORD_H_INCLUDED