#!/usr/bin/env python

import datetime
import re
import sys


class MonitorFile(object):
    # match a category/time line like "foo: 2011-02-28 09:10:11.123456:"
    #
    CATTIME_PAT = re.compile(r"^([^:]+):\s(\d+-\d+-\d+\s\d+:\d+:\d+\.\d+)\s*")

    # match a data line like "    name: value"
    #
    DATA_PAT = re.compile(r"^(\s+)([^:]+):\s+(.*\S)\s*")

    # list/dict separator
    #
    COMMA_SEP_PAT = re.compile("\s*,\s*")

    # dict key/value separator
    #
    COLON_SEP_PAT = re.compile("\s*:\s*")

    def __init__(self, filename):
        """
        Cache a pDAQ monitor file
        """
        self._data = self._read_file(filename)

    def _get_value(self, valstr):
        try:
            return int(valstr)
        except:
            try:
                return float(valstr)
            except:
                if valstr.startswith("["):
                    return self._parse_list(valstr)
                elif valstr.startswith("{"):
                    return self._parse_dict(valstr)
                else:
                    return valstr

    def _parse_list(self, listStr):
        newlist = []
        for val in self.COMMA_SEP_PAT.split(listStr[1:-1]):
            newlist.append(self._get_value(val))
        return newlist

    def _parse_dict(self, dictStr):
        newdict = {}
        for pair in self.COMMA_SEP_PAT.split(dictStr[1:-1]):
            (key, value) = self.COLON_SEP_PAT.split(pair)
            newdict[key] = self._get_value(value)
        return newdict

    def _read_file(self, filename):
        "Read and parse a monitor file"
        data = {}

        curDict = None
        for line in open(filename, "r"):
            line = line.rstrip()

            if len(line) == 0:
                curDict = None
                continue

            m = self.CATTIME_PAT.match(line)
            if m is not None:
                cat = m.group(1)
                time = datetime.datetime.strptime(m.group(2),
                                                  "%Y-%m-%d %H:%M:%S.%f")
                if not time in data:
                    data[time] = {}
                curDict = {}
                data[time][cat] = curDict
                continue

            m = self.DATA_PAT.match(line)
            if m is not None:
                curDict[m.group(2)] = self._get_value(m.group(3))
                continue

            print >>sys.stderr, "Bad line: " + line

        return data

    def dump_data(self):
        "Dump data in pDAQ .moni format"
        self.write_data_to_file(sys.stdout, self._data)

    @classmethod
    def write_data_to_file(cls, out, data):
        "Write data to a file in pDAQ .moni format"
        times = data.keys()
        times.sort()

        need_nl = False

        for t in times:
            keys = data[t].keys()
            keys.sort()
            for k in keys:
                if need_nl:
                    print >>out
                else:
                    need_nl = True

                print >>out, "%s: %s" % (k, t)

                names = data[t][k].keys()
                names.sort()
                for n in names:
                    print >>out, "\t%s: %s" % (n, data[t][k][n])


if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise SystemExit("Please specify a file to process")

    for f in sys.argv[1:]:
        mf = MonitorFile(f)
        mf.dump_data()
