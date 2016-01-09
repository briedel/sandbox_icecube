#include <SNPS/helpers/SNFrameobjectCombiner.h>

#include <dataclasses/physics/I3MCTreeUtils.h>

I3_MODULE(SNFrameobjectCombiner);

SNFrameobjectCombiner::SNFrameobjectCombiner(const I3Context &context) : I3ConditionalModule(context)
{
    AddParameter("InputI3MCTree",
                 "I3MCTree to use for input",
                 "I3MCTree");

    AddParameter("InputSNWeightMap",
                 "SNWeightMap to use for input",
                 "SNWeightMap");

    AddParameter("InputMCPESeriesMap",
                 "Name of input MCHit series",
                 "MCHitSeriesMap");

    // AddParameter("InputPhotonSeriesMap",
    //              "Name of input Photon series",
    //              "PhotonSeriesMap");

    AddParameter("InputMCEventHeader",
                 "Name of input I3EventHeader",
                 "I3EventHeader");

    AddParameter("OutputI3MCTree",
                 "I3MCTree to use for onput",
                 "I3MCTree");

    AddParameter("OutputSNWeightMap",
                 "SNWeightMap to use for output",
                 "SNWeightMap");

    AddParameter("OutputMCPESeriesMap",
                 "Name of output MCHit series",
                 "MCHitSeriesMap");

    // AddParameter("OutputPhotonSeriesMap",
    //              "Name of output Photon series",
    //              "PhotonSeriesMap");

    AddParameter("OutputMCEventHeader",
                 "Name of output I3EventHeader",
                 "I3EventHeader");

    AddOutBox("OutBox");

}

void SNFrameobjectCombiner::Configure()
{
    GetParameter("InputI3MCTree", inputI3MCTreeName_);
    GetParameter("InputSNWeightMap", inputSNWeightMapName_);
    GetParameter("InputMCPESeriesMap", inputMCHitSeriesMapName_);
    // GetParameter("InputPhotonSeriesMap", inputPhotonSeriesMapName_);
    GetParameter("InputMCEventHeader", inputEventHeaderName_);
    GetParameter("OutputI3MCTree", outputI3MCTreeName_);
    GetParameter("OutputSNWeightMap", outputSNWeightMapName_);
    GetParameter("OutputMCPESeriesMap", outputMCHitSeriesMapName_);
    // GetParameter("OutputPhotonSeriesMap", outputPhotonSeriesMapName_);
    GetParameter("OutputMCEventHeader", outputEventHeaderName_);
}

void SNFrameobjectCombiner::DAQ(I3FramePtr frame)
{
    I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(inputI3MCTreeName_);
    I3MCPESeriesMapConstPtr mchit = frame->Get<I3MCPESeriesMapConstPtr>(inputMCHitSeriesMapName_);
    // I3PhotonSeriesMapConstPtr phothit = frame->Get<I3PhotonSeriesMapConstPtr>(inputPhotonSeriesMapName_);
    SNWeightMapConstPtr weightmap = frame->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);


    if ( !mctree ) log_fatal("No I3MCTree");
    if ( !mchit ) log_fatal("No I3MCPESeriesMap");
    // if ( !phothit ) log_fatal("No I3PhotonSeriesMap");
    if ( !weightmap ) log_fatal("No SNWeightMap");

    if ( !nfo.newmctree )
    {
        I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
        if ( !header ) log_fatal("No I3EventHeader");
        nfo.neweventheader = I3EventHeaderPtr( new I3EventHeader(*header) );
        nfo.newmctree = I3MCTreePtr(new I3MCTree(*mctree));
        nfo.newmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap(*mchit));
        // nfo.newphotonseries = I3PhotonSeriesMapPtr(new I3PhotonSeriesMap(*phothit));
        nfo.newweightmap = SNWeightMapPtr(new SNWeightMap);
    }
    else
    {
        instanceFOC.MCTreeCombiner(mctree , nfo.newmctree);
        instanceFOC.MCPESeriesCombiner(mchit, nfo.newmchitseriesmap);
        instanceFOC.SNWeightMapCombiner(weightmap, nfo.newweightmap);
    }

}

void SNFrameobjectCombiner::Finish()
{
    I3FramePtr newframe = I3FramePtr(new I3Frame( I3Frame::DAQ ) );

    newframe->Put(outputI3MCTreeName_, nfo.newmctree);
    newframe->Put(outputSNWeightMapName_, nfo.newweightmap);
    newframe->Put(outputMCHitSeriesMapName_, nfo.newmchitseriesmap);
    newframe->Put(outputEventHeaderName_, nfo.neweventheader);
    PushFrame(newframe);
}