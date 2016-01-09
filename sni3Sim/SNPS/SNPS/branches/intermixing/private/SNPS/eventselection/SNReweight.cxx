// #include <SNPS/eventselection/SNReweight.h>

// #include <dataclasses/I3Constants.h>
// #include <dataclasses/I3TreeUtils.h>

// #include <phys-services/I3RandomService.h>

// #include <gsl/gsl_randist.h>
// #include <gsl/gsl_rng.h>
// #include <gsl/gsl_test.h>
// #include <gsl/gsl_sf_gamma.h>

// #include <cmath>
// #include <iostream>
// #include <numeric>
// #include <iterator>

// #include <boost/foreach.hpp>
// #include <boost/make_shared.hpp>
// #include <boost/assign/list_inserter.hpp>
// #include <boost/assert.hpp>

// I3_MODULE(SNReweight);

// SNReweight::SNReweight(const I3Context &context) : I3Module(context)
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

//     AddParameter("ModelFile",
//                  "Location of the file that has the model. Only supports HDF5 atm."
//                  "Default = $I3_BUILD/SNPS/resources/supernovaspectra.h5",
//                  build_dir + "/SNPS/resources/supernovaspectra.h5");

//     AddParameter("NumParticlesinFrame",
//                  "Total Number of neutrinos in a single frame.",
//                  10000);

//     AddParameter("ShockType",
//                  "Shock type assumed for oscillations. "
//                  "Options:"
//                  "Reverse, Forward, Contact, Off"
//                  "Default = Off",
//                  "Off");

//     AddParameter("Hierarchy",
//                  "Select Neutrino Mass Hierarchy. Default = Normal",
//                  "Normal");

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

//     AddParameter("InjectionSpectrum",
//                  "Injection Spectrum used",
//                  "Flat");

//     AddParameter("Emax",
//                  "Maximum neutrino energy considered in sample",
//                  100 * I3Units::MeV);

//     AddParameter("TimeBinWidth",
//                  "Width of time bin to be considered [ms]",
//                  1.638 * I3Units::ms);

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

//     AddParameter("GenParamAlpha",
//                  "Alpha parameter for Garching/Gamma PDF",
//                  3.);

//     AddParameter("GenParammE",
//                  "Mean Energy parameter for Garching/Gamma PDF",
//                  15.);

//     AddParameter("OverSamplingFactor",
//                  "How many times the total number of neutrinos are needed",
//                  10.);

//     AddOutBox("OutBox");

// }



// void SNReweight::Configure()
// {

//     GetParameter("Distance", distance_);
//     GetParameter("RandomService", rng_);
//     GetParameter("Interaction", gwi.interaction);
//     GetParameter("ModelFile", modelfile_);
//     GetParameter("NumParticlesinFrame", nopart_);
//     GetParameter("ShockType", shocktype_);
//     GetParameter("Hierarchy", hierarchy_);
//     GetParameter("Model", model_);
//     GetParameter("Radius", radius_);
//     GetParameter("Height", height_);
//     GetParameter("InjectionSpectrum", inputspectrum_);
//     GetParameter("Emax", maxE_);
//     GetParameter("TimeBinWidth", deltat_);
//     GetParameter("InputI3MCTree", inputI3MCTreeName_);
//     GetParameter("InputSNWeightMap", inputSNWeightMapName_);
//     GetParameter("InputMCHitSeriesMap", inputMCHitSeriesMapName_);
//     GetParameter("InputPhotonSeriesMap", inputPhotonSeriesMapName_);
//     GetParameter("InputMCEventHeader", inputEventHeaderName_);
//     GetParameter("GenParamAlpha", gwi.gammaalpha );
//     GetParameter("GenParammE", gwi.gammameanE );
//     GetParameter("OverSamplingFactor", oversamplingfactor_);

//     if ( !rng_ )
//     {
//         rng_ = context_.Get<I3RandomServicePtr>();
//         if ( !rng_ )
//         {
//             log_fatal("CANNOT FIND RANDOM SERVICE");
//         }
//     }

//     if ( SNEventSelectionBaseClass::fileNotExists(modelfile_) ) log_fatal("NO MODEL FILE!");

//     std::string modelgroup_ = SNEventSelectionBaseClass::GetGroupName(model_);
//     gwi.physicsspectrum = SNEventSelectionBaseClass::GetPhysSpec(model_);

//     /// Getting data out of Spectrum file
//     InputFileReader T(modelfile_);

//     d = T.openFileandreadModel(modelgroup_);

//     SNEventSelectionBaseClass::CorrectUnits(d.pbtime_a, d.pbtime_LandmE, d.Luminosity_nuebar, d.Mean_Energy_nuebar);

