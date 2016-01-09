#include <SNPS/merging/SNFrameMerger.h>

I3_MODULE(SNFrameMerger);

SNFrameMerger::SNFrameMerger(const I3Context &context) : I3ConditionalModule(context),
totalLength(0),
frameLength(0)
{

    AddParameter("InputI3MCTree",
                 "I3MCTree to use for input",
                 "I3MCTree");

    AddParameter("InputSNWeightMap",
                 "SNWeightMap to use for input",
                 "SNWeightMap");

    AddParameter("InputMCPESeriesMap",
                 "Name of input MCHit series",
                 "MCPESeriesMap");

    AddParameter("InputEventHeader",
                 "Name of input I3EventHeader",
                 "I3EventHeader");

    AddParameter("FrameLength",
                 "Length of desired frame",
                 100*I3Units::millisecond);
                 
    AddParameter("EventHeaderTimeCorrection",
                 "Apply time correction to I3EventHeader",
                 true);

    AddOutBox("OutBox");

}

void SNFrameMerger::Configure()
{

    GetParameter("InputI3MCTree", inputI3MCTreeName_);
    GetParameter("InputSNWeightMap", inputSNWeightMapName_);
    GetParameter("InputMCPESeriesMap", inputMCPESeriesMapName_);
    GetParameter("InputEventHeader", inputEventHeaderName_);
    GetParameter("FrameLength", fullFrameLength);
    GetParameter("EventHeaderTimeCorrection", ehcorr);

}

void SNFrameMerger::DAQ(I3FramePtr frame)
{
    if (!frame) log_fatal("Frame doesn't exist?");

    if ( !( frame->Has(inputI3MCTreeName_) ) )
    {
        log_fatal("There is no MCTree in the frame");
    }
    if ( !( frame->Has(inputSNWeightMapName_) ) )
    {
        log_fatal("No SNWeightMap");
    }
    if ( !( frame->Has(inputEventHeaderName_) ) )
    {
        log_fatal("No EventHeader");
    }
    if ( !( frame->Has(inputMCPESeriesMapName_)) )
    {
        log_fatal("No MCPESeriesMap");
    }
    
    I3EventHeaderConstPtr eventheader = frame->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
    
    if (frameLength == 0) absolutestarttime = eventheader->GetStartTime();
    
    frameLength = eventheader->GetEndTime() - eventheader->GetStartTime();
    
    if (frameLength == 0) log_fatal("Frame with 0 length.... Weird");
    
    if ( queueLength + frameLength >= fullFrameLength)
    {
        frameDeque.push_back(frame);
        log_debug("size of deque %lu", frameDeque.size());
        if (frameDeque.size() >= 1) CreateNewFrameProcessDequeue(frameDeque);
        if (frameDeque.size() >= 1) log_fatal("Still frames in the Deque");
        queueLength = 0;
    }
    else
    {
        frameDeque.push_back(frame);
        queueLength += frameLength;
    }
    
}

void SNFrameMerger::Finish()
{
    log_debug("size of deque %lu", frameDeque.size());
    if ( frameDeque.size() >= 1) CreateNewFrameProcessDequeue(frameDeque);
}

void SNFrameMerger::CreateNewFrameProcessDequeue(std::deque<I3FramePtr>& frameDeque)
{
    if (frameDeque.size() == 1)
    {
        PushFrame(frameDeque.front());
        frameDeque.pop_front();
    }
    else
    {
        I3FramePtr newframe = I3FramePtr(new I3Frame( I3Frame::DAQ ) );
        nfo.neweventheader = I3EventHeaderPtr( new I3EventHeader() );
        nfo.newmctree = I3MCTreePtr(new I3MCTree() );
        nfo.newmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap() );
        // nfo.newphotonseries = I3PhotonSeriesMapPtr(new I3PhotonSeriesMap() );
        nfo.newweightmap = SNWeightMapPtr(new SNWeightMap() );
    
        I3FramePtr tempframe = frameDeque.front();
        I3EventHeaderConstPtr header = tempframe->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
    
        nfo.neweventheader = I3EventHeaderPtr( new I3EventHeader(*header) );
        nfo.neweventheader->SetStartTime( absolutestarttime + totalLength);
        nfo.neweventheader->SetEndTime(nfo.neweventheader->GetStartTime() + queueLength + frameLength);

        ConcatenateFrames(frameDeque, nfo);

        frameobjecthelpers::SortHits(nfo.newmchitseriesmap);

        newframe->Put( inputI3MCTreeName_, nfo.newmctree );
        newframe->Put( inputEventHeaderName_, nfo.neweventheader );
        newframe->Put( inputMCPESeriesMapName_, nfo.newmchitseriesmap );
        // newframe->Put( inputPhotonSeriesMapName_, nfo.newphotonseries );
        newframe->Put( inputSNWeightMapName_, nfo.newweightmap );
        PushFrame(newframe);
        totalLength += queueLength + frameLength;
    }
    if (frameDeque.size() != 0) log_fatal("Still frames in deque");

}

