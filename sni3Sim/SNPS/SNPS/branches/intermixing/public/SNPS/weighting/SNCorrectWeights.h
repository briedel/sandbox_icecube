// #ifndef SNREWEIGHT_H_INCLUDED
// #define SNREWEIGHT_H_INCLUDED

// #include <SNPS/weighting/SNInteractionWeight.h>
// #include <SNPS/injection/SNInjectionBaseClass.h>

// #include <SNPS/interactions/interaction.h>

// #include <icetray/I3Module.h>
// #include <icetray/I3Frame.h>

// class SNCorrectWeights : public I3Module
// {
// public:

//     SNCorrectWeights(const I3Context &ctx);;

//     virtual ~SNCorrectWeights() {};

//     void Configure();

//     void DAQ(I3FramePtr);

// private:

//     std::string inputSNWeightMapName_;

//     std::string inputI3MCTreeName_;

//     std::string outputSNWeightMapName_;

//     std::string interaction_;

//     boost::shared_ptr<SNInteractionWeight> intweight;

//     SNInjectionBaseClass::configuration c_;

// };

// #endif // SNREWEIGHT_H_INCLUDED