//     startendtime_ = SNEventSelectionBaseClass::GetTimeRange(d.pbtime_a, d.pbtime_LandmE);

//     t_ = startendtime_.first;

//     itta = d.pbtime_a.begin();
//     ittLmE = d.pbtime_LandmE.begin();

//     log_debug("Start time in Post Bounce Time [s] %e", t_);

//     if ( shocktype_ != "Off" ) ds = T.openFileandreadShock(shocktype_);

//     Lnue_cspline_ptr = new gsl_linear_wrapper( d.pbtime_LandmE, d.Luminosity_nue );
//     Lnuebar_cspline_ptr = new gsl_linear_wrapper( d.pbtime_LandmE, d.Luminosity_nuebar);
//     Lnux_cspline_ptr = new gsl_linear_wrapper( d.pbtime_LandmE, d.Luminosity_nux );
//     mEnue_cspline_ptr = new gsl_linear_wrapper( d.pbtime_LandmE, d.Mean_Energy_nue );
//     mEnuebar_cspline_ptr = new gsl_linear_wrapper( d.pbtime_LandmE, d.Mean_Energy_nuebar );
//     mEnux_cspline_ptr = new gsl_linear_wrapper( d.pbtime_LandmE, d.Mean_Energy_nux );
//     anue_cspline_ptr = new gsl_linear_wrapper( d.pbtime_a, d.alpha_nue );
//     anuebar_cspline_ptr = new gsl_linear_wrapper( d.pbtime_a, d.alpha_nuebar );
//     anux_cspline_ptr = new gsl_linear_wrapper( d.pbtime_a, d.alpha_nux );

//     noIFrames = 0;
//     noGFrames = 0;
//     noCFrames = 0;
//     noDFrames = 0;
//     noQFrames = 0;
//     noPFrames = 0;

//     d.pbtime_LandmE.begin();
//     d.pbtime_LandmE.end();

//     std::adjacent_difference( d.pbtime_LandmE.begin(), d.pbtime_LandmE.end(), std::back_inserter(dtLandmE) );
//     std::adjacent_difference( d.pbtime_a.begin(), d.pbtime_a.end(), std::back_inserter(dtalpha) );

//     itdta = dtalpha.begin();
//     itdtLmE = dtLandmE.begin();

//     t = 0;
//     noFramesNeeded = 0;
//     noFramesPassed = 0;

//     area_ = instancegenweight.GetGenArea( radius_ );

// }

// void SNReweight::Process()
// {

//     // while ( t != t_ && t_ < startendtime_.second )
//     // {
//     //     double lumin = Lnuebar_cspline_ptr->eval(t_);
//     //     std::cout << "lumin " << lumin << std::endl;
//     //     std::cout << "time " << t_ << std::endl;
//     //     if (lumin < 0)
//     //     {
//     //         std::cout << "start time " << startendtime_.first << std::endl;
//     //         std::cout << "end time " << startendtime_.second << std::endl;
//     //         std::cout << "t_" << t_ << std::endl;
//     //         log_fatal("L < 0");
//     //     }
//     //     gwi.mEnu = mEnuebar_cspline_ptr->eval(t_) * I3Units::MeV;
//     //     gwi.alpha = anuebar_cspline_ptr->eval(t_);
//     //     gwi.N_SN = GetTotalNoNeutrinos( lumin, distance_, gwi.mEnu, deltat_ );
//     //     std::cout << "gwi.N_SN " << gwi.N_SN << std::endl;
//     //     noFramesNeeded = int(ceil(gwi.N_SN * 10 / nopart_));
//     //     gwi.N_gen = noFramesNeeded * nopart_;
//     //     std::cout << "noFramesNeeded " << noFramesNeeded << std::endl;

//     //     t_ += deltat_;
//     // }

//     // if ( t != t_ && t_ < startendtime_.second && noFramesNeeded == 0 )
//     // {
//     //     // put into functions returning gwi and noframes needed
//     //     double lumin = Lnuebar_cspline_ptr->eval(t_);

//     //     log_debug("Neutrino Luminosity %e", lumin );

//     //     gwi.mEnu = mEnuebar_cspline_ptr->eval(t_) * I3Units::MeV;

//     //     log_debug("Mean Energy %e", gwi.mEnu);

//     //     gwi.alpha = anuebar_cspline_ptr->eval(t_);

//     //     log_debug("Alpha %e", gwi.alpha);

//     //     gwi.N_SN = GetTotalNoNeutrinos( lumin, distance_, gwi.mEnu, deltat_ );

