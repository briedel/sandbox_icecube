#!/usr/bin/env python
import sys
import os
import tarfile
import shutil
import ROOT
import tables
import numpy as np
import re
import warnings
from optparse import OptionParser

# Creating the description of the HDF5 tables for the different 
# groups inside the ROOT files. 
class sncandtabledesc(tables.IsDescription):
    """
    Class to describe the format of the SNDAQ candidate table
    candidates being events over threshold significance (6)
    """
    gps_time = tables.UInt64Col()
    base_time = tables.UInt32Col()
    NMAvBins = tables.Int32Col()
    NRebins = tables.Int32Col()
    Signal = tables.Float64Col()
    SignalError = tables.Float64Col()
    Chi2 = tables.Float64Col()
    ActiveChannels = tables.Int32Col()
    RebinIndex = tables.Int32Col()

    #################################################
    # Stuff that doesn't work at the moment
    # NNewActOMs = tables.Int32Col()
    # NewActOMs = tables.Int32Col(shape = (5160,))
    # NNewInactOMs = tables.Int32Col()
    # NewInactOMs = tables.Int32Col(shape = (5160,))

    NCandidate = tables.Int32Col()
    NChannels = tables.Int32Col()
    NEvent = tables.Int32Col()
    # The detector has 5160 DOMs. The maximum size of the data array is therefore 5160
    ChannelRate = tables.Float64Col(shape = (5160,))
    ChannelMean = tables.Float64Col(shape = (5160,))
    ChannelSigmaSquare = tables.Float64Col(shape = (5160,))
    ChannelWeighedDev = tables.Float64Col(shape = (5160,))

class snrouttabledesc(tables.IsDescription):
    """
    Class to describe the format of the SNDAQ analysis 
    class output table
    """
    gps_time = tables.UInt64Col()
    base_time = tables.UInt32Col()
    NMAvBins = tables.Int32Col()
    NRebins = tables.Int32Col()
    Signal = tables.Float64Col()
    SignalError = tables.Float64Col()
    Chi2 = tables.Float64Col()
    ActiveChannels = tables.Int32Col()
    RebinIndex = tables.Int32Col()

    #################################################
    # Stuff that doesn't work at the moment
    # NNewActOMs = tables.Int32Col()
    # NewActOMs = tables.Int32Col(shape = (5160,))
    # NNewInactOMs = tables.Int32Col()
    # NewInactOMs = tables.Int32Col(shape = (5160,))

class snalltabledesc(tables.IsDescription):
    """
    Class to describe the format of the SNDAQ all table 
    Contains the scalers in 500 ms bins
    """
    gps_time = tables.UInt64Col()
    # The detector has 5160 DOMs. The maximum size of the data array is therefore 5160
    counts = tables.Int32Col(shape = (5160,))

# class snomwatchtabledesc(tables.IsDescription):
#     """
#     IGNORE
#     Class to describe the format of the SNDAQ DOM monitoring table
#     Not in data warehousev files.
#     NEEDS DEBUGGING
#     """
#     gps_time = tables.UInt64Col()
#     RebinWidth = tables.Int32Col()
#     OMSNChannel = tables.Int32Col()
#     OMId = tables.UInt64Col()
#     OMMean = tables.Float64Col()
#     OMSigma = tables.Float64Col()
#     OMFano = tables.Float64Col()
#     OMSkew = tables.Float64Col()
#     OMKurtosis = tables.Float64Col()
#     OMGaussSigma = tables.Float64Col()
#     OMGaussRedChi2 = tables.Float64Col()
#     OMGaussNDF = tables.Int32Col()

