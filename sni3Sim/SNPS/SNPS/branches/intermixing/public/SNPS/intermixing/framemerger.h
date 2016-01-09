#ifndef FRAMEMERGER_H_INCLUDED
#define FRAMEMERGER_H_INCLUDED

#include <SNPS/baseclass/SNPSBaseClass.h>
#include <SNPS/timing/SNCorrectTime.h>
#include <SNPS/helpers/frameobjectcombiners.h>

#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3EventHeader.h>

#include <phys-services/I3RandomService.h>

class framemerger : public SNPSBaseClass
{

public:

    framemerger() {};

    virtual ~framemerger() {};

    I3FramePtr IntermixingSNCORSIKA(I3FramePtr, I3FramePtr, double timerange, I3RandomServicePtr rng);

    // I3FramePtr MergeCORSIKA(I3FramePtr, I3FramePtr, I3RandomServicePtr, double);

    double GetTimeRange(I3EventHeaderConstPtr);


private:

  frameobjectcombiners instancefoc;

  SNCorrectTime instancecortime;

};

#endif // FRAMEMERGER_H_INCLUDED