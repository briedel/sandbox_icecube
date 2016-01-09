#ifndef SNWEIGHTINGBASECLASS_H_INCLUDED
#define SNWEIGHTINGBASECLASS_H_INCLUDED


#include <SNPS/baseclass/SNPSBaseClass.h>
#include <SNPS/helpers/frameobjects.h>
#include <SNPS/helpers/gsl_interp_wrapper.h>
#include <SNPS/HDFreaderwriter/inputfilereader.h>
#include <SNPS/helpers/frameobjecthelpers.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3TreeUtils.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Direction.h>
#include <dataclasses/external/tree.hh>
#include <dataclasses/physics/I3EventHeader.h>

#include <simclasses/I3MCPE.h>

#include <vector>
#include <string>

/**
 * Base class for weighting calculations
 */


class SNWeightingBaseClass : public SNPSBaseClass
{
public:

    SNWeightingBaseClass() {};

    virtual ~SNWeightingBaseClass() {};

private:


};

#endif //SNWEIGHTINGBASECLASS_H_INCLUDED


