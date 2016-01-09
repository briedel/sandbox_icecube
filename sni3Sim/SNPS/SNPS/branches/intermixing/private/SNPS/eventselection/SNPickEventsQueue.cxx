#include <SNPS/eventselection/SNPickEventsQueue.h>

#include <SNPS/interactions/IBD.h>

#include <phys-services/I3RandomService.h>

#include <vector>
#include <numeric>

I3_MODULE(SNPickEventsQueue);

SNPickEventsQueue::SNPickEventsQueue(const I3Context &context) : I3Module(context)
{

    std::string build_dir = std::getenv("I3_BUILD");

    AddParameter("Distance",
                 "Distance to supernova in kpc. Default = 10. kpc",
                 10. * 1000. * I3Units::parsec);

    AddParameter("RandomService",
                 "Random Service to be used",
                 rng_);

    AddParameter("Interaction",
                 "Interaction to be considered for this sample"
                 "Options:"
                 "Inverse Beta Decay (IBD)"
                 "Neutrino-electron scattering (nue-e, nuebar-e, nux-e, nuxbar-e)"
                 "Neutrino-Oxygen (nue16O-ep, nue16O-e2p, nue16O-epgamma, nuebar16O-eplusn, nuebar16O-eplus, nuebar16O-eplusngamma, nue18O)"
                 "Default = Inverse beta decay",
                 "IBD");

    AddParameter("InjectionSpectrum",
                 "Injection Spectrum used",
                 "Flat");

    AddParameter("GenParamAlpha",
                 "Alpha parameter for Garching/Gamma PDF",
                 3.);

    AddParameter("GenParammE",
                 "Mean Energy parameter for Garching/Gamma PDF",
                 15.);

    AddParameter("ModelFile",
                 "Location of the file that has the model. Only supports HDF5 atm."
                 "Default = $I3_BUILD/SNPS/resources/supernovaspectra.h5",
                 build_dir + "/SNPS/resources/supernovaspectra.h5");

    AddParameter("NumParticlesinFrame",
                 "Total Number of neutrinos in a single frame.",
                 10000);

    AddParameter("Model",
                 "Model to be used."
                 "Format:"
                 "ModelName_Mass_EOS"
                 "Options:"
                 "Garching_8.8_FullShen - Garching Model 8.8 Solar Masses with Full Shen EOS - Default"
                 "Garching_8.8_ReduShen - Garching Model 8.8 Solar Masses with Reduced Shen EOS"
                 "Garching_810_LS - Garching Model 8-10 Solar Masses with Lattimer and Swesty EOS"
                 "Garching_810_WH - Garching Model 8-10 Solar Masses with Wollf and Hildebrandt EOS"
                 "Lawrence-Livermore_20_H - Lawrence-Livermore Model 20 Solar Mass Hydrodynamic Model"
                 "BlackHole_40_Shen - Dark SN Black Hole Model 40 Solar Mass Shen EOS"
                 "BlackHole_40_LS - Dark SN Black Hole Model 40 Solar Mass Lattimer and Swesty EOS",
                 "Garching_8.8_FullShen");

    AddParameter("Radius",
                 "Radius of Injection Volume",
                 800. * I3Units::m);

    AddParameter("Height",
                 "Height of Injection Volume",
                 1600. * I3Units::m);

    AddParameter("Emax",
                 "Maximum neutrino energy considered in sample",
                 100 * I3Units::MeV);

    AddParameter("InputI3MCTree",
                 "I3MCTree to use for input",
                 "I3MCTree");

    AddParameter("InputSNWeightMap",
                 "SNWeightMap to use for input",
                 "SNWeightMap");

    AddParameter("InputMCHitSeriesMap",
                 "Name of input MCHit series",
                 "MCHitSeriesMap");

    AddParameter("InputPhotonSeriesMap",
                 "Name of input Photon series",
                 "PhotonSeriesMap");

    AddParameter("InputMCEventHeader",
                 "Name of input I3EventHeader",
                 "I3EventHeader");

    AddParameter("OverSamplingFactor",
                 "How many times the total number of neutrinos are needed",
                 10.);

    AddParameter("EventHeaderTimeCorrection",
                 "Apply time correction to I3EventHeader",
                 true);

    AddOutBox("OutBox");

}

