#include <icetray/python/list_indexing_suite.hpp>
#include <icetray/python/stream_to_string.hpp>

#include <dataclasses/ostream_overloads.hpp>

#include <SNPS/weighting/SNGenerationWeight.h>

#include <SNPS/weighting/SNGenerationWeightHelpers.h>

using namespace boost::python;

void register_SNGenerationWeight()
{
    class_<SNGenerationWeight>("SNGenerationWeight")
    .def("GetTotalNoNeutrinos", &SNGenerationWeight::GetTotalNoNeutrinos)
    .def("GetTotalNoInteractedNeutrinos", &SNGenerationWeight::GetTotalNoInteractedNeutrinos)
    .def("GetIntegralCrossSectionSpectrum", &SNGenerationWeight::GetIntegralCrossSectionSpectrum)
    .def("GetWeightedPhysProb", &SNGenerationWeight::GetWeightedPhysProb)
    .def("GetWeightedPhysSpecValue", &SNGenerationWeight::GetWeightedPhysSpecValue)
    .def("GetGenArea", &SNGenerationWeight::GetGenArea)
    .def("GetPhysicsProb", &SNGenerationWeight::GetPhysicsProb)
    .def("GetGenProb", &SNGenerationWeight::GetGenProb)
    .def("GetEmin", &SNGenerationWeight::GetEmin);


}

void register_SNGenerationWeightHelpers()
{
    class_<SNGenerationWeightHelpers>("SNGenerationWeightHelpers")
    .def("GetTotalNumInteractedNeutrinoModel", &SNGenerationWeightHelpers::GetTotalNumInteractedNeutrinoModel)
    .def("GetTotalNumInteractedNeutrinoTimeBin", &SNGenerationWeightHelpers::GetTotalNumInteractedNeutrinoTimeBin);
}
