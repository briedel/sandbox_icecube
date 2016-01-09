i3_project(SNPS
           PYTHON_DIR python)

SET(LIB_${PROJECT_NAME}_SOURCEFILES private/SNPS/*.cxx)
SET(LIB_${PROJECT_NAME}_PROJECTS icetray dataclasses phys-services clsim sim-services simclasses tableio hdfwriter)
SET(LIB_${PROJECT_NAME}_TOOLS boost python gsl hdf5)

i3_add_library(SNPS
               private/SNPS/baseclass/SNPSBaseClass.cxx
               private/SNPS/eventselection/SNEventSelectionBaseClass.cxx
               private/SNPS/eventselection/SNPickEventsQueue.cxx
               private/SNPS/eventselection/SNPickEvents.cxx
               private/SNPS/HDFreaderwriter/inputfilereader.cxx
               private/SNPS/HDFreaderwriter/I3FileBooking/SNPSDataCollector.cxx
               private/SNPS/HDFreaderwriter/I3FileBooking/SNPSHitRecord.cxx
               private/SNPS/HDFreaderwriter/I3FileBooking/converter/SNPSConverter.cxx
               private/SNPS/helpers/frameobjectcombiners.cxx
               private/SNPS/helpers/frameobjecthelpers.cxx
               private/SNPS/helpers/frameobjects.cxx
               private/SNPS/helpers/gsl_interp_wrapper.cxx
               private/SNPS/helpers/integrationhelpers.cxx
               private/SNPS/helpers/interp_gen.cxx
               private/SNPS/helpers/SNFrameobjectCombiner.cxx
               private/SNPS/injection/SNFilePruner.cxx
               private/SNPS/injection/SNInjection.cxx
               private/SNPS/injection/SNInjectionBaseClass.cxx
               private/SNPS/interactions/interaction.cxx
               private/SNPS/interactions/IBD.cxx
               private/SNPS/interactions/Ointeraction.cxx
               private/SNPS/interactions/escatter.cxx
               private/SNPS/interactions/escatter_flavor_selection.cxx
               private/SNPS/intermixing/framemerger.cxx
               private/SNPS/oscillation/SNOscillations.cxx
               private/SNPS/timing/SNCorrectTime.cxx
               private/SNPS/weighting/SNGenerationWeight.cxx
               private/SNPS/weighting/SNGenerationWeightHelpers.cxx
               private/SNPS/weighting/SNInteractionWeight.cxx
               private/SNPS/weighting/SNWeightingBaseClass.cxx
               private/SNPS/weighting/SNOscillationWeight.cxx
               private/SNPS/merging/SNFrameMerger.cxx
               # private/SNPS/spectra
               # private/SNPS/*.cxx
               # private/SNPS/*/*.cxx
               # private/SNPS/HDFreaderwriter/I3FileBooking/*.cxx
               # private/SNPS/HDFreaderwriter/I3FileBooking/converter/*.cxx
               USE_TOOLS boost python gsl hdf5
               USE_PROJECTS icetray dataclasses phys-services clsim sim-services simclasses tableio hdfwriter)


#i3_executable(hdf5inout private/SNPS/HDFreaderwriter/*.cxx
#              USE_PROJECTS SNPS
#              USE_TOOLS hdf5)

#i3_executable(intetest private/SNPS/helpers/integrationhelpers.cxx
#              USE_PROJECTS SNPS
#              USE_TOOLS boost gsl)

add_subdirectory(private/pybindings)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/SNPS/resources/supernovadata/supernovaspectra.h5)
    colormsg(RED   "+-- Downloading supernovaspectra.h5 data file...")
    colormsg(RED   "    (this file will only be downloaded once)")
    EXECUTE_PROCESS(COMMAND ${CMAKE_SOURCE_DIR}/SNPS/resources/make/download_spectra.sh ${CMAKE_SOURCE_DIR}/SNPS/resources)
    colormsg(GREEN "+-- ...download complete.")
else(NOT EXISTS ${CMAKE_SOURCE_DIR}/SNPS/resources/supernovadata/supernovaspectra.h5)
    colormsg(CYAN  "+-- supernovaspectra.h5 already downloaded")
endif(NOT EXISTS ${CMAKE_SOURCE_DIR}/SNPS/resources/supernovadata/supernovaspectra.h5)
