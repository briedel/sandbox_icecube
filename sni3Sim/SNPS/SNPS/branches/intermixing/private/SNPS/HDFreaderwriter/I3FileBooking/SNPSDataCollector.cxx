#include <SNPS/HDFreaderwriter/I3FileBooking/SNPSDataCollector.h>
#include <SNPS/HDFreaderwriter/I3FileBooking/SNPSHitRecord.h>

#include <SNPS/helpers/frameobjects.h>

#include <dataclasses/physics/I3MCTreeUtils.h>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/physics/I3EventHeader.h>

#include <icetray/OMKey.h>

I3_MODULE(SNPSDataCollector);

SNPSDataCollector::SNPSDataCollector(const I3Context &ctx) :
    I3ConditionalModule(ctx),
    inputMCPESeriesName_(""),
    inputPhotonSeriesName_(""),
    inputMCTreeName_(""),
    inputEventHeaderName_(""),
    outputMCPEFilterName_(""),
    outputPhotonFilterName_("")
{

    AddParameter("InputMCPESeriesName",
                 "Name of the input I3MCPESeriesMap",
                 inputMCPESeriesName_);

    AddParameter("InputPhotonSeriesName",
                 "Name of the input I3PhotonSeriesMap",
                 inputPhotonSeriesName_);

    AddParameter("InputMCTreeName",
                 "Name of the input I3MCTree",
                 inputMCTreeName_);

    AddParameter("InputEventHeaderName",
                 "Name of the input I3EventHeader",
                 inputEventHeaderName_);

    AddParameter("InputSNWeightName",
                 "Name of the input SNWeightMap",
                 inputSNWeightMapName_);

    AddParameter("OutputMCPEFilterName",
                 "Name of the output MCPEFilter",
                 outputMCPEFilterName_);

    AddParameter("OutputPhotonFilterName",
                 "Name of the output PhotonFilter",
                 outputPhotonFilterName_);

    AddOutBox("OutBox");

}

void SNPSDataCollector::Configure()
{
    GetParameter("InputMCPESeriesName", inputMCPESeriesName_);
    GetParameter("InputPhotonSeriesName", inputPhotonSeriesName_);
    GetParameter("InputMCTreeName", inputMCTreeName_);
    GetParameter("InputEventHeaderName", inputEventHeaderName_);
    GetParameter("InputSNWeightName", inputSNWeightMapName_);
    GetParameter("OutputMCPEFilterName", outputMCPEFilterName_);
    GetParameter("OutputPhotonFilterName", outputPhotonFilterName_);
}

void SNPSDataCollector::Physics(I3FramePtr frame)
{

    log_debug("=========SNPSDataCollector=========");

    if ( !frame->Has(inputEventHeaderName_) ) log_fatal("NO EVENTHEADER");
    if ( !frame->Has(inputMCTreeName_) ) log_fatal("NO MCTREE");
    if ( !frame->Has(inputMCPESeriesName_) ) log_fatal("NO MCPES");
    // if ( !frame->Has(inputPhotonSeriesName_) ) log_fatal("NO PHOTONSERIES");
    if ( !frame->Has(inputSNWeightMapName_) ) log_fatal("SNWEIGHTMAP"); 

    I3EventHeaderConstPtr eventheader = frame->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
    I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(inputMCTreeName_);
    I3MCPESeriesMapConstPtr hit = frame->Get<I3MCPESeriesMapConstPtr>(inputMCPESeriesName_);
    // I3PhotonSeriesMapConstPtr photon = frame->Get<I3PhotonSeriesMapConstPtr>(inputPhotonSeriesName_);
    SNWeightMapConstPtr weightmap = frame->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);

    if ( !eventheader ) log_fatal("No I3EventHeader");
    if ( !mctree ) log_fatal("No I3MCTree");
    if ( !hit ) log_fatal("No I3MCPESeriesMap");
    if ( !weightmap ) log_fatal("No SNWeightMap");

    boost::shared_ptr<I3Vector<SNPSHitRecord> > resultMCPE (new I3Vector<SNPSHitRecord>);
    // boost::shared_ptr<I3Vector<SNPSHitRecord> > resultPhoton (new I3Vector<SNPSHitRecord>);

    I3Time starttime = eventheader->GetStartTime();

    I3MCPESeriesMap::const_iterator omkey;
    for ( omkey = hit->begin(); omkey != hit->end(); omkey++)
    {
        //Looping through omkeys
        I3MCPESeries::const_iterator h;
        for ( h = omkey->second.begin(); h != omkey->second.end(); h++)
        {
            //Looping through MCPE
            I3Particle splitpart = frameobjecthelpers::GetParticle(*mctree, *h); //Finding particle associated with mcpe
            I3ParticlePtr primarypart = I3MCTreeUtils::GetPrimary(*mctree, splitpart);
            //Idenitifying the I3Particle that generated hit
            SNWeight weights = (*weightmap).find(*primarypart)->second;
            if (weights.OscillationWeight == 0)
            {
                if (weights.GenerationWeight == 0)
                {
                    weights.GenerationWeight = 1.;
                }
                weights.OscillationWeight = 1.;
            }
            // ModuleKey modulekey;
            // modulekey.SetString(omkey->first.GetString());
            // modulekey.SetOM(omkey->first.GetOM());
            // I3Photon phit = frameobjecthelpers::FindPhoton(MCHitminid, MCHitmajid, modulekey, *photon);
            resultMCPE->push_back(SNPSHitRecord(splitpart, *primarypart, *h, starttime, omkey->first, weights));
        }
    }

    // I3PhotonSeriesMap::const_iterator modulekey;
    // for ( modulekey = photon->begin(); modulekey != photon->end(); modulekey++ )
    // {
    //     //Similar to above, but for I3Photons, rather than MCHits
    //     I3PhotonSeries::const_iterator h;
    //     for ( h = modulekey->second.begin(); h != modulekey->second.end(); h++ )
    //     {
    //         int PSminid = h->GetParticleMinorID();
    //         uint64_t PSmajid = h->GetParticleMajorID();
    //         int hitid = h->GetID();
    //         OMKey omkeyHit;
    //         omkeyHit.SetString(modulekey->first.GetString());
    //         omkeyHit.SetOM(modulekey->first.GetOM());
    //         if ( FindHit(hitid, PSminid, PSmajid, omkeyHit, *hit) )
    //         {
    //             I3Photon psmhit = *h;
    //             I3Particle splitpsmp = frameobjectcombiners::FindPart(PSminid, PSmajid, *mctree);
    //             I3ParticlePtr primarypsmp = I3MCTreeUtils::GetPrimary(*mctree, splitpsmp);
    //             resultPSM->push_back(SNPSHitRecord(inicepsmp, *primarypsmp, psmhit, *drivingtime, omkeyHit));
    //         }
    //     }
    // }


    frame->Put(outputMCPEFilterName_, resultMCPE);
    // frame->Put(outputPhotonFilterName_, resultPhoton);
    PushFrame(frame);

}

typedef I3Vector<SNPSHitRecord> I3VectorSNPSHitRecord;
I3_SERIALIZABLE(I3VectorSNPSHitRecord);
