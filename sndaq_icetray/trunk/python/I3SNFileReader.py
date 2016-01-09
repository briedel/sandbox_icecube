from icecube import icetray, dataio, dataclasses, phys_services #, sndaq
from icecube.sndaq import I3SNConfig, I3SNPayload, I3SNDOMStatus #, sn_payload_reader, sn_data#, I3SNDatagramMap
from sn_payload_reader import sn_payload_reader, sn_data
import datetime
import os
from struct import unpack, pack
from collections import defaultdict
import math
import sys
import glob
import time

class I3SNReader(icetray.I3ConditionalModule):
    """

    """
    def __init__(self, context):
        """

        """
        icetray.I3ConditionalModule.__init__(self, context)
        self.AddOutBox("OutBox")

        self.AddParameter("InDirectory",
                          "Directory where input files can be found",
                          None)

        self.AddParameter("InFilePattern",
                          "Pattern to look for in input directory",
                          "sn_*_*_*_*.dat")

        self.AddParameter("InFile",
                          "Single input file",
                          None)

        self.AddParameter("InFileList",
                          "List of input files",
                          None)

        self.AddParameter("Year",
                          "Year to be used",
                          0)

        self.AddParameter("DataBinSize",
                          "Bin size of data scalers",
                          1.6384*icetray.I3Units.millisecond)

        self.AddParameter("TimeSyncedBinSize",
                          "Bin size of time synchronized scalers",
                          2.*icetray.I3Units.millisecond)

        self.AddParameter("NumberBytesScalers",
                          "Number of bytes used per scaler record on DOM",
                          65536)

        self.AddParameter("OMKey2MBIDXMLInFile",
                          "XML file that maps OMKeys to MBID and vice versa",
                          os.path.join(os.path.expandvars("$I3_SRC"), "phys-services/resources/mainboard_ids.xml.gz")
                          )

        self.AddParameter("GeometryFile",
                          "",
                          os.path.join(os.path.expandvars("$I3_SRC"), "sndaq/resources/GD.i3.gz"))

        self.AddParameter("IgnoreIceTop",
                          "Ignore IceTop DOMs",
                          True)

    def Configure(self):
        """

        """
        icetray.logging.log_info("Configuring SNDAQ")
        self.config = I3SNConfig()
        self.dom_status = dict()
        self.n_payloads = 0

        self.indir = self.GetParameter("InDirectory")
        self.infilepattern = self.GetParameter("InFilePattern")
        infilename = self.GetParameter("InFile")
        self.infilelist = self.GetParameter("InFileList")
        self.config.databinsize = long(self.GetParameter("DataBinSize"))
        self.config.timesyncbinsize = long(self.GetParameter("TimeSyncedBinSize"))
        self.config.clockcyclesperscalers = long(self.GetParameter("NumberBytesScalers"))
        self.year = self.GetParameter("Year")
        self.geometry_file = self.GetParameter("GeometryFile")
        self.ignore_IceTop = self.GetParameter("IgnoreIceTop")
        OMKey2MBIDXMLfile = self.GetParameter("OMKey2MBIDXMLInFile")
        self.omkey2mbid = phys_services.I3XMLOMKey2MBID(OMKey2MBIDXMLfile)

        if self.indir and not os.path.exists(self.indir):
            icetray.logging.log_fatal("Please provide a valid input directory.")
        if not (infilename or self.infilelist) and not self.indir:
            icetray.logging.log_fatal("Please provide an input directory and input file.")
        if (infilename or self.infilelist) and self.indir:
            icetray.logging.log_fatal("Please provide either an input directory or input file.")
        if infilename and not self.infilelist:
            self.infilelist = [infilename]
        elif infilename and self.infilelist:
            icetray.logging.log_fatal("Please provide either an input file or input file list.")

        if self.year == 0:
            icetray.logging.log_warn("Getting year from system")
            self.year = datetime.date.today().year

        self._reset_sndaq()
        self.run_start_time = dataclasses.I3Time(self.year, 0)
        self.current_run = 0
        self.previous_daq_record = dict()

    def Process(self):
        """

        """
        # recurcive call to check for file, if new file is present read in
        # if file isnt there, check again
        # try:
        if self.infilelist:
            for f in self.infilelist:
                self._process_file(f)
            else:
                self.RequestSuspension()
        else:
            self._check_for_files()
        # except:
        #     self.RequestSuspension()


        # with open(self.filename, "rb") as f:
        #     try:
        #         while 1:
        #             frame = icetray.I3Frame(icetray.I3Frame.DAQ)
        #             payload = self._read_in_payloads(f)
        #             if not self._check_payload(payload):
        #                 continue
        #             if self.n_payloads == 0: self.run_start_time = payload.time
        #             self.n_payloads += 1
        #             frame.Put("I3SNPayload", payload)
        #             self.PushFrame(frame)
        #     except StopIteration:
        #         self._check_for_new_file_run()
        #         # self.RequestSuspension()
        #     else:
        #         self.RequestSuspension()

    def check_config(self):
        """
        Makre sure people are aware that they are operating with a non-standard definition of data
        """
        if  self.config.databinsize != 16384000L:
            icetray.logging.log_warn("Data bin size has been set to %d" % config.databinsize)
        if  self.config.numbytesscalers != 2**16:
            icetray.logging.log_warn("Number of bytes per scaler in DOM has been set to % d" % config.numbytesscalers)


    def _reset_sndaq(self):
        with dataio.I3File(self.geometry_file) as geofile:
            geometry = geofile.pop_frame()["I3Geometry"]
            self.bad_doms = geofile.pop_frame()["BadDomsListSLC"]
        self.dom_status = {omkey: I3SNDOMStatus() for omkey, geo in geometry.omgeo}

    def _process_file(self, filename):
        run_number = self._get_run_number(filename)
        with open(filename, "rb") as f:
            for daq_record in sn_payload_reader(f):
                if self.current_run != run_number and self.n_payloads == 0:
                    self.run_start_time = dataclasses.I3Time(self.year, daq_record.utc)
                    self.current_run = run_number
                    self.n_payloads = 0
                self.n_payloads += 1
                if not self._is_payload_usable(daq_record):
                    continue
                omkey = self.omkey2mbid.get_omkey(daq_record.mbid)
                frame = icetray.I3Frame(icetray.I3Frame.DAQ)
                payload = I3SNPayload()
                payload.scalers = daq_record.scalers
                payload.time = dataclasses.I3Time(self.year, daq_record.utc)
                payload.omkey = omkey
                eventheader = dataclasses.I3EventHeader()
                eventheader.run_id = run_number
                eventheader.start_time = self.run_start_time
                frame.Put("I3SNPayload", payload)
                frame.Put("I3EventHeader", eventheader)
                self.PushFrame(frame)
        os.rename(filename, filename + "_processed" )
        if not self.infilelist:
            self._check_for_files()

    def _check_for_files(self):
        while True:
            infiles = sorted(glob.glob(os.path.join(self.indir, self.infilepattern)))
            if len(infiles) >= 2:
                _process_file(infiles[0])
            else:
                time.sleep(0.07)

    def _is_payload_usable(self, daq_record):
        """
        Make sure that we only get the desired payloads.
        Remove known bad DOMs
        Make sure that the DOMs are not jittering in time, i.e. overlapping scalers or missing scalers
        """
        omkey = self.omkey2mbid.get_omkey(daq_record.mbid)
        # Remove IceTop DOMs
        if omkey.om > 60 and self.ignore_IceTop:
            return False
        # Skip bad DOMs
        if omkey in self.bad_doms:
            return False
        # Check for jitter
        # If new run, need to change expected DOM clock and start time first
        # SOMETHING DOESNT WORK IN HERE
        if omkey not in self.previous_daq_record:
            self.previous_daq_record[omkey] = daq_record
            return True
        else:
            previous_daq_record = self.previous_daq_record[omkey]
            domclkend = long(previous_daq_record.domclk +\
                             (len(previous_daq_record.scalers) *\
                              self.config.clockcyclesperscalers))
            diff_domclk = daq_record.domclk - domclkend
            # icetray.logging.log_warn("Difference DOM Clock: %d" % diff_domclk)
            self.previous_daq_record[omkey] = daq_record
            if diff_domclk == 0:
                # self.dom_status[omkey].expectedstarttime = utcend
                # self.dom_status[omkey].expectedDOMclock = domclkends
                return True
            else:
                warning_string = "There is %f ns gap in the UTC timestamp " % diff_time +\
                                 "and a %d cycle gap in the number of DOM clock cycle.\n" % diff_domclk +\
                                 "Current DOM UTC time: %s.\n" % daq_record.utc +\
                                 "Expected DOM UTC time: %s.\n" %  self.dom_status[omkey].expectedstarttime +\
                                 "Current number of DOM clock cycles: %d.\n" % daq_record.domclk +\
                                 "Expected number DOM clock cycles: %d." % self.dom_status[omkey].expectedDOMclock
                if diff_domclk > 0:
                    warning_string = "\n%s is jittering and has a data gap. " + warning_string
                    self.dom_status[omkey].jittercount += 1
                    if self.dom_status[omkey] > 10:
                        return False
                    else:
                        icetray.logging.log_warn(warning_string)
                        return True
                if diff_domclk < 0:
                    warning_string = "\n%s is jittering and has a overlapping data. " + warning_string
                    if not self.dom_status[omkey] > 10:
                        icetray.logging.log_warn(warning_string)
                    self.dom_status[omkey].jittercount += 1
                    return False

    def _get_run_number(self, filename):
        return int(filename.split("_")[1])


    # def _read_in_payloads(self, f):
    #     """
    #
    #     """
    #     icetray.logging.log_trace("Read payload");
    #
    #     payload = I3SNPayload()
    #
    #     hdr = f.read(16)
    #     # if len(hdr) == 0: raise StopIteration
    #     bytes, fmtid, utc = unpack('>iiq', hdr)
    #     buf = f.read(bytes - 16)
    #     if fmtid == 16:
    #         mbid, = unpack('>q', buf[0:8])
    #         temp_payload = sn_data(buf[8:], mbid, utc)
    #
    #     payload.scalers = temp_payload.scalers
    #     payload.time = dataclasses.I3Time(self.year, temp_payload.utc)
    #     payload.omkey = self.omkey2mbid.get_omkey(mbid)
    #
    #     return payload