void SNFrameMerger::ConcatenateFrames( std::deque<I3FramePtr>& frameDeque, SNPSBaseClass::newframeobjects& nfo)
{
    
    unsigned int origsize = frameDeque.size();
    
    double deltat = 0;
    
    while( frameDeque.size() )
    {

        I3FramePtr tempframe = frameDeque.front();

        I3EventHeaderConstPtr header = tempframe->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
        I3MCTreeConstPtr mctree = tempframe->Get<I3MCTreeConstPtr>(inputI3MCTreeName_);
        I3MCPESeriesMapConstPtr mchit = tempframe->Get<I3MCPESeriesMapConstPtr>(inputMCPESeriesMapName_);
        SNWeightMapConstPtr weightmap = tempframe->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);
        // I3PhotonSeriesMapConstPtr phothit = tempframe->Get<I3PhotonSeriesMapConstPtr>(inputPhotonSeriesMapName_)


        if ( !header ) log_fatal("No I3EventHeader");
        if ( !mctree ) log_fatal("No I3MCTree");
        if ( !mchit ) log_fatal("No I3MCPESeriesMap");
        if ( !weightmap ) log_fatal("No SNWeightMap");
        
        if (frameDeque.size() == origsize)
        {
            nfo.newmctree = I3MCTreePtr(new I3MCTree(*mctree));
            nfo.newmchitseriesmap = I3MCPESeriesMapPtr( new I3MCPESeriesMap(*mchit) );
            nfo.newweightmap = SNWeightMapPtr(new SNWeightMap(*weightmap) );
        }
        else
        {
            log_trace("=========Getting Delta T between frames=========");
            deltat =  header->GetStartTime() - nfo.neweventheader->GetStartTime();
                        
            log_trace("=========Adding and Shifting Tree=========");
            I3MCTreePtr tmctree = I3MCTreePtr( new I3MCTree(*mctree) );
            
            ShiftMCTree(tmctree, deltat);
            
            MergeMCTree(nfo.newmctree, tmctree);
            
            log_trace("=========Adding and Shifting MCPESeriesMap=========");
            
            I3MCPESeriesMapPtr tmcpe = I3MCPESeriesMapPtr( new I3MCPESeriesMap(*mchit) );
            
            ShiftMCPESeriesMap(tmcpe, deltat);
            
            MergeMCPESeriesMap(nfo.newmchitseriesmap, tmcpe);
            
            SNWeightMapCombiner(nfo.newweightmap, weightmap);
            
        }
        
        frameDeque.pop_front();
    }
}

void SNFrameMerger::ShiftMCTree(I3MCTreePtr tree, double dt )
{
    I3MCTree::sibling_iterator primit;
    for ( primit = tree->begin(); primit != tree->end(); primit++ )
    {
        primit->SetTime( primit->GetTime() + dt );
        I3MCTree::sibling_iterator dit;
        for ( dit = tree->begin(primit); dit != tree->end(primit); dit++)
        {
            dit->SetTime(dit->GetTime() + dt);
        }
        
    }
}

void SNFrameMerger::MergeMCTree(I3MCTreePtr newmctree, I3MCTreePtr mctree)
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

void SNFrameMerger::ShiftMCPESeriesMap(I3MCPESeriesMapPtr mcpemap, double dt)
{
    I3MCPESeriesMap::iterator omkey;
    I3MCPESeries::iterator mcpeit;
    for ( omkey = mcpemap->begin(); omkey != mcpemap->end(); omkey++)
    {
        for ( mcpeit = omkey->second.begin(); mcpeit != omkey->second.end(); mcpeit++ )
        {
            mcpeit->time += dt;
        }
    }
}

void SNFrameMerger::MergeMCPESeriesMap(I3MCPESeriesMapPtr newmcpemap, I3MCPESeriesMapPtr mcpemap)
{
    I3MCPESeriesMap::iterator omkey;
    for ( omkey = mcpemap->begin(); omkey != mcpemap->end(); omkey++)
    {
        (*newmcpemap)[omkey->first].insert((*newmcpemap)[omkey->first].end(), omkey->second.begin(), omkey->second.end());
    }
}

void SNFrameMerger::SNWeightMapCombiner(SNWeightMapPtr newsnweightmap, SNWeightMapConstPtr snweightmap)
{
    SNWeightMap::const_iterator snwmit;
    for (snwmit = snweightmap->begin(); snwmit != snweightmap->end(); snwmit++ )
    {
        newsnweightmap->insert(*snwmit);
    }
}