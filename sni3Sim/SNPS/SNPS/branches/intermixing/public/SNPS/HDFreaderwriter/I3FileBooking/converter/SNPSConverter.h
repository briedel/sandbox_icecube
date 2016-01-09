#ifndef SNPSCONVERTER_H_INCLUDED
#define SNPSCONVERTER_H_INCLUDED

#include <SNPS/HDFreaderwriter/I3FileBooking/SNPSHitRecord.h>

#include <dataclasses/physics/I3Particle.h>

#include <clsim/I3Photon.h>

#include <tableio/I3Converter.h>

/**
 * Class to register the tableio converter
 */

class SNPSConverter : public I3ConverterImplementation<SNPSHitRecord>
{

private:

    virtual I3TableRowDescriptionPtr CreateDescription(const SNPSHitRecord &r);
    virtual size_t FillRows(const SNPSHitRecord &r, I3TableRowPtr rows);
};

#endif //SNPSCONVERTER_H_INCLUDED