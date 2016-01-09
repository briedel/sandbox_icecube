// #ifndef SNREWEIGHT_H_INCLUDED
// #define SNREWEIGHT_H_INCLUDED

// #define GRANULAR_GEOMETRY_SUPPORT

// #include <icetray/I3Frame.h>
// #include <icetray/I3Units.h>
// #include <icetray/I3Module.h>
// #include <icetray/I3TrayInfo.h>
// #include <icetray/I3TrayInfoService.h>
// #include <icetray/I3Configuration.h>

// #include <SNPS/eventselection/SNEventSelectionBaseClass.h>
// #include <SNPS/weighting/SNWeightingBaseClass.h>
// #include <SNPS/weighting/SNGenerationWeight.h>
// #include <SNPS/timing/SNCorrectTime.h>

// #include <phys-services/I3RandomService.h>

// #include <clsim/I3Photon.h>

// // #include <queue>

// class SNReweight : public I3Module, public SNEventSelectionBaseClass
// {

// public:

//     SNReweight(const I3Context &ctx);

//     virtual ~SNReweight() {};

//     void Configure();

//     /**
//      * Instead a TrayInfo, Geometry, DetectorStatus, Calibration, Physics, or
//      * DAQ method we use the Process method to work on the frames. For a given
//      * model and time bin calculate the number of Q frames needed. For now we
//      * pass just one I, G, D, C to the writer and drops the remainder.
//      *
//      * For Q frame: Check for frame objects being present. Count frame. If we
//      * have not accumulated enough frames put a pointer into a vector. If we
//      * have accumulated enough frames we generate a new frame with new
//      * frame objects to accumulate the data into. Push resulting super-frame to
//      * writer
//      *
//      * For P frame:
//      * LOG_FATAL
//      *
//      * NOTE: Does not take a frame, but you need to pop the frame for Process()
//      *
//      * TODO: Work on TrayInfo Frame to get information about the frame rather
//      *       than passing it to the module.
//      *       What to get out?
//      *       * Injection volume parameters - Radius, height, center
//      *       * Generation spectrum and parameters
//      *       * Maximum energy 
//      *       * Interaction type
//      * TODO: Add changes for neutrino flavor for different interactions
//      * TODO: Support for changing GCD?
//      */
//     void Process();

//     /**
//      * Function to concatenate the required number of frames into one large
//      * frame. Add together MCTree, MCHitSeriesMap, and PhotonSeriesMap. And
//      * Correct time for new frame objects
//      *
//      * @param  Vector of pointers to frames
//      * @param  Struct of new frame objects
//      */
//     // void ConcatFrameobjectsApplyWeightsTimes( std::queue<I3FramePtr> &, newframeobjects & );

//     /**
//      * Function to change time of particles, hit, and photon to be within the
//      * selected time bin.
//      *
//      * @param Struct of filled new frame objects
//      */
//     // void CorrectTime(newframeobjects &);

// private:

//     double t;
//     unsigned int noFramesNeeded;
//     unsigned int noFramesPassed;

//     std::string model_;
//     std::string hierarchy_;
//     std::string shocktype_;
//     std::string interactions_;
//     std::string modelfile_;
//     std::string samplefile_;
//     std::string inputspectrum_;
//     std::string physicsspectrum_;
//     std::string inputI3MCTreeName_;
//     std::string inputSNWeightMapName_;
//     std::string inputMCHitSeriesMapName_;
//     std::string inputPhotonSeriesMapName_;
//     std::string inputEventHeaderName_;

//     std::pair<double, double> startendtime_;
    
//     double distance_;
//     double garchingmeanE_;
//     double garchingalpha_;
//     double gammameanE_;
//     double gammaalpha_;
//     double nopart_;
//     double radius_;
//     double height_;
//     double maxE_;
//     double deltat_;
//     double t_;
//     double area_;
//     double oversamplingfactor_;


//     int noIFrames;
//     int noGFrames;
//     int noCFrames;
//     int noDFrames;
//     int noQFrames;
//     int noPFrames;

//     // std::queue<I3FramePtr> frameList_;
    
//     std::vector<double> dtalpha;
//     std::vector<double> dtLandmE;

//     std::vector<double>::const_iterator itta;
//     std::vector<double>::const_iterator ittLmE;

//     std::vector<double>::const_iterator itdta;
//     std::vector<double>::const_iterator itdtLmE;

//     I3RandomServicePtr rng_;

//     /// GSL interpolation wrappers wrappers
//     gsl_linear_wrapper *Lnue_cspline_ptr;
//     gsl_linear_wrapper *Lnuebar_cspline_ptr;
//     gsl_linear_wrapper *Lnux_cspline_ptr;
//     gsl_linear_wrapper *mEnue_cspline_ptr;
//     gsl_linear_wrapper *mEnuebar_cspline_ptr;
//     gsl_linear_wrapper *mEnux_cspline_ptr;
//     gsl_linear_wrapper *anue_cspline_ptr;
//     gsl_linear_wrapper *anuebar_cspline_ptr;
//     gsl_linear_wrapper *anux_cspline_ptr;

//     InputFileReader::DataModel d;
//     InputFileReader::DataShock ds;

//     // SNWeightingBaseClass::genweigtinfo gwi;

//     // SNWeightingBaseClass::newframeobjects nfo;

//     // frameobjecthelpers instanceFOH;

//     SNGenerationWeight instancegenweight;

//     SNCorrectTime instancecortime;

// };

// #endif //SNREWEIGHT_H_INCLUDED
