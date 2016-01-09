#include <SNPS/oscillation/SNOscillations.h>

#include <SNPS/eventselection/SNEventSelectionBaseClass.h>

I3_MODULE(SNOscillations);

SNOscillations::SNOscillations(const I3Context &context) : I3ConditionalModule(context)
{

    std::string build_dir = std::getenv("I3_BUILD");

    AddParameter("InputI3MCTree",
                 "I3MCTree to use for input",
                 "I3MCTree");

    AddParameter("InputSNWeightMap",
                 "SNWeightMap to use for input",
                 "SNWeightMap");

    // AddParameter("InputMCHitSeriesMap",
    //              "Name of input MCHit series",
    //              "MCHitSeriesMap");

    AddParameter("InputMCEventHeader",
                 "Name of input I3EventHeader",
                 "I3EventHeader");

    AddParameter("EventHeaderTimeCorrection",
                 "Apply time correction to I3EventHeader",
                 true);

    AddParameter("AbsoluteStartTime",
                "Assumed start time of run",
                I3Time(2012, 158100000000000000));

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

    AddParameter("ModelFile",
                 "Location of the file that has the model. Only supports HDF5 atm."
                 "Default = $I3_BUILD/SNPS/resources/supernovaspectra.h5",
                 build_dir + "/SNPS/resources/supernovaspectra.h5");


    AddParameter("Hierarchy",
                 "Hierarchy Selection",
                 "Normal");

    // NOTE: ASSUMING NH DEFAULT VALUES HERE

    AddParameter("sintheta13",
                 "Value for sintheta13",
                 0.155);

    AddParameter("sintheta23",
                 "Value for sintheta23",
                 0.621);

    AddParameter("sintheta12",
                 "Value for sintheta12",
                 0.554);

}

void SNOscillations::Configure()
{

    GetParameter("ModelFile", c_.modelfile);
    GetParameter("Model", c_.model);
    GetParameter("InputI3MCTree", inputI3MCTreeName_);
    GetParameter("InputSNWeightMap", inputSNWeightMapName_);
    // GetParameter("InputMCHitSeriesMap", inputMCHitSeriesMapName_);
    GetParameter("InputMCEventHeader", inputEventHeaderName_);
    GetParameter("EventHeaderTimeCorrection", ehcorr);
    GetParameter("Hierarchy", params.hierarchy);
    GetParameter("sintheta13", params.sintheta13);
    GetParameter("sintheta23", params.sintheta23);
    GetParameter("sintheta12", params.sintheta12);
    GetParameter("AbsoluteStartTime", abstime);

    SNEventSelectionBaseClass::ConfigureTimeInterpolation(c_);

    oscweight = boost::shared_ptr<SNOscillationWeight>( new SNOscillationWeight(c_) );
}

void SNOscillations::DAQ(I3FramePtr frame)
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

    if (ehcorr)
    {
        I3EventHeaderConstPtr eventheader = frame->Get<I3EventHeaderConstPtr>(inputEventHeaderName_);
        starttime = eventheader->GetStartTime();
    }
    else
    {
        starttime = I3Time(0);
    }
    
    log_debug("Start time = %s", starttime.GetUTCString().c_str());
    
    /// Getting mactree and weightmap out of frame
    I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>(inputI3MCTreeName_);
    SNWeightMapConstPtr weightmap = frame->Get<SNWeightMapConstPtr>(inputSNWeightMapName_);
    
    SNWeightMapPtr newweightmap = SNWeightMapPtr(new SNWeightMap() );

    I3MCTree::sibling_iterator primit;
    for (primit = mctree->begin(); primit != mctree->end(); primit++)
    {
        SNWeight weights = (*weightmap).find(*primit)->second;
        
        weights.OscillationWeight = oscweight->GetOscillationWeight(params, *primit, starttime, abstime);
        
        log_debug("Osc weight = %f", weights.OscillationWeight);
        
        weights.TotalWeight = weights.InteractionWeight * weights.GenerationWeight * weights.OscillationWeight;
        weights.GenOscWeight = weights.GenerationWeight * weights.OscillationWeight; 
        
        newweightmap->insert( std::make_pair( *primit, weights ) );
                
    }
    
    frame->Delete(inputSNWeightMapName_);
    frame->Put(inputSNWeightMapName_, newweightmap);
    PushFrame(frame);
    
}