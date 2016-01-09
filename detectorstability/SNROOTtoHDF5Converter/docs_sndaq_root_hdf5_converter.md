# Documentation for converting SNDAQ ROOT files to a HDF5 Tables file

The script converts the original SNDAQ ROOT data files into a table format (HDF5) that is fairly language agnostic and standardized, while maintaining most of the original structure and data. It provides a means to access the SNDAQ output data without having a running copy of SNDAQ, having to understand the SNDAQ data format, or access the data without ROOT.

## Dependencies

The following packages are required to use the script, and should be provided by your favorite package manager:

* ROOT with PyROOT: https://root.cern.ch/
* hdf5: http://www.hdfgroup.org/HDF5/
* pytables: http://www.pytables.org/
* numpy: http://www.numpy.org/

## Input Data

The structure of the input files (https://wiki.icecube.wisc.edu/index.php/Data_Formats) is a table in which the columns are data objects and the rows are identified by a timestamp object and an index internal to the ROOT file. The data objects can contain their own sub-table. The sub-tables may have more than one of the respective objects in them. For example, a given timestamp can have up four `SN_ORoutEvent_t` objects associated with it. The internal index of the ROOT file will differiate the objects in the sub-tables.

The input data allows for variable length arrays to be stored in the data. This means that some of the input data can not be converted into a HDF5 table, mainly how many DOMs were re-activated or removed from the analysis. This information is not important at this time, as the analysis is assumed to be accurate. The variable length arrays also cause the position of various detectors in the data to shift over time or between runs, which requires some correcting, see sections below for details.

## Output Data

The output format exposes this internal mapping by storing the data in three separate tables:

* snall: Contains a timestamp and the associated scaler count for every DOM for that timestamp. 
* snrout: Contains a timestamp and the output of the online analysis.
* sncand: Contains additional information, i.e. inputs into the analysis method, for timestamps with analysis output above threshold

This approach creates some data redundancy. It does allow for a more easy access to, clearer organization, and better understanding of the data. Some improvements, mainly using HDF5 arrays to store data, could be made, but are not implemented at this time. 

## Converting SNDAQ ROOT Files to HDF5 Tables

The following instructions should get you to be able to run the conversion script. You will need two inputs:

* SNDAQ output file, either the ROOT or a tarfile with a ROOT file in it
* Geometry file (somewhat optional, see details below)

The SNDAQ output file is the original data container. The geometry file is a text file that is created from an object stored in the ROOT file. It cannot be created with this script, as it requires a running version of SNDAQ to extract. Most runs have a geometry file available. To run the code simply execute one of the following variations:

    ./sndaq_root_hdf5_converter.py <list of input files> -O <output directory> -g <geometry file>
    ./sndaq_root_hdf5_converter.py <list of input files> -o <output file> -g <geometry file>
    ./sndaq_root_hdf5_converter.py <list of input files> -o <output file> -G <geometry file directory>
    ./sndaq_root_hdf5_converter.py <list of input files> -O <output directory> -G <geometry file directory>
    
The code will now convert the given input files into a output file that will be stored either in the given `<output directory>` or as `<output file>`. In case `<output directory>` is given, the output file name is determined by the filename of the input file(s). To perform the conversion, the code will extract the individual objects by column, loop through the rows by index, and write the converted data the respective table. In case the column consists of a sub-table, ROOT will extend the index accordingly. For example, the `snall` column may have had 57000 entries and associated timestamps, while `snrout` will have 228000 total entries with 57000 associated timestamps. This will be reflected in the table as well, with certain timestamps having multiple outputs associated with them.

### Geometry file

If a geometry file is not given, a fairly good correction to the geometry can be performed using `correct_missing_doms`. By default this method is not used because it operates under the assumption that no new DOMs have dropped out of the detector and that only certain DOMs have dropped out so far. In case you are not interested in the noise rates of individual DOMs, you do not need a geometry file at all. In this case, one can simply use the data as is. 