class SNDAQROOTtoHDF5Converter(object):
    def __init__(self, options, args):
        self.options = args
        self.args = args
        
    def create_out_file(self, outfn):
        """
        Creating the output HDF5 file
    
        :param outfn: Output file name
        """
        self.h5file = tables.open_file(outfn, "w")
    
        # Setting highest possible compression level
        filtercomp = tables.Filters(complevel = 9) 
    
        # Creating the necessary tables
        self.sncandtable = h5file.createTable("/", 'sncand', sncandtabledesc, "SN candidates", filters = filtercomp)
        self.sncandrow = sncandtable.row

        self.snrouttable = h5file.createTable("/", 'snrout', snrouttabledesc, "SN Analysis Class Results", filters = filtercomp)
        self.snroutrow = snrouttable.row

        self.snalltable = h5file.createTable("/", 'snall', snalltabledesc, "500 ms of DOM ", filters = filtercomp)
        self.snallrow = snalltable.row

        # self.snomwatchtable = h5file.createTable("/", 'snomwatch', snomwatchtabledesc, "Monitoring quantities for channels ")
        # self.snomwatchrow = snomwatchtable.row
    
    def get_geo_mapping_filename(self, runnum, filenum, mapdir = None, mapname = None):
        """
        Method to find the correct geometry file for the 
        run depending on the run number and file number for 
        that run
    
        :param runnum: Run number being processed
        :param filenum: Which file number of this run is being processed
        :param mapdir: Directory with geometry files
        :param mapname: Location of a geometry
        :returns: Location of geometry file
        """
        if mapname:
            try:
                check_file_exist_open(mapname)
                return mapname
            except:
                try:
                    check_file_exist_open(os.path.join(mapdir, mapname))
                    return os.path.join(mapdir, mapname)
                except:
                    raise RuntimeError("Could not locate file: %s or %s" % (mapname, os.path.join(mapdir, mapname)))
        else:
            mapname = "reduced_try2_DOM_mapping_%d_%s.txt" % (runnum, filenum)
            mapfullpath = os.path.join(mapdir, mapname)
            if check_file_exist_open(mapfullpath):
                return mapfullpath
            else:
                mapname = "reduced_try2_DOM_mapping_%d_%03d.txt" % (runnum, filenum)
                mapfullpath = os.path.join(mapdir, mapname)
                if check_file_exist_open(mapfullpath):
                    return mapfullpath
                    
    def get_daq_time_as_long(self, gps_time_high, gps_time_low):
        """
        Some bit shifting to create a int64 out of two int32
    
        ROOT querk: 
        ROOT files cannot store 64 bit integers, so they are split apart
    
        :param gps_time_high: 4 high bytes of time stamp
        :param gps_time_low: 4 low bytes of time stamp
        :returns: time as int64
        """
        return (gps_time_high << 32) + gps_time_low

    def correct_missing_doms(self, scalerarray, no_channels):
        """ 
        Backup method in case geometry is not given. 
        Very back-of-the-envelope.
        Not used at the moment.
    
        Correcting an artifact of storing variable length arrays in a table.
    
        Changes to the SNDAQ geometry removed certain DOMs from the snall 
        data array, so putting them back into the array at the right location.
        Need to remove last 7 or 8 dummy entries produced when reading data from file
        and insert zeros at appropriate places in array.

        :param scalerarray: Scaler array missing DOMs shifted to end
        :param no_channels: Number of active channels assumed for the file
        :returns: Scaler array with the correct location mapping
        """
        if no_channels == 5153:
            return np.insert(scalerarray[:-7], 
                             [45, 403, 1308, 1925, 2278, 3594, 4061], 0)
        elif no_channels == 5152:
            return np.insert(scalerarray[:-8],
                             [45, 403, 1308, 1925, 2278, 3594, 4061, 5069], 0)
        else:
            raise RuntimeError("No. of channels (= %d) is not support" % no_channels)

    def geo_map_data(self, data, geomap):
        """
        Correcting for geometry shifts that are caused by storing
        the data in variable length arrays depending on the run.
    
        :param data: Scaler array missing DOMs shifted to end
        :param geomap: Dictionary that maps the location in the ROOT file's array to a DOM
        :returns: Scaler array with the correct location mapping
        """
        newdata = np.zeros(5160, dtype = np.int32)
        for p in geomap.keys(): 
            newdata[convert_omkey_domnum(geomap[p])] = data[int(p)]
        return newdata

    def set_branches(self, snobject, snmemory, names):
        """
        Automate setting pointers in to right memory locations of a given variables
        by name
    
        :param snobject: Pointer to scaler data object (snall) in ROOT file
        :param snmemory: Memory allocation object where the data will be put by ROOT
        :param names: Names of variables to be located
        """
        for name in names:
            snobject.SetBranchAddress(name, ROOT.AddressOf(snmemory, name))

    def extract_sn_all(self, snall, snallrow, geomap):
        """
        Extract snall tree out of file
    
        SNDAQ/ROOT querk: 
        ROOT does not understand the SNDAQ ROOT files. The CINT dicts that are needed
        are not written into the file. SNDAQ creates normal ROOT trees, branches, leaves,
        but does not write the structure of these into the file. Hence ROOT will not understand
        them without the proper definitions. You have to tell ROOT what the structure is supposed
        and where in memory to address it. 
        Thanks to Lutz for the inspiration.
    
        :param snall: Pointer to scaler data object (snall) in ROOT file
        :param snallrow: Pointer to HDF5 row object that referes to a row in the table 
                          we are writing to
        :param geomap: Dictionary that maps the location in the ROOT file's array to a DOM
        """

        # Allocating memory
        snallstreaming = ROOT.streaming_t()
        snallvar = ROOT.all_t()

        # Setting pointers in to right memory locations
        snall.SetMakeClass(1)
        set_branches(snall, snallstreaming, 
                     ["base_time", "gps_time_high", "gps_time_low"])
        set_branches(snall, snallvar, 
                     ["no_channels", "data"])
    
        # General description for the loops:
        # Since we dont know the structure of the file, ROOT does not allow us to loop 
        # through the records directly. We have to tell it which record to look at by 
        # number. Not sure how this works internally, lets just say the desired result happens.
        for i in range(snall.GetEntries()):
            # Getting data by index
            snall.GetEntry(i)

            # Getting correct time
            time = get_daq_time_as_long(snallstreaming.gps_time_high, 
                                        snallstreaming.gps_time_low)

            # The detector has 5160 DOMs. The maximum size of the data array is therefore 5160
            data = np.array([snallvar.data[j] for j in range(5160)])
        
            # Correcting for the geometry of the detector
            data =  geo_map_data(data, geomap)
        
            # Putting things into the row
            snallrow["gps_time"] = time
            snallrow["counts"] = data
            snallrow.append()

    def extract_sn_rout(self, snrout, snroutrow):
        """
        Extract snrout tree from file
    
        SNDAQ/ROOT querk: 
        ROOT does not understand the SNDAQ ROOT files. The CINT dicts that are needed
        are not written into the file. SNDAQ creates normal ROOT trees, branches, leaves,
        but does not write the structure of these into the file. Hence ROOT will not understand
        them without the proper definitions. You have to tell ROOT what the structure is supposed
        and where in memory to address it. 
        Thanks to Lutz for the inspiration.
    
        :param snrout: Pointer to analysis output (snrout) data object in ROOT file
        :param snroutrow: Pointer to HDF5 row object that referes to a row in the table 
                          we are writing to
        """

        # Allocating memory
        snroutstreaming = ROOT.streaming_t()
        snroutvar = ROOT.rout_t()

        # Setting pointers in to right memory locations
        snrout.SetMakeClass(1)
        set_branches(snrout, snroutstreaming, 
                     ["base_time", "gps_time_high", "gps_time_low"])
        set_branches(snrout, snroutvar, 
                     ["NMAvBins", "NRebins", "Signal", "SignalError",
                      "Chi2", "ActiveChannels", "RebinIndex",
                      "NNewActOMs", "NewActOMs", "NNewInactOMs", "NewInactOMs"])

        # General description for the loops:
        # Since we dont know the structure of the file, ROOT does not allow us to loop 
        # through the records directly. We have to tell it which record to look at by 
        # number. Not sure how this works internally, lets just say the desired result happens.
        for i in range(snrout.GetEntries()):
            # Getting data by index
            snrout.GetEntry(i)

            # Getting correct time
            time = get_daq_time_as_long(snroutstreaming.gps_time_high, 
                                        snroutstreaming.gps_time_low)
            # Putting things into the row
            snroutrow["gps_time"] = time
            #Correction to get the right bin size. Note from GK
            snroutrow["base_time"] = snroutvar.NRebins * 500 
            snroutrow["NMAvBins"] = snroutvar.NMAvBins
            snroutrow["NRebins"] = snroutvar.NRebins
            snroutrow["Signal"] = snroutvar.Signal
            snroutrow["SignalError"] = snroutvar.SignalError
            snroutrow["Chi2"] = snroutvar.Chi2
            snroutrow["ActiveChannels"] = snroutvar.ActiveChannels
            snroutrow["RebinIndex"] = snroutvar.RebinIndex
        

            ##########################################################
            # Stuff that doesn't work at the moment
            # Prototype code
            # This attempts to get the number of new active DOMs 
            # out of the file. The problem is again variable
            # length arrays. One can not save a variable length
            # array to an HDF5 file without some contortions. 
            # We don't need the information right now, so ignore it.
            # We still need to tell ROOT that the data is there
            # else we will reading in the wrong data
            ##########################################################
        
            # snroutrow["NNewActOMs"] = snroutvar.NNewActOMs
            # newDOMs = np.array([snroutvar.NewActOMs[j] for j in range(snroutvar.NNewActOMs)])
            # for j in range(len(newDOMs)): NewActOMs[j] = newDOMs[j]
            # snroutrow["NewActOMs"] = NewActOMs
        
            # snroutrow["NNewInactOMs"] = snroutvar.NNewInactOMs
            # removedDOMs = np.array([snroutvar.NewInactOMs[j] for j in range(snroutvar.NNewInactOMs)])
            # for j in range(len(removedDOMs)): NewInactOMs[j] = removedDOMs[j]
            # snroutrow["NewInactOMs"] = NewInactOMs

            snroutrow.append()

    def extract_sn_cand(self, sncand, sncandrow):
        """
        Extract sncand tree from file
    
        SNDAQ/ROOT querk: 
        ROOT does not understand the SNDAQ ROOT files. The CINT dicts that are needed
        are not written into the file. SNDAQ creates normal ROOT trees, branches, leaves,
        but does not write the structure of these into the file. Hence ROOT will not understand
        them without the proper definitions. You have to tell ROOT what the structure is supposed
        and where in memory to address it. 
        Thanks to Lutz for the inspiration.
    
        :param sncand: Pointer to supernova candidate object (sncand) in ROOT file
        :param snroutrow: Pointer to HDF5 row object that referes to a row in the table 
                          we are writing to
        """

        # Allocating memory
        sncandstreaming = ROOT.streaming_t()
        sncandroutvar = ROOT.rout_t()
        sncandvar = ROOT.cand_t()

        # Setting pointers in to right memory locations
        sncand.SetMakeClass(1)
        set_branches(sncand, sncandstreaming,
                     ["base_time", "gps_time_high", "gps_time_low"])
        set_branches(sncand, sncandroutvar,
                     ["NMAvBins", "NRebins", "Signal", "SignalError",
                      "Chi2", "ActiveChannels", "RebinIndex",
                      "NNewActOMs", "NewActOMs", "NNewInactOMs", "NewInactOMs"])
        set_branches(sncand, sncandvar,
                     ["NCandidate", "NChannels", "NEvent", "ChannelRate",
                      "ChannelMean", "ChannelSigmaSquare", "ChannelWeighedDev"])

        # General description for the loops:
        # Since we dont know the structure of the file, ROOT does not allow us to loop 
        # through the records directly. We have to tell it which record to look at by 
        # number. Not sure how this works internally, lets just say the desired result happens.
        for i in range(sncand.GetEntries()):
            # Getting data by index
            sncand.GetEntry(i)

            time = get_daq_time_as_long(sncandstreaming.gps_time_high, 
                                        sncandstreaming.gps_time_low)

            sncandrow["gps_time"] = time
            # Correction to get the right bin size. Note from GK
            sncandrow["base_time"] = sncandroutvar.NRebins * 500 
            sncandrow["NMAvBins"] = sncandroutvar.NMAvBins
            sncandrow["NRebins"] = sncandroutvar.NRebins
            sncandrow["Signal"] = sncandroutvar.Signal
            sncandrow["SignalError"] = sncandroutvar.SignalError
            sncandrow["Chi2"] = sncandroutvar.Chi2
            sncandrow["ActiveChannels"] = sncandroutvar.ActiveChannels
            sncandrow["RebinIndex"] = sncandroutvar.RebinIndex
            sncandrow["NCandidate"] = sncandvar.NCandidate
            sncandrow["NChannels"] = sncandvar.NChannels
            sncandrow["NEvent"] = sncandvar.NEvent
            # The detector has 5160 DOMs. The maximum size of the data array is therefore 5160
            sncandrow["ChannelRate"] = np.array([sncandvar.ChannelRate[j] for j in range(5160)])
            sncandrow["ChannelMean"] = np.array([sncandvar.ChannelMean[j] for j in range(5160)])
            sncandrow["ChannelSigmaSquare"] = np.array([sncandvar.ChannelSigmaSquare[j] for j in range(5160)])
            sncandrow["ChannelWeighedDev"] = np.array([sncandvar.ChannelWeighedDev[j] for j in range(5160)])
        
            ##########################################################
            # Stuff that doesn't work at the moment
            # Prototype code
            # This attempts to get the number of new active DOMs 
            # out of the file. The problem is again variable
            # length arrays. One can not save a variable length
            # array to an HDF5 file without some contortions. 
            # We don't need the information right now, so ignore it.
            # We still need to tell ROOT that the data is there
            # else we will reading in the wrong data
            ##########################################################

            # sncandrow["NNewActOMs"] = sncandroutvar.NNewActOMs
            # newDOMs = np.array([sncandroutvar.NewActOMs[j] for j in range(sncandroutvar.NNewActOMs)])
            # for j in range(len(newDOMs)): NewActOMs[j] = newDOMs[j]
            # sncandrow["NewActOMs"] = NewActOMs
        
            # sncandrow["NNewInactOMs"] = sncandroutvar.NNewInactOMs
            # removedDOMs = np.array([sncandroutvar.NewInactOMs[j] for j in range(sncandroutvar.NNewInactOMs)])
            # for j in range(len(removedDOMs)): NewInactOMs[j] = removedDOMs[j]
            # sncandrow["NewInactOMs"] = NewInactOMs
        
            sncandrow.append()

    # def get_sn_om_watch(self, snomwatch, snomwatchrow):
    #     """
    #     IGNORE
    #     Extract snomwatch tree from file
    #     PROTOTYPE CODE
    #     NEEDS DEBUGGING
    #
    #     Typically this data is not written until later in Mainz, so it isn't being extracted
    #     at the moment
    #
    #     SNDAQ/ROOT querk:
    #     ROOT does not understand the SNDAQ ROOT files. The CINT dicts that are needed
    #     are not written into the file. SNDAQ creates normal ROOT trees, branches, leaves,
    #     but does not write the structure of these into the file. Hence ROOT will not understand
    #     them without the proper definitions. You have to tell ROOT what the structure is supposed
    #     and where in memory to address it.
    #     Thanks to Lutz for the inspiration.
    #
    #     :param snomwatch: Pointer to supernova DOM monitoring object (snomwatch) in ROOT file
    #     :param snomwatchrow: Pointer to HDF5 row object that referes to a row in the table
    #                       we are writing to
    #     """
    #
    #     Allocating memory
    #     snomwatchstreaming = ROOT.streaming_t()
    #     snomwatchvar = ROOT.omwatch_t()
    #
    #     Setting pointers in to right memory locations
    #     snomwatch.SetMakeClass(1)
    #     snomwatch.SetBranchAddress("base_time", ROOT.AddressOf(snomwatchstreaming, 'base_time') )
    #     snomwatch.SetBranchAddress("gps_time_high", ROOT.AddressOf(snomwatchstreaming, 'gps_time_high') )
    #     snomwatch.SetBranchAddress("gps_time_low", ROOT.AddressOf(snomwatchstreaming, 'gps_time_low') )
    #     snomwatch.SetBranchAddress("buf_pos", ROOT.AddressOf(snomwatchstreaming, 'buf_pos') )
    #     snomwatch.SetBranchAddress("buf_loop", ROOT.AddressOf(snomwatchstreaming, 'buf_loop') )
    #     snomwatch.SetBranchAddress("RebinWidth", ROOT.AddressOf(snomwatchvar, 'RebinWidth') )
    #     snomwatch.SetBranchAddress("OMSNChannel", ROOT.AddressOf(snomwatchvar, 'OMSNChannel') )
    #     snomwatch.SetBranchAddress("OMId", ROOT.AddressOf(snomwatchvar, 'OMId') )
    #     snomwatch.SetBranchAddress("OMMean", ROOT.AddressOf(snomwatchvar, 'OMMean') )
    #     snomwatch.SetBranchAddress("OMSigma", ROOT.AddressOf(snomwatchvar, 'OMSigma') )
    #     snomwatch.SetBranchAddress("OMFano", ROOT.AddressOf(snomwatchvar, 'OMFano') )
    #     snomwatch.SetBranchAddress("OMSkew", ROOT.AddressOf(snomwatchvar, 'OMSkew') )
    #     snomwatch.SetBranchAddress("OMKurtosis", ROOT.AddressOf(snomwatchvar, 'OMKurtosis') )
    #     snomwatch.SetBranchAddress("OMGaussSigma", ROOT.AddressOf(snomwatchvar, 'OMGaussSigma') )
    #     snomwatch.SetBranchAddress("OMGaussRedChi2", ROOT.AddressOf(snomwatchvar, 'OMGaussRedChi2') )
    #     snomwatch.SetBranchAddress("OMGaussNDF", ROOT.AddressOf(snomwatchvar, 'OMGaussNDF') )
    #     snomwatch.SetBranchAddress("OMRate", ROOT.AddressOf(snomwatchvar, 'OMRate') )
    #
    #     for i in range(nentriesomwatch):
    #         # Getting data by index
    #         snomwatch.GetEntry(i)
    #
    #         time = get_daq_time_as_long(snomwatchstreaming.gps_time_high,
    #                                     snomwatchstreaming.gps_time_low)
    #         snomwatchrow["gps_time"] = time
    #         snomwatchrow["RebinWidth"] = snomwatchvar.RebinWidth
    #         snomwatchrow["OMSNChannel"] = snomwatchvar.OMSNChannel
    #         snomwatchrow["OMId"] = snomwatchvar.OMId
    #         snomwatchrow["OMMean"] = snomwatchvar.OMMean
    #         snomwatchrow["OMSigma"] = snomwatchvar.OMSigma
    #         snomwatchrow["OMFano"] = snomwatchvar.OMFano
    #         snomwatchrow["OMSkew"] = snomwatchvar.OMSkew
    #         snomwatchrow["OMKurtosis"] = snomwatchvar.OMKurtosis
    #         snomwatchrow["OMGaussSigma"] = snomwatchvar.OMGaussSigma
    #         snomwatchrow["OMGaussRedChi2"] = snomwatchvar.OMGaussRedChi2
    #         snomwatchrow["OMGaussNDF"] = snomwatchvar.OMGaussNDF
    #         snomwatchrow.append()

    def untar_files(self, files, outdir):
        """
        Checking if input files are tar or ROOT files. Either untar files,
        find ROOT files, and return, check if ROOT file and return, 
        or raise error if no proper input files

        :param files: List of file names to be considered
        :param outdir: outdir or temp dir for untaring
        :returns: List of root files to be processed
        """
        fnroot = []
        for f in files:
            # Checking if we have tar file
            if tarfile.is_tarfile(f):
                if not os.path.isdir(outdir):
                    raise RuntimeError("Need a path to untar ROOT file.")
                tarf = tarfile.open(f)
                # Getting only ROOT file out of tar file
                for fn in tarf.getnames():
                    if ".root" in fn:
                        tarf.extract(fn, outdir)
                        fnroot.append(os.path.join(outdir, os.path.basename(fn)))
            # Checkin if we have a ROOT file
            elif ROOT.TFile.Open(f):
                fnroot.append(f)
            else:
                raise RuntimeError("No tarfile with ROOT file or ROOT file as input")
        if len(fnroot) == 0:
            raise RuntimeError("No input files")
        return fnroot

    def get_base_outfilename_run_number(self, fnroot):
        """
        Get base name for file, i.e. sndata_RUNUMBER.root
    
        :param fnroot: List of input ROOT files
        :returns: Tuple of name of output file without extension and Run Number
        """
        fbn = os.path.basename(fnroot)
        runNum = fbn.split("_")[1]
        dirn = os.path.dirname(fnroot)
        outn = "%s/sndata_%s.%s" % (dirn, fbn.split("_")[1], "h5")
        return outn, int(runNum)

    def convert_omkey_domnum(self, omkey):
        """
        Converting a string OMKey to a DOM number
    
        :param omkey: OMKey in "String #-DOM #" as a string
        :returns: Dom number
        """
        try:
            string, dom = omkey.split("-")
        except:
            raise RuntimeError("Cannot convert %s into an DOM number" % omkey)
        return get_dom_num(string, dom)

    def get_dom_num(self, string, dom):
        """
        Converting OMKey from String # and DOM #
    
        :param string: String #
        :param dom: DOM #
        :returns: DOM numbers
        """
        return ((int(string) - 1) * 60) + (int(dom) - 1)

    def get_geo_mapping(self, fn, fullfile = False):
        """
        Generating geometry mapping. 
        Looking for the file line by line and matching regex expression. 
    
        :param fn: Filename of geometry file
        :returns: Dict that maps DOM number to the location in an array.
        """
        # Picking the right pattern depending on if we have a complete geometry file or reduced geometry file
        if fullfile: 
            pattern = "DOM\s'\w+'\,\sPosition\s(\d+\-\d+)\,\s\w+\s\w+\/\w+\s\w+\s\w+\s(\d+)\s\w+\s+\w+:\s\d\.?\d?\d?\s\w+:\s\-?\d"
        else:
            pattern = "(\d+\-\d+)\s(\d+)"
        com_pattern = re.compile(pattern)
        # Creating the geometry dictionary
        with open(fn, "r") as f:
            geomap = {com_pattern.match(l).group(2): com_pattern.match(l).group(1) for l in f.readlines() if com_pattern.match(l) != None}
        return geomap

    def get_filenumber(self, fn):
        """
        Get file number by splitting up file name 
    
        :param fn: Filename
        :returns: File number, i.e. file identifier, 
                  if run had more than one output file
        """
        return int(fn.split("_")[-1].split(".")[0])

    def check_file_exist_open(self, filename):
        """
        Sanity check if file exists and can be opened
    
        :param filename: Name of file to be checked
        :returns: Boolean
        """
        if os.path.isfile(filename):
            try:
                fp = open(filename)
            except IOError as e:
                raise RuntimeError("Could not open file: %s" % mapfullpath)
            else:
                return True
        else:
            raise RuntimeError("Could not locate file: %s" % mapfullpath)
        
    def convert(self):
        """
        Main function.
        """
        # Checking if we are in Condor world and we have a local scratch disk we can
        # untar things into
        if "_CONDOR_SCRATCH_DIR" in os.environ.keys():
            fnroot = untar_files(self.args, "%s/" % os.environ["_CONDOR_SCRATCH_DIR"])
        elif self.options.outdir:
            fnroot = untar_files(self.args, self.options.outdir)
        else:
            fnroot = untar_files(self.args, os.path.dirname(self.options.outfile))

        if options.outfile:
            create_out_file(self.options.outfile)
        previous_run_num = 0

        fnroot = sorted(set(fnroot))
        for fr in fnroot:
            # Getting file name and run number
            outfn, runNum = get_base_outfilename_run_number(fr)
    
            # Creating output file according to input file name and run number
            if not options.outfile and previous_run_num != runNum:
                if previous_run_num != 0:
                    # Flushing tables to disk.
                    self.sncandtable.flush()
                    self.snrouttable.flush()
                    self.snalltable.flush()
                    # snomwatchtable.flush()
                    # Closing previous file
                    self.h5file.close()
                create_out_file(outfn)
                previous_run_num = runNum
    
            # Opening ROOT files and getting data pointers
            print "Opening ROOT file %s" % fr
            f = ROOT.TFile(fr)
            snall = f.Get("sn_all")
            snrout = f.Get("sn_o2rout")
            sncand = f.Get("sn_o2cand")
            # snomwatch = f.Get("sn_omwatch")
            filenum = get_filenumber(fr)
    
            # Checking file integrity 
            try:
                print "Number of entries in snall %d" % snall.GetEntries()
                dosnall = True if snall.GetEntries() > 0 else False
            except:
                warnings.warn("snall broken for %s" % fr)
                dosnall = False

            try:
                print "Number of entries in snrout %d" % snrout.GetEntries()
                dosnrout = True if snrout.GetEntries() > 0 else False
            except:
                warnings.warn("snrout broken for %s" % fr)
                dosnrout = False

            try:
                print "Number of entries in sncand %d" % sncand.GetEntries()
                dosncand = True if sncand.GetEntries() > 0 else False
            except:
                warnings.warn("sncand broken for %s" % fr)
                dosncand = False
    
            # Do all the work
            if dosnall:
                geomapfn = get_geo_mapping_filename(runNum, filenum, options.geodir, options.geofile)
                geomap = get_geo_mapping(geomapfn)
                extract_sn_all(snall, snallrow, geomap)
    
            if dosnrout:
                extract_sn_rout(snrout, snroutrow)
    
            if dosnrout:
                extract_sn_cand(sncand, sncandrow)
    
            if not dosnall and not dosnrout and not dosncand:
                raise RuntimeError("File weird")
    
            f.Close()
            os.remove(fr)
    
        # Flushing tables to disk.
        self.sncandtable.flush()
        self.snrouttable.flush()
        self.snalltable.flush()
        # snomwatchtable.flush()

        self.h5file.close()
    
        # Moving things out to network storage if we are in Condor land
        if "_CONDOR_SCRATCH_DIR" in os.environ.keys():
            shutil.copyfile(outfn, os.path.join(outdir, os.path.basename(outfn)))

