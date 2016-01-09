#ifndef SNCORRECTTIME_H_INCLUDED
#define SNCORRECTTIME_H_INCLUDED

#include <SNPS/baseclass/SNPSBaseClass.h>
#include <SNPS/helpers/frameobjecthelpers.h>

#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <dataclasses/physics/I3Particle.h>

#include <simclasses/I3MCPE.h>

#include <clsim/I3Photon.h>

#include <phys-services/I3RandomService.h>

#include <vector>

/**
 * Class to provide all your needs to correct times in frameobjects
 */

class SNCorrectTime : public SNPSBaseClass
{
public:

    SNCorrectTime() {};

    virtual ~SNCorrectTime() {};

    /**
     * Correcting EventHeader time by adding first double to start time
     * and setting end time by adding second double (time bin width) to
     * new start time
     *
     * @params I3EventHeader Pointer
     * @params Time to shift event time by
     * @params Width of time bin 
     */
    void CorrectTimeEventHeader( I3EventHeaderPtr, double , double);

    /**
     * Apply time correction to primary and its daughter particles
     *
     * @params I3MCTree
     * @params Location of primary in mctree
     * @params time shift
     */
    void CorrectTimePrimaryDaugther(I3MCTreePtr, I3MCTree::sibling_iterator, double);

    /**
     * Wrapper around CorrectTimePrimaryDaugther. To apply an absolute time shift + random time 
     * within time bin to primary and and respective dautgher(s)
     *
     * @params I3MCTree
     * @params Location of primary
     * @params absolute time shift
     * @params time bin width
     * @params random service for get uniform random number
     */
    void CorrectTimeMCTree( I3MCTreePtr, I3MCTree::sibling_iterator, double, double, I3RandomServicePtr );

    /**
     * Wrapper around CorrectTimePrimaryDaugther. To apply an random time shoft to
     * within time bin to primary and and respective dautgher(s)
     *
     * @params I3MCTree
     * @params Location of primary
     * @params time bin width
     * @params random service for get uniform random number
     */
    void CorrectTimeMCTree( I3MCTreePtr, I3MCTree::sibling_iterator, double, I3RandomServicePtr );

    /**
     * Wrapper around CorrectTimePrimaryDaugther. To apply an random time shift to
     * within time bin to all primaries and respective dautgher(s)
     *
     * @params I3MCTree
     * @params time bin width
     * @params random service for get uniform random number
     */
    void CorrectTimeMCTree( I3MCTreePtr, double, I3RandomServicePtr );

    /**
     * Wrapper around CorrectTimePrimaryDaugther. To apply an absolute time shift +
     * random time shoft to within time bin to all primaries and respective dautgher(s)
     *
     * @params I3MCTree
     * @params absolute time shift
     * @params time bin width
     * @params random service for get uniform random number
     */
    void CorrectTimeMCTree( I3MCTreePtr, double, double, I3RandomServicePtr );

    /**
     * Shift time of MCPEs by time of primary in MCTree     
     * 
     * @params I3MCTree
     * @params OMKey in MCPESeriesMap
     */
    void CorrectTimeMCPE( I3MCTreePtr, I3MCPESeriesMap::iterator );

    /**
     * Shift time of I3Photons by time of primary in MCTree
     * 
     * @params I3MCTree
     * @params OMKey in PhotonSeriesMap
     */
    void CorrectTimePhotonSeries( I3MCTreePtr, I3PhotonSeriesMap::iterator );

    /**
     * Apply random time within time bin to CORSIKA events in both MCTree and MCPESeriesMap
     *
     * @params I3MCTree
     * @params MCPESeriesMap
     * @params Width of time bin
     * @params random service
     */
    void CorrectTimeCORSIKA( I3MCTreePtr , I3MCPESeriesMapPtr , double , I3RandomServicePtr  );

    /**
     * Shift time of MCTree by given time
     *
     * @params I3MCtree
     * @params time shift
     */
    void ShiftMCTree(I3MCTreePtr, double);
    
    /**
    * Shift time of MCPESeriesMap by given time
    *
    * @params MCPESeriesMap
    * @params time shift
    */
    void ShiftMCPESeriesMap(I3MCPESeriesMapPtr, double);

private:

};

#endif // SNCORRECTTIME_H_INCLUDED