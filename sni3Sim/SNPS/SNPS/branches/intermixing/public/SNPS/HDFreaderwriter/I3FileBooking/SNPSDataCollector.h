#ifndef SNPSDATACOLLECTOR_H_INCLUDED
#define SNPSDATACOLLECTOR_H_INCLUDED
#define GRANULAR_GEOMETRY_SUPPORT

#include <SNPS/helpers/frameobjecthelpers.h>

#include <icetray/I3Frame.h>
#include <icetray/I3ConditionalModule.h>

/**
 * IceTray Module to filter the relevant information into a frameobject
 * that can be written to a table based file format
 */

class SNPSDataCollector : public I3ConditionalModule
{

public:

    SNPSDataCollector(const I3Context& ctx);

    virtual ~SNPSDataCollector() {};


    /**
     * Standard function for IceTray module
     */
    void Configure();

    void Physics(I3FramePtr);

private:
    
    // Input quantities
    std::string inputMCPESeriesName_;
    std::string inputPhotonSeriesName_;
    std::string inputMCTreeName_;
    std::string inputEventHeaderName_;
    std::string inputSNWeightMapName_;
    std::string outputMCPEFilterName_;
    std::string outputPhotonFilterName_;

    SET_LOGGER("SNPSDataCollector");
};

#endif //SNPSDATACOLLECTOR_H_INCLUDED