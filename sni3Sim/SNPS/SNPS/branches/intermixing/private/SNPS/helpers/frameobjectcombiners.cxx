#include <SNPS/helpers/frameobjectcombiners.h>

void frameobjectcombiners::MCTreeCombiner( I3MCTreeConstPtr mctree, I3MCTreePtr newmctree)
{
    I3MCTree::sibling_iterator primit;
    for (primit = mctree->begin(); primit != mctree->end() ; primit++)
    {
        I3MCTreeUtils::AddPrimary(*newmctree, *primit);
        I3MCTree::sibling_iterator dit;
        for ( dit = mctree->begin(primit); dit != mctree->end(primit); dit++)
        {
            I3MCTreeUtils::AppendChild(newmctree, *primit, *dit);
        }
    }
}

void frameobjectcombiners::MCTreeCombiner( I3MCTreePtr mctree, I3MCTreePtr newmctree, I3MCTree::sibling_iterator primit)
{
    I3MCTreeUtils::AddPrimary(*newmctree, *primit);
    I3MCTree::sibling_iterator dit;
    for ( dit = mctree->begin(primit); dit != mctree->end(primit); dit++)
    {
        I3MCTreeUtils::AppendChild(newmctree, *primit, *dit);
    }
}

void frameobjectcombiners::MCPESeriesCombiner(I3MCPESeriesMapConstPtr imcpeseries, I3MCPESeriesMapPtr fmcpeseries)
{
    I3MCPESeriesMap::const_iterator omkey;
    for ( omkey = imcpeseries->begin(); omkey != imcpeseries->end(); omkey++)
    {
        (*fmcpeseries)[omkey->first].insert((*fmcpeseries)[omkey->first].end(), omkey->second.begin(), omkey->second.end());
    }
}

void frameobjectcombiners::MCPESeriesCombinerTimeShifter(I3MCTreePtr mctree, I3MCPESeriesMapPtr imcpeseries, I3MCPESeriesMapPtr fmcpeseries)
{
    I3MCPESeriesMap::iterator omkey;
    for ( omkey = imcpeseries->begin(); omkey != imcpeseries->end(); omkey++)
    {
        instancecortime.CorrectTimeMCPE(mctree, omkey);
        (*fmcpeseries)[omkey->first].insert((*fmcpeseries)[omkey->first].end(), omkey->second.begin(), omkey->second.end());

    }
}

void frameobjectcombiners::SNWeightMapCombiner(SNWeightMapConstPtr isnweightmap, SNWeightMapPtr fsnweightmap)
{
    SNWeightMap::const_iterator snwmit;
    for (snwmit = isnweightmap->begin(); snwmit != isnweightmap->end(); snwmit++ )
    {
        fsnweightmap->insert(*snwmit);
    }
}

// void frameobjectcombiners::MMCTrackListCombiner(I3MMCTrackListConstPtr , I3MMCTrackListPtr)
// {

// }