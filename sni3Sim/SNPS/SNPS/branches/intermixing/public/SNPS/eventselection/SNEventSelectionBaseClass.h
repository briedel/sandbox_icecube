#ifndef SNEVENTSELECTIONBASECLASS_H_INCLUDED
#define SNEVENTSELECTIONBASECLASS_H_INCLUDED

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
 * Base class to provide repeatedly used function to event selecton modules
 */

class SNEventSelectionBaseClass : public SNPSBaseClass
{
public:

    SNEventSelectionBaseClass() {};

    virtual ~SNEventSelectionBaseClass() {};

    /**
     * Get the group name for a given model
     *
     * @return String needed to access group in HDF5 file
     */
    std::string GetGroupName(std::string);

    /**
     * Get the energy spectral function for a given model
     *
     * @return String needed to select physics spectrum
     */
    std::string GetPhysSpec(std::string);


    /**
     * Correcting the values for the units
     *
     * @params  Vector of time range for pinch parameter
     * @params  Vector of time range for luminosity and average energy
     * @params  Vector of luminosity
     */
    void CorrectUnits(std::vector<double> &, std::vector<double> &, std::vector<double> &, std::vector<double> &);

    void CorrectUnits(InputFileReader::DataModel &);

    /**
     * Finding the inside edges of the time range
     *
     * @params Vector of time range for pinch parameter
     * @params Vector of time range for luminosity and average energy
     * @return Pair with start and end time
     */
    std::pair<double, double> GetTimeRange(std::vector<double> , std::vector<double>);


    /**
     * Finding the outside edges of the time range
     *
     * @params Vector of time range for pinch parameter
     * @params Vector of time range for luminosity and average energy
     * @return Pair with start and end time
     */
    std::pair<double, double> GetTimeRangeEdges(std::vector<double>, std::vector<double>);


    /**
     * Extending the first and last bins of the data to the respective outside edges,
     * i.e. if t_0 for alpha < t_0 for L, mE we add t_0 for alpha to t for L, mE
     * and first L, mE as the value for that time bin and vice versa.
     *
     * @params Vector of time range for pinch parameter
     * @params Vector of time range for luminosity and average energy
     * @params Luminosity values
     * @params Mean Energy values
     * @params alpha values
     * @params edgetimes
     */
    void AdjustTimeVector(std::vector<double> &, std::vector<double> &, \
                          std::vector<double> &, std::vector<double> &, \
                          std::vector<double> &, std::pair<double, double> &);


    /**
     * Extending the first and last bins of the data to the respective outside edges,
     * i.e. if t_0 for alpha < t_0 for L, mE we add t_0 for alpha to t for L, mE
     * and first L, mE as the value for that time bin and vice versa.
     *
     * @params Filename
     * @return Boolean
     */
    bool fileNotExists( const std::string & );

    /**
     * Common configuration function needed for selecting events from sample
     *
     * @params Reference to configuration object
     */
    void ConfigureTimeInterpolation(modelintconfiguration &c);



private:

    /*
     * Functions to change units from model units to IceCube units
     */

    // Correct Luminosity
    void CorrectLuminUnits(std::vector<double> &);

    // Correct time units
    void CorrectTimeUnits(std::vector<double> &);

    // Correct mean energy units
    void CorrectMeanEUnits(std::vector<double> &);

};

#endif //SNEVENTSELECTIONBASECLASS_H_INCLUDED