if __name__ == "__main__":
    parser = OptionParser("usage: %prog [options] arg1 arg2")
    parser.add_option("-g","--geofile", dest="geofile", help="Geometry file", default=None, type=str)
    parser.add_option("-G","--geodir", dest="geodir", help="Directory where output files go", default=None, type=str)
    parser.add_option("-o","--outfile", dest="outfile", help="Directory where output files go", default=None, type=str)
    parser.add_option("-O","--outdir", dest="outdir", help="Output file", default=None, type=str)
    options, args = parser.parse_args()
    if len(args) < 1:
        parser.error("You must supply at least one input file")
    if (options.outfile and os.path.isdir(options.outfile)) or \
       (options.geofile and os.path.isdir(options.geofile)):
        parser.error("You provided directory for a file argument.")
    if options.outdir and options.outfile:
        parser.error("You provided both an output file and directory. Can only have one.")
    if not options.geodir and not options.geofile:
        parser.error("You have not provided a location for the geometry file.")
    if not options.outdir and not options.outfile:
        parser.error("You have not provided an output file or directory.")
    if options.outdir and not os.path.exists(options.outdir):
        print "Creating output directory: %s" % options.outdir
        os.makedirs(outdir)
        
    # Cleaning out ROOT settings - For Safety
    # ROOT likes to keep state
    ROOT.gROOT.Reset()

    # Defining data structures that are stored in the 
    # ROOT file, see 
    # https://wiki.icecube.wisc.edu/index.php/Data_Formats
    s = "struct streaming_t {\
        ULong_t base_time;\
        ULong_t gps_time_high;\
        ULong_t gps_time_low;\
        ULong_t buf_pos;\
        ULong_t buf_loop;\
        };\
        struct all_t {\
        Int_t no_channels;\
        UShort_t data[5160];\
        };\
        struct rout_t {\
        Int_t NMAvBins;\
        Int_t NRebins;\
        Double_t Signal;\
        Double_t SignalError;\
        Double_t Chi2;\
        Int_t ActiveChannels;\
        Int_t RebinIndex;\
        Int_t NNewActOMs;\
        Int_t NewActOMs[5160];\
        Int_t NNewInactOMs;\
        Int_t NewInactOMs[5160];\
        };\
        struct cand_t {\
        Int_t NCandidate;\
        Int_t NChannels;\
        Int_t NEvent;\
        Double_t ChannelRate[5160];\
        Double_t ChannelMean[5160];\
        Double_t ChannelSigmaSquare[5160];\
        Double_t ChannelWeighedDev[5160];\
        };\
        struct omwatch_t{\
        Int_t RebinWidth;\
        Int_t OMSNChannel;\
        ULong64_t OMId;\
        Double_t OMMean;\
        Double_t OMSigma;\
        Double_t OMFano;\
        Double_t OMSkew;\
        Double_t OMKurtosis;\
        Double_t OMGaussSigma;\
        Double_t OMGaussRedChi2;\
        Int_t OMGaussNDF;\
        TH1D* OMRate;\
        };\
        "

    # Telling ROOT what the data looks like. Creating
    # the correct ROOT state
    ROOT.gROOT.ProcessLine(s)
    
    # Lets get the action started!
    SNDAQROOTtoHDF5Converter(options, args)
