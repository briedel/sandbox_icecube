from struct import unpack, pack
from icecube import icetray
# class sn_data:
#     def __init__(self, data, mbid, utc):
#         """
#         Create from payload
#         """
#         bytes, fmtid, t5, t4, t3, t2, t1, t0 = unpack('>hh6B', data[0:10])
#         # self.mbid = '%12.12x' % mbid
#         self.mbid = mbid
#         self.utc  = utc
#         self.scalers = unpack('%dB' % (bytes - 10), data[10:])
#         self.domclk  = ((((t5 << 8L | t4) << 8L | t3) \
#                         << 8L | t2) << 8L | t1) << 8L | t0
#         self.utcend  = self.utc + len(self.scalers) * 16384000L
#
# class sn_payload_reader:
#     def __init__(self, f):
#         self.f = f
#
#     def __iter__(self):
#         return self
#
#     def next(self):
#         while 1:
#             hdr = self.f.read(16)
#             if len(hdr) == 0: raise StopIteration
#             bytes, fmtid, utc = unpack('>iiq', hdr)
#             buf = self.f.read(bytes - 16)
#             if fmtid == 16:
#                 mbid, = unpack('>q', buf[0:8])
#                 return sn_data(buf[8:], mbid, utc)


class sn_payload_reader(object):
    def __init__(self, file = None):
        if file: self.f = file

    def __iter__(self):
        while True:
            p = self.read_payload()
            if p != 0 and p != None:
                yield p
            else:
                print "breaking"
                break

    def read_pDAQ_hdr(self, pDAQ_hdr):
        return unpack('>iiq', pDAQ_hdr)

    def read_sn_header(self, sn_header):
        mbid, sn_bytes, sn_fmtid, t5, t4, t3, t2, t1, t0 = unpack('>qhh6B', sn_header)
        domclk = ((((t5 << 8L | t4) << 8L | t3) \
                  << 8L | t2) << 8L | t1) << 8L | t0
        return  mbid, sn_bytes, sn_fmtid, domclk

    def read_payload(self):
        pDAQ_hdr = self.f.read(16)
        if len(pDAQ_hdr) != 0:
            bytes, fmtid, utc = self.read_pDAQ_hdr(pDAQ_hdr)
            sn_buf = self.f.read(bytes - 16)
            if fmtid == 16 :
                mbid, sn_bytes, sn_fmtid, domclk = self.read_sn_header(sn_buf[:18])
                if sn_fmtid == 300:
                     return sn_data(sn_buf[18:], mbid, sn_bytes, utc, domclk)
                else:
                    icetray.logging.log_fatal("Payload does not have correct SN format ID!")
            else:
                icetray.logging.log_fatal("Payload does not have correct pDAQ payload type!")
        else:
            return 0

class sn_data(object):
    def __init__(self, data, mbid, sn_bytes,
                 utc, domclk):
        """
        Create from payload
        """
        # self.mbid = '%12.12x' % mbid
        self.mbid = mbid
        self.utc  = utc
        # 10 instead of 18 because
        # mbid is not part of the bytes counted in the header
        self.scalers = list(unpack('%dB' % (sn_bytes - 10), data))
        self.domclk  = domclk

    def __str__(self):
        string = "MBID = %12.12x\n" % self.mbid +\
                 "UTC = %s\n" % self.utc +\
                 "DOM Clock = %s\n" % self.domclk +\
                 "Scalers = %s" % self.scalers
        return string
