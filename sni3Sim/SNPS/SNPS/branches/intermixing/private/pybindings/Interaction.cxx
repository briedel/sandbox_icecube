#include <icetray/python/list_indexing_suite.hpp>
#include <icetray/python/stream_to_string.hpp>

#include <dataclasses/ostream_overloads.hpp>

#include <SNPS/interactions/interaction.h>
#include <SNPS/interactions/IBD.h>
#include <SNPS/interactions/escatter.h>
#include <SNPS/interactions/escatter_flavor_selection.h>

using namespace boost::python;


void register_Interaction()
{
    class_<IBD>("IBD")
    .def("GetE1", &IBD::GetE1)
    .def("GetE2", &IBD::GetE2)
    .def("GetKE1", &IBD::GetKE1)
    .def("GetKE2", &IBD::GetKE2)
    .def("GetTotalCrossSection", &IBD::GetTotalCrossSection)
    .def("GetDifferentialCrossSection", &IBD::GetDifferentialCrossSection)
    .def("GetRandomEvent", &IBD::GetRandomEvent)
    .def("GetMaximalCrossSection", &IBD::GetMaximalCrossSection);

    class_<EScatter_neutrino_e>("EScatter_neutrino_e")
    .def("GetE1", &EScatter_neutrino_e::GetE1)
    .def("GetE2", &EScatter_neutrino_e::GetE2)
    .def("GetKE1", &EScatter_neutrino_e::GetKE1)
    .def("GetKE2", &EScatter_neutrino_e::GetKE2)
    .def("GetTotalCrossSection", &EScatter_neutrino_e::GetTotalCrossSection)
    .def("GetDifferentialCrossSection", &EScatter_neutrino_e::GetDifferentialCrossSection)
    .def("GetRandomEvent", &EScatter_neutrino_e::GetRandomEvent)
    .def("GetMaximalCrossSection", &EScatter_neutrino_e::GetMaximalCrossSection);

    class_<EScatter_antineutrino_e>("EScatter_antineutrino_e")
    .def("GetE1", &EScatter_antineutrino_e::GetE1)
    .def("GetE2", &EScatter_antineutrino_e::GetE2)
    .def("GetKE1", &EScatter_antineutrino_e::GetKE1)
    .def("GetKE2", &EScatter_antineutrino_e::GetKE2)
    .def("GetTotalCrossSection", &EScatter_antineutrino_e::GetTotalCrossSection)
    .def("GetDifferentialCrossSection", &EScatter_antineutrino_e::GetDifferentialCrossSection)
    .def("GetRandomEvent", &EScatter_antineutrino_e::GetRandomEvent)
    .def("GetMaximalCrossSection", &EScatter_antineutrino_e::GetMaximalCrossSection);

    class_<EScatter_neutrino_x>("EScatter_neutrino_x")
    .def("GetE1", &EScatter_neutrino_x::GetE1)
    .def("GetE2", &EScatter_neutrino_x::GetE2)
    .def("GetKE1", &EScatter_neutrino_x::GetKE1)
    .def("GetKE2", &EScatter_neutrino_x::GetKE2)
    .def("GetTotalCrossSection", &EScatter_neutrino_x::GetTotalCrossSection)
    .def("GetDifferentialCrossSection", &EScatter_neutrino_x::GetDifferentialCrossSection)
    .def("GetRandomEvent", &EScatter_neutrino_x::GetRandomEvent)
    .def("GetMaximalCrossSection", &EScatter_neutrino_x::GetMaximalCrossSection);

    class_<EScatter_antineutrino_x>("EScatter_antineutrino_x")
    .def("GetE1", &EScatter_antineutrino_x::GetE1)
    .def("GetE2", &EScatter_antineutrino_x::GetE2)
    .def("GetKE1", &EScatter_antineutrino_x::GetKE1)
    .def("GetKE2", &EScatter_antineutrino_x::GetKE2)
    .def("GetTotalCrossSection", &EScatter_antineutrino_x::GetTotalCrossSection)
    .def("GetDifferentialCrossSection", &EScatter_antineutrino_x::GetDifferentialCrossSection)
    .def("GetRandomEvent", &EScatter_antineutrino_x::GetRandomEvent)
    .def("GetMaximalCrossSection", &EScatter_antineutrino_x::GetMaximalCrossSection);
}