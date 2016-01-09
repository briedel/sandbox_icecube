i3_project(sndaq
           PYTHON_DIR python
           # DOCS_DIR resources/docs
          )
           
i3_add_library(sndaq
               private/sndaq/I3SNDataclasses.cxx
               private/sndaq/I3SNFileReaderModule.cxx
               private/sndaq/I3SNTimeSyncBinner.cxx
               private/sndaq/I3SNHelpers.cxx
               USE_TOOLS boost python
               USE_PROJECTS icetray dataclasses interfaces phys-services
)

i3_add_pybindings(sndaq
    private/pybindings/I3SNDataclasses.cxx
    # private/pybindings/I3SNDataclassesMap.cxx
    private/pybindings/module.cxx
    USE_TOOLS boost python
    USE_PROJECTS icetray dataclasses
    )

# if(NOT EXISTS ${CMAKE_SOURCE_DIR}/sndaq/resources/geo/geo.i3.gz)
#     colormsg(RED   "+-- Can't find geometry file (geo.i3.gz) data file...")
#     colormsg(RED   "    (this file will need to be downloaded once)")
# else(NOT EXISTS ${CMAKE_SOURCE_DIR}/sndaq/resources/geo/geo.i3.gz)
#     colormsg(CYAN  "+-- Found geometry file (geo.i3.gz)")
# endif(NOT EXISTS ${CMAKE_SOURCE_DIR}/sndaq/resources/geo/geo.i3.gz)