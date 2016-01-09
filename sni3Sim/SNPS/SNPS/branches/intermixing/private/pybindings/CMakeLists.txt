i3_add_pybindings(SNPS
    SNWeightingHelpers.cxx
    Interaction.cxx
    SNPSFileFilter.cxx
    framemerger.cxx
    module.cxx
    USE_TOOLS boost python
    USE_PROJECTS icetray dataclasses phys-services clsim sim-services tableio SNPS
    )