void SNPickEventsQueue::Configure()
{

    GetParameter("Distance", distance_);
    GetParameter("RandomService", rng_);
    GetParameter("Interaction", gwi.interaction);
    GetParameter("InjectionSpectrum", gwi.inputspectrum);
    GetParameter("GenParamAlpha", gwi.gammaalpha );
    GetParameter("GenParammE", gwi.gammameanE );
    GetParameter("ModelFile", c.modelfile);
    GetParameter("NumParticlesinFrame", nopart_);
    GetParameter("Model", c.model);
    GetParameter("Radius", radius_);
    GetParameter("Height", height_);
    GetParameter("Emax", gwi.maxE);
    GetParameter("InputI3MCTree", inputI3MCTreeName_);
    GetParameter("InputSNWeightMap", inputSNWeightMapName_);
    GetParameter("InputMCHitSeriesMap", inputMCHitSeriesMapName_);
    GetParameter("InputPhotonSeriesMap", inputPhotonSeriesMapName_);
    GetParameter("InputMCEventHeader", inputEventHeaderName_);
    GetParameter("OverSamplingFactor", oversamplingfactor_);
    GetParameter("EventHeaderTimeCorrection", ehcorr);

    if ( !rng_ )
    {
        rng_ = context_.Get<I3RandomServicePtr>();
        if ( !rng_ )
        {
            log_fatal("CANNOT FIND RANDOM SERVICE");
        }
    }
    gwi.physicsspectrum = SNEventSelectionBaseClass::GetPhysSpec(c.model);

    SNEventSelectionBaseClass::ConfigureTimeInterpolation(c);

    t_ = c.startendtime.first;
    vecitdt = c.itdt.begin();
    t = -1;

    log_debug("Start time in Post Bounce Time [s] %e", t_);

    numFramesNeeded = 0;

    noIFrames = 0;
    noGFrames = 0;
    noCFrames = 0;
    noDFrames = 0;
    noQFrames = 0;
    noPFrames = 0;

    area_ = instancegenweight.GetGenArea( radius_ );

}