//     //     log_debug("Total number of neutrinos  %e", gwi.N_SN );

//     //     noFramesNeeded = int(ceil(gwi.N_SN * oversamplingfactor_ / nopart_));

//     //     log_debug("Number of frames needed %d", noFramesNeeded );

//     //     gwi.N_gen = noFramesNeeded * nopart_;

//     //     log_debug("Number of particles in sample %e", gwi.N_gen);

//     //     t = t_;

//     //     log_debug("Time Bin %e", t_);

//     //     log_debug("Global Time Bin %e", t);
//     // }


//     if ( t != t_ && t_ < startendtime_.second && noFramesNeeded == 0 )
//     {

//         log_info("Time bin width before increment %e", *itdtLmE);

//         itdtLmE++;

//         log_info("Time bin width after increment %e", *itdtLmE);

//         log_info( "Time %e", t_);

//         // put into functions returning gwi and noframes needed
//         double lumin = Lnuebar_cspline_ptr->eval(t_);

//         log_info("Neutrino Luminosity %e", lumin );

//         gwi.mEnu = mEnuebar_cspline_ptr->eval(t_) * I3Units::MeV;

//         log_info("Mean Energy %e", gwi.mEnu);

//         gwi.alpha = anuebar_cspline_ptr->eval(t_);

//         log_info("Alpha %e", gwi.alpha);

//         gwi.N_SN = instancegenweight.GetTotalNoNeutrinos( lumin, distance_, gwi.mEnu, *itdtLmE , area_);

//         log_info("Total number of neutrinos  %e", gwi.N_SN );

//         noFramesNeeded = int(ceil(gwi.N_SN * oversamplingfactor_ / nopart_));

//         log_info("Number of frames needed %d", noFramesNeeded );

//         gwi.N_gen = noFramesNeeded * nopart_;

//         log_info("Number of particles in sample %e", gwi.N_gen);

//         t = t_;

//         log_info("Time Bin %e", t_);

//         log_info("Global Time Bin %e", t);

//     }

//     if (noFramesNeeded == 0) log_fatal("No frames requested seems a little weird");

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
//     else if (t_ > startendtime_.second )
//     {
//         return;
//     }
//     else if ( frame->GetStop() == I3Frame::DAQ || frame->GetStop() == I3Frame::Physics )
//     {

//         log_debug("Q/P FRAME");

//         log_info("noFramesPassed initially %d", noFramesPassed);


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
//         if ( !( frame->Has(inputPhotonSeriesMapName_) ) )
//         {
//             log_fatal("No PhotonSeriesMap");
//         }
//         if ( frame->GetStop() == I3Frame::DAQ ) noQFrames++;
//         if ( frame->GetStop() == I3Frame::Physics ) log_fatal("PHYSICS FRAME.... WTF?");

//         if ( noFramesPassed < noFramesNeeded )
//         {
//             I3EventHeaderConstPtr header = frame->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
//             I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(inputI3MCTreeName_);
//             I3MCPESeriesMapConstPtr mchit = frame->Get<I3MCPESeriesMapConstPtr>(inputMCHitSeriesMapName_);
//             // I3PhotonSeriesMapConstPtr phothit = frame->Get<I3PhotonSeriesMapConstPtr>(inputPhotonSeriesMapName_);
//             SNWeightMapConstPtr weightmap = frame->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);

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
//                 log_debug("Time Bin Width [s] %e", *itdtLmE / I3Units::s);
//                 instancecortime.CorrectTimeMCTree(nfo.newmctree, primit, t_, *itdtLmE, rng_);
//             }

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
//         log_info("noFramesPassed before %d", noFramesPassed);
//         noFramesPassed++;
//         log_info("noFramesPassed after %d", noFramesPassed);


//         if ( noFramesPassed == noFramesNeeded )
//         {
//             noFramesNeeded = 0;
//             noFramesPassed = 0;
//             t_ += *itdtLmE;
//         }



//         // if ( frameList_.empty() )
//         // {
//         //     log_debug("=========Creating new frame and frame objects=========");


//         // }
//         // if ( frameList_.empty() || frameList_.size() < noFramesNeeded )
//         // {
//         //     log_debug("=========Pushing frame into frame list=========");
//         //     frameList_.push(frame);
//         // }
//         // if ( frameList_.size() == noFramesNeeded )
//         // {

//         //     log_debug("=========Concatinating frames and applying time correction=========");
//         //     log_info("Number of frames to be combined %ld", frameList_.size());
//         //     ConcatFrameobjectsApplyWeightsTimes(frameList_, nfo);
//         //     // frameList_.clear();
//         //     noFramesNeeded = 0;
//         //     t_ += *itdtLmE;

