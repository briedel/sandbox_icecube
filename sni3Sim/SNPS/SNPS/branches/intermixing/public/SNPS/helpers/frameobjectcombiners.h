#ifndef FRAMEOBJECTCOMBINERS_H_INCLUDED
#define FRAMEOBJECTCOMBINERS_H_INCLUDED


#include <SNPS/helpers/frameobjects.h>
#include <SNPS/timing/SNCorrectTime.h>

class frameobjectcombiners
{
public:

    frameobjectcombiners() {};

    virtual ~frameobjectcombiners() {};

    /**
     * Function to add primaries and daughters from one mctree to another
     * 
     * @params Old MCTree
     * @params New MCTree
     * @params Location of primary to be added
     */
    void MCTreeCombiner(I3MCTreePtr, I3MCTreePtr, I3MCTree::sibling_iterator);

    /**
     * Function to combine two MCTree
     *
     * @params Old MCTrees
     * @params New MCTree
     */
    void MCTreeCombiner(I3MCTreeConstPtr, I3MCTreePtr);

    /**
     * Function to combine two MCPESeriesMaps
     *
     * @params Old MCPESeriesMap
     * @params New MCPESeriesMap
     */
    void MCPESeriesCombiner(I3MCPESeriesMapConstPtr, I3MCPESeriesMapPtr);

    /**
     * Function to combine two MCPESeriesMaps and shift time according to the MCTree
     *
     * @params MCTree
     * @params Old MCPESeriesMap
     * @params New MCPESeriesMap
     */
    void MCPESeriesCombinerTimeShifter(I3MCTreePtr, I3MCPESeriesMapPtr, I3MCPESeriesMapPtr);

    /**
     * Function to combine two SNWeightMaps
     *
     * @params Old SNWeightMap
     * @params New SNWeightMap
     */
    void SNWeightMapCombiner(SNWeightMapConstPtr, SNWeightMapPtr);

    // void MMCTrackListCombiner();

private:

    SNCorrectTime instancecortime;

};

#endif // FRAMEOBJECTCOMBINERS_H_INCLUDED