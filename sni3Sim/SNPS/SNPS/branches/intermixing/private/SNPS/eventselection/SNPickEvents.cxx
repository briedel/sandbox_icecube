// #include <SNPS/eventselection/SNPickEvents.h>

// #include <SNPS/interactions/IBD.h>

// #include <phys-services/I3RandomService.h>

// #include <vector>
// #include <numeric>

// I3_MODULE(SNPickEvents);

// SNPickEvents::SNPickEvents(const I3Context &context) : I3Module(context)
// {

//     std::string build_dir = std::getenv("I3_BUILD");

//     AddParameter("Distance",
//                  "Distance to supernova in kpc. Default = 10. kpc",
//                  10. * 1000. * I3Units::parsec);

//     AddParameter("RandomService",
//                  "Random Service to be used",
//                  rng_);

//     AddParameter("Interaction",
//                  "Interaction to be considered for this sample"
//                  "Options:"
//                  "Inverse Beta Decay (IBD)"
//                  "Neutrino-electron scattering (nue-e, nuebar-e, nux-e, nuxbar-e)"
//                  "Neutrino-Oxygen (nue16O-ep, nue16O-e2p, nue16O-epgamma, nuebar16O-eplusn, nuebar16O-eplus, nuebar16O-eplusngamma, nue18O)"
//                  "Default = Inverse beta decay",
//                  "IBD");

//     AddParameter("InjectionSpectrum",
//                  "Injection Spectrum used",
//                  "Flat");

//     AddParameter("GenParamAlpha",
//                  "Alpha parameter for Garching/Gamma PDF",
//                  3.);

//     AddParameter("GenParammE",
//                  "Mean Energy parameter for Garching/Gamma PDF",
//                  15.);

//     AddParameter("ModelFile",
//                  "Location of the file that has the model. Only supports HDF5 atm."
//                  "Default = $I3_BUILD/SNPS/resources/supernovaspectra.h5",
//                  build_dir + "/SNPS/resources/supernovaspectra.h5");

//     AddParameter("NumParticlesinFrame",
//                  "Total Number of neutrinos in a single frame.",
//                  10000);

//     AddParameter("Model",
//                  "Model to be used."
//                  "Format:"
//                  "ModelName_Mass_EOS"
//                  "Options:"
//                  "Garching_8.8_FullShen - Garching Model 8.8 Solar Masses with Full Shen EOS - Default"
//                  "Garching_8.8_ReduShen - Garching Model 8.8 Solar Masses with Reduced Shen EOS"
//                  "Garching_810_LS - Garching Model 8-10 Solar Masses with Lattimer and Swesty EOS"
//                  "Garching_810_WH - Garching Model 8-10 Solar Masses with Wollf and Hildebrandt EOS"
//                  "Lawrence-Livermore_20_H - Lawrence-Livermore Model 20 Solar Mass Hydrodynamic Model"
//                  "BlackHole_40_Shen - Dark SN Black Hole Model 40 Solar Mass Shen EOS"
//                  "BlackHole_40_LS - Dark SN Black Hole Model 40 Solar Mass Lattimer and Swesty EOS",
//                  "Garching_8.8_FullShen");

//     AddParameter("Radius",
//                  "Radius of Injection Volume",
//                  800. * I3Units::m);

//     AddParameter("Height",
//                  "Height of Injection Volume",
//                  1600. * I3Units::m);


//     AddParameter("Emax",
//                  "Maximum neutrino energy considered in sample",
//                  100 * I3Units::MeV);

//     AddParameter("InputI3MCTree",
//                  "I3MCTree to use for input",
//                  "I3MCTree");

//     AddParameter("InputSNWeightMap",
//                  "SNWeightMap to use for input",
//                  "SNWeightMap");

//     AddParameter("InputMCHitSeriesMap",
//                  "Name of input MCHit series",
//                  "MCHitSeriesMap");

//     AddParameter("InputPhotonSeriesMap",
//                  "Name of input Photon series",
//                  "PhotonSeriesMap");

//     AddParameter("InputMCEventHeader",
//                  "Name of input I3EventHeader",
//                  "I3EventHeader");