# class I3SNReader(icetray.I3ConditionalModule):
#     """
#
#     """
#     def __init__(self, context):
#         """
#         Class constructor for the icetray python module
#
#         :param context: I3Context
#         """
#         icetray.I3ConditionalModule.__init__(self, context)
#         self.AddOutBox("OutBox")
#
#         self.AddParameter("InDirectory",
#                           "Directory where input files can be found",
#                           None)
#
#         self.AddParameter("InFile",
#                           "Single input file",
#                           None)
#
#         self.AddParameter("Year",
#                           "Year to be used",
#                           0)
#
#         self.AddParameter("DataBinSize",
#                           "Bin size of data scalers",
#                           1.6384*icetray.I3Units.millisecond)
#
#         self.AddParameter("TimeSyncedBinSize",
#                           "Bin size of time synchronized scalers",
#                           2.*icetray.I3Units.millisecond)
#
#         self.AddParameter("OMKey2MBIDXMLInFile",
#                           "XML file that maps OMKeys to MBID and vice versa",
#                           os.path.join(os.path.expandvars("$I3_SRC"), "phys-services/resources/mainboard_ids.xml.gz")
#                           )
#
#         self.config = I3SNConfig()
#         self.domstatus = dict()
#         self.n_payloads = 0
#
#     def Configure(self):
#         """
#
#         """
#         icetray.logging.log_info("Configuring SNDAQ")
#
#         self.indir = self.GetParameter("InDirectory")
#         self.infilename = self.GetParameter("InFile")
#         self.config.databinsize = long(self.GetParameter("DataBinSize"))
#         self.config.timesyncbinsize = long(self.GetParameter("TimeSyncedBinSize"))
#         self.year = self.GetParameter("Year")
#         OMKey2MBIDXMLfile = self.GetParameter("OMKey2MBIDXMLInFile")
#
#         if self.infilename:
#             self.f = open(self.infilename, "rb")
#         if self.indir:
#             self.check_for_files()
#         if not self.infilename and not self.indir:
#             icetray.logging.log_fatal("Please provide an input directory and input file.")
#         if self.infilename and self.indir:
#             icetray.logging.log_fatal("Please provide an input directory and input file.")
#
#         if self.year == 0:
#             icetray.logging.log_warn("Getting year from system")
#             self.year = datetime.date.today().year
#
#         self.omkey2mbid = phys_services.I3XMLOMKey2MBID(OMKey2MBIDXMLfile)
#
#     def Process(self):
#         """
#
#         """
#         try:
#             while 1:
#                 frame = icetray.I3Frame(icetray.I3Frame.DAQ)
#                 payload = self._read_in_raw_payloads()
#                 frame.Put("I3SNPayload", payload)
#                 if self.n_payloads == 0: self.run_start_time = payload.time
#                 self.n_payloads += 1
#                 datagram = self._rebin_payloads(payload)
#                 # print self.n_payloads
#                 frame.Put("I3SNDatagram", datagram)
#                 self.PushFrame(frame)
#         except StopIteration:
#             self.RequestSuspension()
#         else:
#             self.RequestSuspension()
#
#     def _check_for_files(self):
#         pass
#
#     def _read_in_raw_payloads(self):
#         """
#
#         """
#         icetray.logging.log_trace("Read payload");
#
#         payload = I3SNPayload()
#
#         hdr = self.f.read(16)
#         if len(hdr) == 0: raise StopIteration
#         bytes, fmtid, utc = unpack('>iiq', hdr)
#         buf = self.f.read(bytes - 16)
#         if fmtid == 16:
#             mbid, = unpack('>q', buf[0:8])
#             temp_payload = sn_data(buf[8:], mbid, utc)
#
#         payload.scalers = list(temp_payload.scalers)
#         payload.time = dataclasses.I3Time(self.year, temp_payload.utc)
#         payload.omkey = self.omkey2mbid.get_omkey(mbid)
#
#         return payload
#
#     def _set_run_start(self):
#         pass
#
#     def _rebin_payloads(self, payload):
#         """
#
#         """
#         datagram = I3SNDatagram()
#
#         abs_index = 0
#         previous_absolute_time_left_edge = None
#         time_sync_bins = []
#         # print payload.scalers
#
#         for i, s in enumerate(payload.scalers):
#             # print i, s
#             time_right_edge_scaler = payload.time + (i * self.config.databinsize)
#             time_left_edge_scaler = time_right_edge_scaler + self.config.databinsize
#
#             # print (time_right_edge_scaler - self.run_start_time) / self.config.timesyncbinsize
#             # print math.floor((time_right_edge_scaler - self.run_start_time) / self.config.timesyncbinsize)
#             time_sync_bin_number = math.floor((time_right_edge_scaler - self.run_start_time) / self.config.timesyncbinsize)
#
#             absolute_time_right_edge = self.run_start_time + (time_sync_bin_number * self.config.timesyncbinsize);
#             absolute_time_left_edge = absolute_time_right_edge + self.config.timesyncbinsize;
#
#             # print absolute_time_right_edge
#             # print absolute_time_left_edge
#
#             if abs_index == 0 and i == 0:
#                 datagram.start_time = absolute_time_right_edge
#                 previous_absolute_time_left_edge = absolute_time_left_edge
#             # print previous_absolute_time_left_edge
#
#             left_edge = max(time_right_edge_scaler, absolute_time_right_edge)
#             right_edge = min(absolute_time_left_edge, time_left_edge_scaler)
#
#             # print (right_edge - left_edge) / self.config.databinsize
#             right_split = ((right_edge - left_edge) / self.config.databinsize)*100
#             # print right_split
#             left_split = (100 - right_split)
#
#             if previous_absolute_time_left_edge == absolute_time_right_edge:
#                 abs_index += 1
#                 previous_absolute_time_left_edge = absolute_time_left_edge
#
#             # print i, s, right_split, left_split, abs_index, len(time_sync_bins)
#             if len(time_sync_bins) <= abs_index + 1:
#                 # print ([0] * (abs_index - len(time_sync_bins) + 2))
#                 time_sync_bins += ([0] * (abs_index - len(time_sync_bins) + 2))
#             time_sync_bins[abs_index] += int(right_split*s)
#             time_sync_bins[abs_index + 1] += int(left_split*s)
#             # print time_sync_bins
#             # if self.n_payloads > 2: sys.exit()
#         # print time_sync_bins
#         datagram.timesyncedscalers = dataclasses.I3VectorUShort(time_sync_bins)
#         return datagram
#         # out = I3SNDatagramMap()
#         # out[payload.omkey] = datagram
#         # return out