void SNPickEventsQueue::Process()
{
    I3FramePtr frame = PopFrame();
    if (!frame) log_fatal("Not passing a frame to");

    log_debug( "Number of I Frames %d", noIFrames );
    log_debug( "Number of G Frames %d", noGFrames );
    log_debug( "Number of C Frames %d", noCFrames );
    log_debug( "Number of D Frames %d", noDFrames );
    log_debug( "Number of Q Frames %d", noQFrames );

    if ( frame->GetStop() == I3Frame::TrayInfo )
    {
        log_debug("I FRAME");
        noIFrames++;
        boost::shared_ptr<const I3TrayInfo> info = frame->Get<boost::shared_ptr<const I3TrayInfo> > ("TrayInfo");

        if ( noIFrames <= 2 ) PushFrame(frame);
        else return;
    }
    else if ( frame->GetStop() == I3Frame::Geometry )
    {
        log_debug("G FRAME");
        noGFrames++;
        if ( noGFrames == 0 ) PushFrame(frame);
        else return;
    }
    else if ( frame->GetStop() == I3Frame::Calibration )
    {
        log_debug("C FRAME");
        noCFrames++;
        if ( noCFrames == 0 ) PushFrame(frame);
        else return;
    }
    else if ( frame->GetStop() == I3Frame::DetectorStatus )
    {
        log_debug("D FRAME");
        noDFrames++;
        if ( noDFrames == 0 ) PushFrame(frame);
        else return;
    }
    // else if (t_ > c.startendtime_.second )
    // {
    //     return;
    // }
    else if ( frame->GetStop() == I3Frame::DAQ || frame->GetStop() == I3Frame::Physics )
    {

        log_debug( "Time %e", t_);

        if ( t != t_ && t_ < c.startendtime.second && numFramesNeeded == 0 )
        {
            do
            {

                log_info("Time bin width before increment %e", *vecitdt);

                vecitdt++;

                log_info("Time bin width after increment %e", *vecitdt);

                log_info( "Time %e", t_);

                // put into functions returning gwi and noframes needed
                double lumin = c.Lnuebar_cspline_ptr->eval(t_);

                log_info("Neutrino Luminosity %e", lumin );

                gwi.mEnu = c.mEnuebar_cspline_ptr->eval(t_);

                log_info("Mean Energy %e", gwi.mEnu);

                gwi.alpha = c.anuebar_cspline_ptr->eval(t_);

                log_info("Alpha %e", gwi.alpha);

                gwi.N_SN = instancegenweight.GetTotalNoInteractedNeutrinos( lumin, distance_, gwi.mEnu, \
                           *vecitdt , area_, height_, \
                           gwi.alpha, gwi.maxE, \
                           gwi.interaction, \
                           gwi.physicsspectrum);

                log_info("Total number of interacted neutrinos  %e", gwi.N_SN );

                int tempframenum = int(ceil(gwi.N_SN * oversamplingfactor_ / nopart_));

                if ( tempframenum < 0 ) log_fatal("Negative number of frames required ");
                else numFramesNeeded = tempframenum;

                log_info("Number of frames needed %d", numFramesNeeded );

                if ( numFramesNeeded == 0 ) log_info("Number frames requested == 0");

                gwi.N_gen = numFramesNeeded * nopart_;

                log_info("Number of particles in sample %e", gwi.N_gen);

                if ( numFramesNeeded == 0 ) t_ += *vecitdt;

                t = t_;

                log_info("Time Bin %e", t_);

                log_info("Global Time Bin %e", t);

            }
            while ( numFramesNeeded == 0 );
        }

        // !(t != t_ && t_ < startendtime_.second &&

        log_debug("Q/P FRAME");

        if ( !( frame->Has(inputI3MCTreeName_) ) )
        {
            log_fatal("There is no MCTree in the frame");
        }
        if ( !( frame->Has(inputSNWeightMapName_) ) )
        {
            log_fatal("No SNWeightMap");
        }
        if ( !( frame->Has(inputMCHitSeriesMapName_) ) )
        {
            log_fatal("No MCHitSeriesMap");
        }
        // if ( !( frame->Has(inputPhotonSeriesMapName_) ) )
        // {
        //     log_fatal("No PhotonSeriesMap");
        // }
        if ( frame->GetStop() == I3Frame::DAQ ) noQFrames++;
        if ( frame->GetStop() == I3Frame::Physics ) log_fatal("PHYSICS FRAME.... WTF?");

        if ( frameQueue.size() < numFramesNeeded )
        {
            log_debug("=========Adding Frame to Queue=========");
            frameQueue.push(frame);
            log_info("Number of Frames in Queue %ld of %d", frameQueue.size(), numFramesNeeded);
        }
        if ( frameQueue.size() == numFramesNeeded && numFramesNeeded != 0 )
        {
            log_info("=========Concatenating Frames=========");
            I3FramePtr newframe = I3FramePtr(new I3Frame( I3Frame::DAQ ) );
            nfo.neweventheader = I3EventHeaderPtr( new I3EventHeader() );
            nfo.newmctree = I3MCTreePtr(new I3MCTree() );
            nfo.newmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap() );
            // nfo.newphotonseries = I3PhotonSeriesMapPtr(new I3PhotonSeriesMap() );
            nfo.newweightmap = SNWeightMapPtr(new SNWeightMap() );

            ConcatFOApplyWeightsTimeShift( frameQueue, nfo );

            frameobjecthelpers::SortHits(nfo.newmchitseriesmap);

            newframe->Put( inputI3MCTreeName_, nfo.newmctree );
            newframe->Put( inputEventHeaderName_, nfo.neweventheader );
            newframe->Put( inputMCHitSeriesMapName_, nfo.newmchitseriesmap );
            // newframe->Put( inputPhotonSeriesMapName_, nfo.newphotonseries );
            newframe->Put( inputSNWeightMapName_, nfo.newweightmap );
            PushFrame(newframe);

            if (frameQueue.size() != 0 )
            {
                log_fatal("Your frame queue is not empty!");
            }

            numFramesNeeded = 0;
            t_ += *vecitdt;
        }
        if ( frameQueue.size() > numFramesNeeded )
        {
            log_fatal( "frameQueue.size() > numFramesNeeded " );
        }



    }
    else if ( !(frame->GetStop() == I3Frame::TrayInfo) ||
              !(frame->GetStop() == I3Frame::Geometry) ||
              !(frame->GetStop() == I3Frame::Calibration) ||
              !(frame->GetStop() == I3Frame::DetectorStatus) ||
              !(frame->GetStop() == I3Frame::DAQ) ||
              !(frame->GetStop() == I3Frame::Physics) )
    {
        log_fatal("No I, G, C, D, Q, or P frames. Check your input file.");
    }
}