//     AddParameter("OverSamplingFactor",
//                  "How many times the total number of neutrinos are needed",
//                  10.);

//     AddOutBox("OutBox");

// }

// void SNPickEvents::Configure()
// {

//     GetParameter("Distance", distance_);
//     GetParameter("RandomService", rng_);
//     GetParameter("Interaction", gwi.interaction);
//     GetParameter("InjectionSpectrum", gwi.inputspectrum);
//     GetParameter("GenParamAlpha", gwi.gammaalpha );
//     GetParameter("GenParammE", gwi.gammameanE );
//     GetParameter("ModelFile", c.modelfile);
//     GetParameter("NumParticlesinFrame", nopart_);
//     GetParameter("Model", c.model);
//     GetParameter("Radius", radius_);
//     GetParameter("Height", height_);
//     GetParameter("Emax", gwi.maxE);
//     GetParameter("InputI3MCTree", inputI3MCTreeName_);
//     GetParameter("InputSNWeightMap", inputSNWeightMapName_);
//     GetParameter("InputMCHitSeriesMap", inputMCHitSeriesMapName_);
//     GetParameter("InputPhotonSeriesMap", inputPhotonSeriesMapName_);
//     GetParameter("InputMCEventHeader", inputEventHeaderName_);
//     GetParameter("OverSamplingFactor", oversamplingfactor_);

//     if ( !rng_ )
//     {
//         rng_ = context_.Get<I3RandomServicePtr>();
//         if ( !rng_ )
//         {
//             log_fatal("CANNOT FIND RANDOM SERVICE");
//         }
//     }

//     gwi.physicsspectrum = SNEventSelectionBaseClass::GetPhysSpec(c.model);

//     SNEventSelectionBaseClass::ConfigureTimeInterpolation(c);

//     t_ = c.startendtime.first;
//     vecitdt = c.itdt.begin();
//     t = -1;

//     log_debug("Start time in Post Bounce Time [s] %e", t_);

//     noFramesNeeded = 0;
//     noFramesPassed = 0;

//     noIFrames = 0;
//     noGFrames = 0;
//     noCFrames = 0;
//     noDFrames = 0;
//     noQFrames = 0;
//     noPFrames = 0;

//     area_ = instancegenweight.GetGenArea( radius_ );

// }

// void SNPickEvents::Process()
// {
//     I3FramePtr frame = PopFrame();
//     if (!frame) log_fatal(" Not passing a frame to");

//     log_debug( "Number of I Frames %d", noIFrames );
//     log_debug( "Number of G Frames %d", noGFrames );
//     log_debug( "Number of C Frames %d", noCFrames );
//     log_debug( "Number of D Frames %d", noDFrames );
//     log_debug( "Number of Q Frames %d", noQFrames );

//     if ( frame->GetStop() == I3Frame::TrayInfo )
//     {
//         log_debug("I FRAME");
//         noIFrames++;
//         boost::shared_ptr<const I3TrayInfo> info = frame->Get<boost::shared_ptr<const I3TrayInfo> > ("TrayInfo");

//         if ( noIFrames <= 2 ) PushFrame(frame);
//         else return;
//     }
//     else if ( frame->GetStop() == I3Frame::Geometry )
//     {
//         log_debug("G FRAME");
//         noGFrames++;
//         if ( noGFrames == 0 ) PushFrame(frame);
//         else return;
//     }
//     else if ( frame->GetStop() == I3Frame::Calibration )
//     {
//         log_debug("C FRAME");
//         noCFrames++;
//         if ( noCFrames == 0 ) PushFrame(frame);
//         else return;
//     }
//     else if ( frame->GetStop() == I3Frame::DetectorStatus )
//     {
//         log_debug("D FRAME");
//         noDFrames++;
//         if ( noDFrames == 0 ) PushFrame(frame);
//         else return;
//     }
//     // else if (t_ > c.startendtime_.second )
//     // {
//     //     return;
//     // }
//     else if ( frame->GetStop() == I3Frame::DAQ || frame->GetStop() == I3Frame::Physics )
//     {

//         log_debug( "Time %e", t_);

//         if ( t != t_ && t_ < c.startendtime.second && noFramesNeeded == 0 )
//         {
//             do
//             {

