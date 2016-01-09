// #ifndef SNPICKEVENTSPARALLEL_H_INCLUDED
// #define SNPICKEVENTSPARALLEL_H_INCLUDED

// #include <SNPS/eventselection/SNEventSelectionBaseClass.h>
// #include <SNPS/weighting/SNWeightingBaseClass.h>
// #include <SNPS/weighting/SNGenerationWeight.h>
// #include <SNPS/timing/SNCorrectTime.h>

// #include <SNPS/HDFreaderwriter/inputfilereader.h>

// #include <SNPS/interactions/IBD.h>

// #include <icetray/I3Module.h>
// #include <icetray/I3Frame.h>

// #include <phys-services/I3RandomService.h>

// class SNPickEventsParallel : public I3Module, public SNEventSelectionBaseClass
// {

// public:

//     SNPickEventsParallel(const I3Context &ctx);

//     virtual ~SNPickEventsParallel() {};

//     void Configure();

//     void DAQ(I3FramePtr);
// private:

//     boost::shared_ptr<gsl_linear_wrapper> Lnue_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> Lnuebar_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> Lnux_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> mEnue_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> mEnuebar_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> mEnux_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> anue_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> anuebar_cspline_ptr;
//     boost::shared_ptr<gsl_linear_wrapper> anux_cspline_ptr;

//     int noFramesNeeded;
//     int noFramesPassed;
//     int noFramesSkip;

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

//     std::pair<double, double> startendtime_;

//     double distance_;
//     double nopart_;
//     double radius_;
//     double height_;
//     double maxE_;
//     double t_;
//     double dt_;
//     double area_;
//     double oversamplingfactor_;

//     int noIFrames;
//     int noGFrames;
//     int noCFrames;
//     int noDFrames;
//     int noQFrames;
//     int noPFrames;


//     I3RandomServicePtr rng_;

//     InputFileReader::DataModel d;
//     InputFileReader::DataShock ds;

//     SNGenerationWeight instancegenweight;

//     SNCorrectTime instancecortime;

//     std::vector<double>::const_iterator vecitdt;

// };


// #endif // SNPICKEVENTSPARALLEL_H_INCLUDED


