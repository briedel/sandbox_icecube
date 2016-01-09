// #include <SNPS/weighting/SNCorrectWeights.h>

// #include <SNPS/helpers/frameobjects.h>

// #include <SNPS/interactions/IBD.h>
// #include <SNPS/interactions/escatter_flavor_selection.h>

// #include <dataclasses/physics/I3MCTree.h>
// #include <dataclasses/physics/I3MCTreeUtils.h>
// #include <dataclasses/physics/I3Particle.h>


// I3_MODULE(SNCorrectWeights);

// SNCorrectWeights::SNCorrectWeights(const I3Context &context) : I3Module(context)
// {
//     AddParameter("InputSNWeightMapName",
//                  "Name of input SNWeightMap",
//                  "SNWeightMap");

//     AddParameter("InputI3MCTreeName",
//                  "Name of input I3MCTree",
//                  "I3MCTree");

//     AddParameter("OutputSNWeightMapName",
//                  "Name of output SNWeightMap",
//                  "SNWeightMap");
    
//     AddParameter("InjectionVolumeHeight",
//                  "Height of the forced interaction volume. Default = 1600. m",
//                  1600. * I3Units::m);

//     AddParameter("InjectionCylinderRadius",
//                  "Radius of the forced interaction volume. Default = 800. m",
//                  800. * I3Units::m);

//     AddParameter("Interaction",
//                  "Interaction to be considered for this sample"
//                  "Options:"
//                  "Inverse Beta Decay (IBD)"
//                  "Neutrino-electron scattering (nue-e, nuebar-e, nux-e, nuxbar-e)"
//                  "Neutrino-Oxygen (nue16O-ep, nue16O-e2p, nue16O-epgamma, nuebar16O-eplusn, nuebar16O-eplus, nuebar16O-eplusngamma, nue18O)"
//                  "Default = Inverse beta decay",
//                  "IBD");

//     AddOutBox("OutBox");

// }


// void SNCorrectWeights::Configure()
// {
//     GetParameter("InputSNWeightMapName", inputSNWeightMapName_);
//     GetParameter("InputI3MCTreeName", inputI3MCTreeName_);
//     GetParameter("OutputSNWeightMapName", outputSNWeightMapName_);
//     GetParameter("InjectionVolumeHeight", c_.height_);
//     GetParameter("InjectionVolumeRadius", c_.radius_);
//     GetParameter("Interaction", interaction_);

//     if (interaction_ == "IBD")
//     {
//         c_.inter = boost::shared_ptr<Interaction> (new IBD);
//     }
//     else if (interaction_ == "nue-e")
//     {
//         c_.inter = boost::shared_ptr<Interaction> (new EScatter_neutrino_e);
//     }
//     else if (interaction_ == "nuebar-e")
//     {
//         c_.inter = boost::shared_ptr<Interaction> (new EScatter_antineutrino_e);
//     }
//     else if (interaction_ == " nux-e")
//     {
//         c_.inter = boost::shared_ptr<Interaction> (new EScatter_neutrino_x);
//     }
//     else if (interaction_ == " nuxbar-e")
//     {
//         c_.inter = boost::shared_ptr<Interaction> (new EScatter_antineutrino_x);
//     }
//     else
//     {

//         log_fatal("PLEASE SELECT A SUPPORTED INTERACTION");

//     }
//     intweight = boost::shared_ptr<SNInteractionWeight>( new SNInteractionWeight(c_) ); 
// }

// void SNCorrectWeights::DAQ(I3FramePtr frame)
// {
//     if ( !frame->Has(inputSNWeightMapName_) ) log_fatal("No SNWeightMap");
//     if ( !frame->Has(inputI3MCTreeName_) ) log_fatal("No I3MCTree");

//     boost::shared_ptr<const I3MCTree> mctree = frame->Get<boost::shared_ptr<const I3MCTree> >(inputI3MCTreeName_);
//     boost::shared_ptr<const SNWeightMap> weightmap = frame->Get<boost::shared_ptr<const SNWeightMap> >(inputSNWeightMapName_);

//     boost::shared_ptr<SNWeightMap> newweightmap( new SNWeightMap );

//     std::vector<I3Particle> primaries = I3MCTreeUtils::GetPrimaries(mctree);

//     std::vector<I3Particle>::const_iterator primit;
//     for ( primit = primaries.begin(); primit != primaries.end(); primit++)
//     {
//         double weight = intweight->GetInteractionWeight( *primit );
//         SNWeight weights = (*weightmap).find(std::make_pair(primit->GetMajorID(), primit->GetMinorID()))->second;
//         weights.insert( weights.begin(), weight );
//         newweightmap->insert( std::make_pair( std::make_pair(primit->GetMajorID(), primit->GetMinorID()), weights ) );
//     }

//     frame->Delete(inputSNWeightMapName_);
//     frame->Put(outputSNWeightMapName_, newweightmap);
//     PushFrame(frame);
// }