void SNPickEventsQueue::ConcatFOApplyWeightsTimeShift(std::queue<I3FramePtr> &frameQueue, newframeobjects &nfo)
{

    unsigned int origsize = frameQueue.size();

    while ( frameQueue.size() > 0 )
    {
        I3FramePtr tempframe = frameQueue.front();

        I3EventHeaderConstPtr header = tempframe->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
        I3MCTreeConstPtr mctree = tempframe->Get<I3MCTreeConstPtr>(inputI3MCTreeName_);
        I3MCPESeriesMapConstPtr mchit = tempframe->Get<I3MCPESeriesMapConstPtr>(inputMCHitSeriesMapName_);
        SNWeightMapConstPtr weightmap = tempframe->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);
        // I3PhotonSeriesMapConstPtr phothit = tempframe->Get<I3PhotonSeriesMapConstPtr>(inputPhotonSeriesMapName_)


        if ( !header ) log_fatal("No I3EventHeader");
        if ( !mctree ) log_fatal("No I3MCTree");
        if ( !mchit ) log_fatal("No I3MCPESeriesMap");
        if ( !weightmap ) log_fatal("No SNWeightMap");
        // if ( !phothit ) log_fatal("No I3PhotonSeriesMap");

        if (frameQueue.size() == origsize)
        {
            nfo.neweventheader = I3EventHeaderPtr( new I3EventHeader(*header) );
            if ( ehcorr ) instancecortime.CorrectTimeEventHeader(nfo.neweventheader, t_, *vecitdt);
        }


        log_debug("=========Creating Temporary Tree and MCPESeries=========");

        I3MCTreePtr tmctree = I3MCTreePtr( new I3MCTree(*mctree) );
        I3MCPESeriesMapPtr tmcpe = I3MCPESeriesMapPtr( new I3MCPESeriesMap(*mchit) );
        // I3PhotonSeriesMapPtr tphothit = I3PhotonSeriesMapPtr(new I3PhotonSeriesMap(*phothit) );


        log_debug("Size MCTree Before %d", nfo.newmctree->size());


        I3MCTree::sibling_iterator primit;
        for (primit = tmctree->begin(); primit != tmctree->end() ; primit++)
        {

            log_debug("=========Getting Generation Weight=========");

            SNWeight weights = (*weightmap).find(*primit)->second;

            gwi.Enu = primit->GetEnergy();

            log_debug("Neutrino Energy [GeV] %e", gwi.Enu);

            weights.GenerationWeight = instancegenweight.GetGenWeight(gwi);

            weights.TotalWeight = weights.InteractionWeight * weights.GenerationWeight * weights.OscillationWeight;
            weights.GenOscWeight = weights.GenerationWeight * weights.OscillationWeight;

            log_debug("Generation Weight %e", weights.GenerationWeight);

            log_debug("=========Putting things into NewWeightMap=========");

            nfo.newweightmap->insert( std::make_pair( *primit, weights ) );

            if ( ehcorr )
            {
                instancecortime.CorrectTimeMCTree(tmctree, primit, *vecitdt , rng_);
            }
            else
            {
                instancecortime.CorrectTimeMCTree(tmctree, primit, t_, *vecitdt , rng_);
            }

            if ( nfo.newmctree->size() == 0 )
            {
                log_debug("=========Adding First Tree=========");
                nfo.newmctree = I3MCTreePtr(new I3MCTree(*tmctree));
            }
            else
            {
                log_debug("=========Adding Tree=========");
                if ( tmctree->size() != 0 )
                {
                    I3MCTreeUtils::AddPrimary(*nfo.newmctree, *primit);
                    I3MCTree::sibling_iterator dit;
                    for ( dit = tmctree->begin(primit); dit != tmctree->end(primit); dit++)
                    {
                        I3MCTreeUtils::AppendChild(nfo.newmctree, *primit, *dit);
                    }
                }
            }

        }

        log_debug("=========Correcting Event Header Time and Adding Trees Together=========");

        std::vector<I3Particle> temp = I3MCTreeUtils::GetPrimaries(*tmctree);
        log_debug( "Number of Primaries %ld", temp.size() );

        log_debug( "Size Queue %ld", frameQueue.size() );

        log_debug( "Size MCTree After %d", nfo.newmctree->size() );


        log_debug("=========Applying Time Correction MCPESeriesMap=========");
        I3MCPESeriesMap::iterator omkey;
        for ( omkey = tmcpe->begin(); omkey != tmcpe->end(); omkey++)
        {
            instancecortime.CorrectTimeMCPE(tmctree, omkey);
            (*nfo.newmchitseriesmap)[omkey->first].insert((*nfo.newmchitseriesmap)[omkey->first].end(), omkey->second.begin(), omkey->second.end());

        }

        // log_debug("=========Applying Time Correction PhotonSeriesMap=========");
        // I3PhotonSeriesMap::iterator modulekey;
        // for ( modulekey = tempphoton->begin(); modulekey != tempphoton->end(); modulekey++)
        // {
        //     (*nfo.newphotonseries)[modulekey->first].insert((*nfo.newphotonseries)[modulekey->first].end(), modulekey->second.begin(), modulekey->second.end());
        // }

        frameQueue.pop();
    }


}