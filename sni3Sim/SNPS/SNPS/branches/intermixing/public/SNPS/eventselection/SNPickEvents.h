// #ifndef SNPICKEVENTS_H_INCLUDED
// #define SNPICKEVENTS_H_INCLUDED

// #include <SNPS/eventselection/SNEventSelectionBaseClass.h>
// #include <SNPS/weighting/SNWeightingBaseClass.h>
// #include <SNPS/weighting/SNGenerationWeight.h>
// #include <SNPS/timing/SNCorrectTime.h>

// #include <SNPS/HDFreaderwriter/inputfilereader.h>

// #include <SNPS/interactions/IBD.h>

// #include <icetray/I3Module.h>
// #include <icetray/I3Frame.h>

// #include <phys-services/I3RandomService.h>

// class SNPickEvents : public I3Module, public SNEventSelectionBaseClass
// {

// public:

//     SNPickEvents(const I3Context &ctx);

//     virtual ~SNPickEvents() {};

//     void Configure();

//     void Process();
// private:

//     double t;
//     int noFramesNeeded;
//     int noFramesPassed;

//     std::string model_;
//     std::string hierarchy_;
//     std::string shocktype_;
//     std::string interactions_;
//     std::string modelfile_;
//     std::string inputspectrum_;
//     std::string physicsspectrum_;
//     std::string inputI3MCTreeName_;
//     std::string inputSNWeightMapName_;
//     std::string inputMCHitSeriesMapName_;
//     std::string inputPhotonSeriesMapName_;
//     std::string inputEventHeaderName_;

//     double distance_;
//     double nopart_;
//     double radius_;
//     double height_;
//     double maxE_;
//     double t_;
//     double area_;
//     double oversamplingfactor_;

//     int noIFrames;
//     int noGFrames;
//     int noCFrames;
//     int noDFrames;
//     int noQFrames;
//     int noPFrames;

//     I3RandomServicePtr rng_;

//     SNGenerationWeight instancegenweight;

//     SNCorrectTime instancecortime;

//     SNPSBaseClass::modelintconfiguration c;

//     std::vector<double>::const_iterator vecitdt;


// };


// #endif // SNPICKEVENTS_H_INCLUDED


