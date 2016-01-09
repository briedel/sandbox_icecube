// #include <icetray/load_project.h>
// #include <icetray/python/list_indexing_suite.hpp>
// #include <icetray/python/stream_to_string.hpp>

// #include <dataclasses/ostream_overloads.hpp>

// #include <SNPS/interactions/interaction.h>
// #include <SNPS/interactions/IBD.h>
// #include <SNPS/SNInjection.h>
// #include <SNPS/weighting/SNReweight.h>

// using namespace boost::python;


// void register_Interaction()
// {
//     class_<Interaction, boost::noncopyable>("Base", no_init);

//     class_<IBD, bases<Interaction> >("IBD")
//     .def("GetE1", &IBD::GetE1)
//     .def("GetE2", &IBD::GetE2)
//     .def("GetKE1", &IBD::GetKE1)
//     .def("GetKE2", &IBD::GetKE2)
//     .def("GetTotalCrossSection", &IBD::GetTotalCrossSection)
//     .def("GetDifferentialCrossSection", &IBD::GetDifferentialCrossSection)
//     .def("GetRandomEvent", &IBD::GetRandomEvent)
//     .def("GetMaximalCrossSection", &IBD::GetMaximalCrossSection);

// }