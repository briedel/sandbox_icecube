#include <SNPS/intermixing/framemerger.h>

I3FramePtr framemerger::IntermixingSNCORSIKA(I3FramePtr snframe, I3FramePtr bgframe, double timerange, I3RandomServicePtr rng)
{

    /*
     *
     * 1. Create new frame
     * 2. Get snframe frameobjects
     *    2.1 Eventheader
     *    2.2 I3MCTree
     *    2.3 MCPE
     *    2.4 Weights
     * 3. Make copy of snframe frameobjects
     * 4. Get bgframe frameobjects
     *    4.1 MCTree
     *    4.2 MCPE
     *    4.3 MMCTrackList?
     * 5. Make copy of bgframe frameobjects
     * 6. Shift time of bgframe objects
     * 7. Add bgframe MCTree to snframe MCTree
     * 8. Add bgframe MCPESeriesMap to snframe MCPESeriesMap
     * 9. Put new temporary frameobjects to frame
     *
     */

    I3FramePtr newframe = I3FramePtr(new I3Frame( I3Frame::DAQ ) );

    I3EventHeaderConstPtr eventheader = snframe->Get<I3EventHeaderConstPtr>("I3EventHeader");
    I3MCTreeConstPtr mctree = snframe->Get<I3MCTreeConstPtr>("I3MCTree");
    I3MCPESeriesMapConstPtr mcpe = snframe->Get<I3MCPESeriesMapConstPtr>("MCHitSeriesMap");
    SNWeightMapConstPtr weights = snframe->Get<SNWeightMapConstPtr>("SNWeightMap");

    I3EventHeaderPtr tempeventheader = I3EventHeaderPtr( new I3EventHeader(*eventheader) );
    I3MCTreePtr tempmctree = I3MCTreePtr(new I3MCTree(*mctree) );
    I3MCPESeriesMapPtr tempmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap(*mcpe) );
    SNWeightMapPtr tempweightmap = SNWeightMapPtr(new SNWeightMap(*weights) );

    I3MCTreeConstPtr bgmctree = bgframe->Get<I3MCTreeConstPtr>("I3MCTree");
    I3MCPESeriesMapConstPtr bgmcpe = bgframe->Get<I3MCPESeriesMapConstPtr>("MCHitSeriesMap");

    I3MCTreePtr tempbgmctree = I3MCTreePtr( new I3MCTree(*bgmctree) );
    I3MCPESeriesMapPtr tempbgmchitseriesmap = I3MCPESeriesMapPtr( new I3MCPESeriesMap(*bgmcpe) );

    instancecortime.CorrectTimeCORSIKA(tempbgmctree, tempbgmchitseriesmap, timerange, rng);

    I3MCTreeUtils::AddPrimary(*tempmctree, *tempbgmctree);
    instancefoc.MCPESeriesCombiner(tempmchitseriesmap, tempbgmchitseriesmap);

    newframe->Put( "I3EventHeader", tempeventheader);
    newframe->Put( "I3MCTree", tempmctree );
    newframe->Put( "MCHitSeriesMap", tempmchitseriesmap );
    newframe->Put( "SNWeightMap", tempweightmap );

    return newframe;
}

// I3FramePtr framemerger::MergeCORSIKA(I3FramePtr bgframe, I3FramePtr cframe, I3RandomServicePtr rng, double dt)
// {

//     I3FramePtr newframe = I3FramePtr(new I3Frame( I3Frame::DAQ ) );

//     I3MCTreeConstPtr mctree = cframe->Get<I3MCTreeConstPtr>("I3MCTree");
//     I3MCPESeriesMapConstPtr mcpe = cframe->Get<I3MCPESeriesMapConstPtr>("MCHitSeriesMap");
//     // I3MMCTrackListConstPtr mmc = cframe->Get<I3MMCTrackListConstPtr>("MMCTrackList");

//     I3MCTreePtr tempcmctree = I3MCTreePtr(new I3MCTree(*mctree) );
//     I3MCPESeriesMapPtr tempcmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap(*mcpe) );

//     instancecortime.CorrectTimeCORSIKA(tempcmctree, tempcmchitseriesmap, dt, rng);

//     I3MCTreeConstPtr bgmctree = bgframe->Get<I3MCTreeConstPtr>("I3MCTree");
//     I3MCPESeriesMapConstPtr bgmcpe = bgframe->Get<I3MCPESeriesMapConstPtr>("MCHitSeriesMap");
//     // I3MMCTrackListConstPtr bgmmc = bgframe->Get<I3MMCTrackListConstPtr>("MMCTrackList");

//     I3MCTreePtr tempmctree = I3MCTreePtr(new I3MCTree(*bgmctree) );
//     I3MCPESeriesMapPtr tempmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap(*bgmcpe) );
//     // I3MMCTrackListPtr tempmmc = I3MMCTrackListPtr(new I3MMCTrackList(*bgmmc) );

//     instancefoc.MCTreeCombiner( tempcmctree, tempmctree );
//     instancefoc.MCPESeriesCombiner( tempcmchitseriesmap, tempmchitseriesmap );
//     // instancefoc.MMCTrackListCombiner

//     newframe->Put( "I3MCTree", tempmctree );
//     newframe->Put( "MCHitSeriesMap", tempmchitseriesmap );

//     return newframe;
// }

double framemerger::GetTimeRange(I3EventHeaderConstPtr eventheader)
{
    if ( eventheader->GetEndTime() <= eventheader->GetStartTime() ) log_fatal("End time before Start time");
    double dt = eventheader->GetEndTime() - eventheader->GetStartTime();
    return dt;
}