//         //


//         // }
//         // else if ( frameList_.size() >= noFramesNeeded )
//         // {
//         //     log_fatal( "Something went wrong with the frame counting");
//         // }
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

// // void SNReweight::ConcatFrameobjectsApplyWeightsTimes(std::queue<I3FramePtr> &frames, newframeobjects &nfo)
// // {

// //     log_debug("=========Combining frame objects=========");

// //     // std::queue<I3FramePtr>::const_iterator it;
// //     // for (it = frames.begin(); it != frames.end(); it++)
// //     // {
// //     int origsize = frames.size();

// //     log_debug("Original Size of queue %d", origsize);

// //     while (frames.size() > 0)
// //     {
// //         I3FramePtr it = frames.front();

// //         log_debug("Size of queue %ld", frames.size());

// //         if ( frames.size() == origsize )
// //         {
// //             I3EventHeaderConstPtr header = it->Get<I3EventHeaderConstPtr>(inputMCEventHeader_);
// //         }


// //         if ( mctree == 0 ) log_fatal(" No MCTree!");
// //         if ( mchit == 0 ) log_fatal("No MCHitSeriesMap!");
// //         if ( phothit == 0) log_fatal("No PhotonSeriesMap!");
// //         if ( weightmap == 0) log_fatal("No SNWeightMap!");

// //         I3MCTreePtr temptree = I3MCTreePtr( new I3MCTree(*mctree) );
// //         I3MCPESeriesMapPtr tempmcpe = I3MCPESeriesMapPtr( new I3MCPESeriesMap(*mchit) );
// //         I3PhotonSeriesMapPtr tempphoton = I3PhotonSeriesMapPtr( new I3PhotonSeriesMap(*phothit) );

// //         // std::vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(mctree);

// //         // log_debug( "Number of Primaries %ld", primaries.size());

// //         // if ( primaries.size() != nopart_ )
// //         // {
// //         //     log_fatal( "=========WARNING!========="
// //         //                "The MCTree is not as big as you thought.\n"
// //         //                "The MCTree has %ld primaries.",
// //         //                primaries.size() );
// //         // }

// //         int primcount = 0;
// //         I3MCTree::sibling_iterator primit;
// //         for (primit = temptree->begin(); primit != temptree->end() ; primit++)
// //         {
// //             log_debug("=========Getting Generation Weight=========");

// //             std::vector<double> weights = (*weightmap).find(std::make_pair(primit->GetMajorID(), primit->GetMinorID()))->second;

// //             gwi.Enu = primit->GetEnergy();

// //             log_debug("Neutrino Energy [GeV] %e", gwi.Enu);

// //             weights.push_back(GetGenWeight(gwi));

// //             log_debug("Generation Weight %e", weights[1]);

// //             log_debug("=========Putting things into NewWeightMap=========");

// //             nfo.newweightmap->insert( std::make_pair( std::make_pair(primit->GetMajorID(), primit->GetMinorID()), weights ) );

// //             primcount++;

// //             CorrectTimeMCTree(temptree, primit);
// //         }

// //         log_info("Number of Primaries %d", primcount);

// //         if ( primcount != nopart_ )
// //         {
// //             log_fatal( "=========WARNING!========="
// //                        "The MCTree is not the same size as you thought.\n"
// //                        "The MCTree has %d primaries.",
// //                        primcount );
// //         }

// //         // if ( frames.size() == origsize )
// //         // {
// //         //     log_debug("=========Moving first MCTree into new tree=========");
// //         //     nfo.newmctree = I3MCTreePtr( new I3MCTree(*temptree) );
// //         // }
// //         // else
// //         // {
// //         //     I3MCTreeUtils::AddPrimary(*nfo.newmctree, *temptree);
// //         // }

// //         log_debug("=========Applying Time Correction MCHitSeriesMap=========");
// //         I3MCPESeriesMap::iterator omkey;
// //         for ( omkey = tempmcpe->begin(); omkey != tempmcpe->end(); omkey++)
// //         {
// //             CorrectTimeMCPE( temptree, omkey );
// //             // (*nfo.newmchitseriesmap)[omkey->first].insert((*nfo.newmchitseriesmap)[omkey->first].end(), omkey->second.begin(), omkey->second.end());
// //         }

// //         log_debug("=========Applying Time Correction PhotonSeriesMap=========");
// //         I3PhotonSeriesMap::iterator modulekey;
// //         for ( modulekey = tempphoton->begin(); modulekey != tempphoton->end(); modulekey++)
// //         {
// //             CorrectTimePhotonSeries( temptree, modulekey );
// //             // (*nfo.newphotonseries)[modulekey->first].insert((*nfo.newphotonseries)[modulekey->first].end(), modulekey->second.begin(), modulekey->second.end());
// //         }

