#include <SNPS/injection/SNFilePruner.h>

I3_MODULE(SNFilePruner);

SNFilePruner::SNFilePruner(const I3Context &context) : I3Module(context)
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

    AddParameter("InputPhotonSeriesMap",
                 "Name of input Photon series",
                 "PhotonSeriesMap");

    AddParameter("InputMCEventHeader",
                 "Name of input I3EventHeader",
                 "I3EventHeader");

    AddParameter("NoPhotonHitSeriesMap",
                 "Bool to either keep or remove PhotonHitSeriesMap",
                 true);

    AddParameter("NumPrimariesPerFrame",
                 "Number of primaries in frame",
                 1000);
    
    AddParameter("NoSNWeightMap",
                 "Option to turn off usage of the SNWeightMap",
                 false);
    
    AddOutBox("OutBox");

}

void SNFilePruner::Configure()
{
    GetParameter("InputI3MCTree", inputI3MCTreeName_);
    GetParameter("InputSNWeightMap", inputSNWeightMapName_);
    GetParameter("InputMCPESeriesMap", inputMCHitSeriesMapName_);
    GetParameter("InputPhotonSeriesMap", inputPhotonSeriesMapName_);
    GetParameter("InputMCEventHeader", inputEventHeaderName_);
    GetParameter("NoPhotonHitSeriesMap", noPhotonHitSeriesMap);
    GetParameter("NumPrimariesPerFrame", numPrimaries);
    GetParameter("NoSNWeightMap", noSNWeightMap);
}

void SNFilePruner::DAQ(I3FramePtr frame)
{
    I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
    I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(inputI3MCTreeName_);
    I3MCPESeriesMapConstPtr mchit = frame->Get<I3MCPESeriesMapConstPtr>(inputMCHitSeriesMapName_);
    SNWeightMapConstPtr weightmap = frame->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);


    if ( !header ) log_fatal("No I3EventHeader");
    if ( !mctree ) log_fatal("No I3MCTree");
    if ( !mchit ) log_fatal("No I3MCPESeriesMap");
    if ( !noSNWeightMap )
    {
        if ( !weightmap ) log_fatal("No SNWeightMap");
    }

    I3FramePtr newframe = I3FramePtr(new I3Frame( I3Frame::DAQ ) );

    I3EventHeaderPtr neweventheader = I3EventHeaderPtr( new I3EventHeader(*header) );
    I3MCTreePtr newmctree = I3MCTreePtr(new I3MCTree);
    I3MCPESeriesMapPtr newmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap(*mchit));
    SNWeightMapPtr newweightmap = SNWeightMapPtr(new SNWeightMap);

    int primcount = 0;
    I3MCTree::sibling_iterator primit;
    for ( primit = mctree->begin(); primit != mctree->end(); primit++)
    {
        primcount++;
    }

    if ( primcount != numPrimaries ) log_fatal("You don't know the number of primaries in the frame");

    if (!noPhotonHitSeriesMap)
    {
        I3PhotonSeriesMapConstPtr phothit = frame->Get<I3PhotonSeriesMapConstPtr>(inputPhotonSeriesMapName_);
        I3PhotonSeriesMapPtr newphotonseries = I3PhotonSeriesMapPtr(new I3PhotonSeriesMap(*phothit));
        newframe->Put( inputPhotonSeriesMapName_, newphotonseries );
        
        I3PhotonSeriesMap::const_iterator modulekey;
        I3PhotonSeries::const_iterator p;
        for ( modulekey = phothit->begin(); modulekey != phothit->end(); modulekey++ )
        {
            for ( p = modulekey->second.begin(); p != modulekey->second.end(); modulekey++ )
            {
                I3Particle hitpart = frameobjecthelpers::FindPart(p->GetParticleMinorID(), p->GetParticleMajorID(), *mctree);
                I3ParticlePtr primarypart = I3MCTreeUtils::GetPrimary(*mctree, hitpart);
                I3MCTreeUtils::AddPrimary(newmctree, *primarypart);
                std::vector<I3Particle> daughters = I3MCTreeUtils::GetDaughters(*mctree, *primarypart);
                std::vector<I3Particle>::const_iterator dit;
                for ( dit = daughters.begin(); dit != daughters.end(); dit++ )
                {
                    I3MCTreeUtils::AppendChild(newmctree, *primarypart, *dit);
                }
                if ( !noSNWeightMap )
                {
                    SNWeight weights = (*weightmap).find(*primarypart)->second;
                    newweightmap->insert( std::make_pair(*primarypart, weights) );
                }
            }
        }
    }
    else
    {
        
        I3MCPESeriesMap::const_iterator omkey;
        I3MCPESeries::const_iterator h;
        for ( omkey = mchit->begin(); omkey != mchit->end(); omkey++)
        {

            for ( h = omkey->second.begin(); h != omkey->second.end(); h++)
            {
                I3Particle splitpart = frameobjecthelpers::GetParticle(*mctree, *h); //Finding particle associated with mcpe
                I3ParticlePtr primarypart = I3MCTreeUtils::GetPrimary(*mctree, splitpart);
                I3MCTreeUtils::AddPrimary(newmctree, *primarypart);
                std::vector<I3Particle> daughters = I3MCTreeUtils::GetDaughters(*mctree, *primarypart);
                std::vector<I3Particle>::const_iterator dit;
                for ( dit = daughters.begin(); dit != daughters.end(); dit++ )
                {
                    I3MCTreeUtils::AppendChild(newmctree, *primarypart, *dit);
                }
                if ( !noSNWeightMap )
                {
                    SNWeight weights = (*weightmap).find(*primarypart)->second;
                    newweightmap->insert( std::make_pair(*primarypart, weights) );
                }
            }
        }
    }

    newframe->Put( inputI3MCTreeName_, newmctree );
    newframe->Put( inputEventHeaderName_, neweventheader );
    newframe->Put( inputMCHitSeriesMapName_, newmchitseriesmap );
    if ( !noSNWeightMap ) newframe->Put( inputSNWeightMapName_, newweightmap );
    PushFrame(newframe);

}