//                 log_info("Time bin width before increment %e", *vecitdt);

//                 vecitdt++;

//                 log_info("Time bin width after increment %e", *vecitdt);

//                 log_info( "Time %e", t_);

//                 // put into functions returning gwi and noframes needed
//                 double lumin = c.Lnuebar_cspline_ptr->eval(t_);

//                 log_info("Neutrino Luminosity %e", lumin );

//                 gwi.mEnu = c.mEnuebar_cspline_ptr->eval(t_);

//                 log_info("Mean Energy %e", gwi.mEnu);

//                 gwi.alpha = c.anuebar_cspline_ptr->eval(t_);

//                 log_info("Alpha %e", gwi.alpha);

//                 gwi.N_SN = instancegenweight.GetTotalNoInteractedNeutrinos( lumin, distance_, gwi.mEnu, \
//                            *vecitdt , area_, height_, \
//                            gwi.alpha, gwi.maxE, \
//                            gwi.interaction, \
//                            gwi.physicsspectrum);

//                 log_info("Total number of interacted neutrinos  %e", gwi.N_SN );

//                 noFramesNeeded = int(ceil(gwi.N_SN * oversamplingfactor_ / nopart_));

//                 log_info("Number of frames needed %d", noFramesNeeded );

//                 if ( noFramesNeeded < 0 ) log_fatal("WTF?");

//                 if ( noFramesNeeded == 0 ) log_info("Number frames requested == 0");

//                 gwi.N_gen = noFramesNeeded * nopart_;

//                 log_info("Number of particles in sample %e", gwi.N_gen);

//                 if ( noFramesNeeded == 0 ) t_ += *vecitdt;

//                 t = t_;

//                 log_info("Time Bin %e", t_);

//                 log_info("Global Time Bin %e", t);

//             }
//             while ( noFramesNeeded == 0 );
//         }

//         // !(t != t_ && t_ < startendtime_.second &&

//         if (noFramesNeeded <= 0) log_fatal("No frames requested seems a little weird");

//         log_debug("Q/P FRAME");

//         if ( !( frame->Has(inputI3MCTreeName_) ) )
//         {
//             log_fatal("There is no MCTree in the frame");
//         }
//         if ( !( frame->Has(inputSNWeightMapName_) ) )
//         {
//             log_fatal("No SNWeightMap");
//         }
//         if ( !( frame->Has(inputMCHitSeriesMapName_) ) )
//         {
//             log_fatal("No MCHitSeriesMap");
//         }
//         // if ( !( frame->Has(inputPhotonSeriesMapName_) ) )
//         // {
//         //     log_fatal("No PhotonSeriesMap");
//         // }
//         if ( frame->GetStop() == I3Frame::DAQ ) noQFrames++;
//         if ( frame->GetStop() == I3Frame::Physics ) log_fatal("PHYSICS FRAME.... WTF?");

//         if ( noFramesPassed < noFramesNeeded && noFramesNeeded != 0)
//         {
//             I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
//             I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(inputI3MCTreeName_);
//             I3MCPESeriesMapConstPtr mchit = frame->Get<I3MCPESeriesMapConstPtr>(inputMCHitSeriesMapName_);
//             // I3PhotonSeriesMapConstPtr phothit = frame->Get<I3PhotonSeriesMapConstPtr>(inputPhotonSeriesMapName_);
//             SNWeightMapConstPtr weightmap = frame->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);

//             if ( !header ) log_fatal("No I3EventHeader");
//             if ( !mctree ) log_fatal("No I3MCTree");
//             if ( !mchit ) log_fatal("No I3MCPESeriesMap");
//             // if ( !phothit ) log_fatal("No I3PhotonSeriesMap");
//             if ( !weightmap ) log_fatal("No SNWeightMap");


//             I3FramePtr newframe = I3FramePtr(new I3Frame( I3Frame::DAQ ) );

//             nfo.neweventheader = I3EventHeaderPtr( new I3EventHeader(*header) );
//             nfo.newmctree = I3MCTreePtr(new I3MCTree(*mctree));
//             nfo.newmchitseriesmap = I3MCPESeriesMapPtr(new I3MCPESeriesMap(*mchit));
//             // nfo.newphotonseries = I3PhotonSeriesMapPtr(new I3PhotonSeriesMap(*phothit));
//             nfo.newweightmap = SNWeightMapPtr(new SNWeightMap);
            