// //         // CorrectTime(nfo);

// //         // std::vector<I3Particle> prim2 = I3MCTreeUtils::GetPrimaries(nfo.newmctree);

// //         // std::vector<I3Particle>::const_iterator primit2;
// //         // for (primit2 = prim2.begin(); primit2 != prim2.end(); primit2++)
// //         // {
// //         //     if ( primit2->GetTime() == 0 ) log_fatal("Time is not set! Time is %e", primit2->GetTime());
// //         // }

// //         frames.pop();
// //     }
// //     // }

// // }

// // void SNReweight::CorrectTime(newframeobjects &nfo)
// // {
// //     log_debug("=========Correcting time for I3MCTree, MCHitSeriesMap, and PhotonSeriesMap=========");

// //     boost::shared_ptr<I3MCTree> tree = nfo.newmctree;
// //     boost::shared_ptr<I3MCPESeriesMap> mchit = nfo.newmchitseriesmap;
// //     boost::shared_ptr<I3PhotonSeriesMap> photonseries = nfo.newphotonseries;

// //     I3MCTree::sibling_iterator primit;
// //     for ( primit = tree->begin(); primit != tree->end(); primit++)
// //     {

// //         log_debug("=========Correcting Time MCTree=========");
// //         log_debug("Time Primary before correction [s] %e", primit->GetTime() / I3Units::s);
// //         log_debug("Time Bin [s] %e", t_ / I3Units::s );
// //         log_debug("Global Time Bin [s] %e", t / I3Units::s);
// //         log_debug("Time Bin Width [s] %e", *itdtLmE / I3Units::s);

// //         double timeinbin = rng_->Uniform(0., *itdtLmE);
// //         log_debug("Time in Bin [s] %e", timeinbin / I3Units::s);
// //         double t = t_ + timeinbin;
// //         primit->SetTime(t);

// //         log_debug("Time Primary after correction [s] %e", primit->GetTime() / I3Units::s );
// //         if (t > primit->GetTime() ) log_fatal("Time calculations are off!");

// //         I3MCTree::sibling_iterator dit;
// //         for ( dit = tree->begin(primit); dit != tree->end(primit); dit++)
// //         {
// //             log_debug("Daughter Time before correction [s] %e", dit->GetTime() / I3Units::s);
// //             dit->SetTime(t);
// //             log_debug("Daughter Time after correction [s] %e", dit->GetTime() / I3Units::s);
// //         }

// //     }

// //     I3MCPESeriesMap::iterator omkey;
// //     for ( omkey = mchit->begin(); omkey != mchit->end(); omkey++)
// //     {
// //         for ( I3MCPESeries::iterator hitit = omkey->second.begin(); hitit != omkey->second.end(); hitit++)
// //         {
// //             log_debug("Time Hit before correction %e", hitit->time );

// //             I3Particle splitpart = I3MCTreeUtils::Get(*tree, *hitit);
// //             I3ParticlePtr primarypart = I3MCTreeUtils::GetPrimary(*tree, splitpart);
// //             double newhittime = primarypart->GetTime() + hitit->time;
// //             hitit->time = newhittime;

// //             log_debug("Time Hit after correction %e", hitit->time );
// //         }
// //     }

// //     I3PhotonSeriesMap::iterator modulekey;
// //     for ( modulekey = photonseries->begin(); modulekey != photonseries->end(); modulekey++)
// //     {
// //         for ( I3PhotonSeries::iterator phothit = modulekey->second.begin(); phothit != modulekey->second.end(); phothit++)
// //         {
// //             uint64_t majid = phothit->GetParticleMajorID();
// //             int minid = phothit->GetParticleMinorID();
// //             I3Particle inicephotsource = dummyinstance.FindPart(minid, majid, *tree);

// //             log_debug("Time Photon Hit before correction %e", phothit->GetTime() );
// //             double newtime = phothit->GetTime() + inicephotsource.GetTime();
// //             phothit->SetTime(newtime);
// //             log_debug("Time Photon Hit after correction %e", phothit->GetTime() );

// //             log_debug("Start Time Photon Hit before correction %e", phothit->GetStartTime() );
// //             double newstarttime = phothit->GetStartTime() + inicephotsource.GetTime();
// //             phothit->SetStartTime(newstarttime);
// //             log_debug("Start Time Photon Hit after correction %e", phothit->GetStartTime() );

// //         }
// //     }
// // }