//             int primcount = 0;
//             I3MCTree::sibling_iterator primit;
//             for (primit = nfo.newmctree->begin(); primit != nfo.newmctree->end() ; primit++)
//             {
//                 log_debug("=========Getting Generation Weight=========");

//                 std::vector<double> weights = (*weightmap).find(std::make_pair(primit->GetMajorID(), primit->GetMinorID()))->second;

//                 gwi.Enu = primit->GetEnergy();

//                 log_debug("Neutrino Energy [GeV] %e", gwi.Enu);

//                 weights.push_back( instancegenweight.GetGenWeight(gwi) );

//                 log_debug("Generation Weight %e", weights[1]);

//                 log_debug("=========Putting things into NewWeightMap=========");

//                 nfo.newweightmap->insert( std::make_pair( std::make_pair(primit->GetMajorID(), primit->GetMinorID()), weights ) );

//                 primcount++;

//                 log_debug("=========Correcting Time MCTree=========");
//                 log_debug("Time Primary before correction [s] %e", primit->GetTime() / I3Units::s);
//                 log_debug("Time Bin [s] %e", t_ / I3Units::s );
//                 log_debug("Global Time Bin [s] %e", t / I3Units::s);
//                 log_debug("Time Bin Width [s] %e", *vecitdt / I3Units::s);
//                 instancecortime.CorrectTimeMCTree(nfo.newmctree, primit, t_, *vecitdt, rng_);
//             }

//             // if ( nopart_ != primcount ) log_fatal("No. of Primaries per frame incorrect! Primaries in frame %d", primcount);

//             log_debug("=========Applying Time Correction MCHitSeriesMap=========");
//             I3MCPESeriesMap::iterator omkey;
//             for ( omkey = nfo.newmchitseriesmap->begin(); omkey != nfo.newmchitseriesmap->end(); omkey++)
//             {
//                 instancecortime.CorrectTimeMCPE( nfo.newmctree, omkey );
//             }

//             // log_debug("=========Applying Time Correction PhotonSeriesMap=========");
//             // I3PhotonSeriesMap::iterator modulekey;
//             // for ( modulekey = nfo.newphotonseries->begin(); modulekey != nfo.newphotonseries->end(); modulekey++)
//             // {
//             //     instancecortime.CorrectTimePhotonSeries( nfo.newmctree, modulekey );
//             // }

//             frameobjecthelpers::SortHits(nfo.newmchitseriesmap);

//             newframe->Put( inputI3MCTreeName_, nfo.newmctree );
//             newframe->Put( inputEventHeaderName_, nfo.neweventheader );
//             newframe->Put( inputMCHitSeriesMapName_, nfo.newmchitseriesmap );
//             // newframe->Put( inputPhotonSeriesMapName_, nfo.newphotonseries );
//             newframe->Put( inputSNWeightMapName_, nfo.newweightmap );
            
//             PushFrame(newframe);

//         }
//         noFramesPassed++;
//         log_info("Number of Frames passed %d of %d", noFramesPassed, noFramesNeeded);

//         // if ( itdt == dtLandmE.end() || itdt == dtalpha.end() && noFramesPassed == noFramesNeeded )
//         // {
//         //     RequestSuspension();
//         // }

//         if ( noFramesPassed == noFramesNeeded )
//         {
//             noFramesNeeded = 0;
//             noFramesPassed = 0;
//             t_ += *vecitdt;
//         }
//     }
//     else if ( !(frame->GetStop() == I3Frame::TrayInfo) ||
//               !(frame->GetStop() == I3Frame::Geometry) ||
//               !(frame->GetStop() == I3Frame::Calibration) ||
//               !(frame->GetStop() == I3Frame::DetectorStatus) ||
//               !(frame->GetStop() == I3Frame::DAQ) ||
//               !(frame->GetStop() == I3Frame::Physics) )
//     {
//         log_fatal("No I, G, C, D, Q, or P frames. Check your input file.");
//     }
// }
