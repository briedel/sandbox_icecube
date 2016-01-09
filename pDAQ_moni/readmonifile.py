#-----------------------------------------------------------------------------------
# Taken from i3Live and adjusted by dglo and briedel for the pDAQ moni tool
#
# -briedel
# Last Changed: 11/05/2011
#
# 11/05/2011 - briedel 
# * Adjusted the dictionary pattern to 'dict[category][subcategory][time] = value' for 
#   plotting and more efficient GUI creation
# * Remove write_data_to_file not needed for this
#
# 10/24/2011 - briedel/dglo
# * Adjusted CATTIME_PAT to get reduce precision in time gotten out of file
# * Introduce COMMA_SEP_PAT, COLON_SEP_PAT, _parse_list, and _parse_dict by dglo
#   to parse dictionaries and lists in moni files.
#
#-----------------------------------------------------------------------------------

import re, sys, os, datetime

class MonitorFile(object):
    # match a category/time line like "foo: 2011-02-28 09:10:11.123456:"
    #
    CATTIME_PAT = re.compile(r"^([^:]+):\s(\d+-\d+-\d+\s\d+:\d+:\d+\.\d{2})\d+\s*")

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
        ###
        #Cache a pDAQ monitor file
        ###
        self._data = self._read_file(filename)

    def _get_value(self, valstr):
        ###
        # Getting value of line
        ###
        try:
            return int(valstr)
        except:
            try:
                return float(valstr)
            except:
                ###
                # Differentiate between lists and dictionary
                # and parse them 
                ###
                if valstr.startswith("["):
                    return self._parse_list(valstr)
                elif valstr.startswith("{"):
                    return self._parse_dict(valstr)
                else:
                    return valstr
    
    def _parse_list(self, listStr):
        ###
        # Parsing List
        ###
        newlist = []
        for val in self.COMMA_SEP_PAT.split(listStr[1:-1]):
            newlist.append(self._get_value(val))
        return newlist

    def _parse_dict(self, dictStr):
        ###
        # Parsing dictionary 
        ###
        newdict = {}
        try:
            for pair in self.COMMA_SEP_PAT.split(dictStr[1:-1]):
                (key, value) = self.COLON_SEP_PAT.split(pair)
                newdict[key] = self._get_value(value)
            return newdict
        except:
            print 'Empty dict'


    def _read_file(self, filename):
        ###
        #Read and parse a moni file
        ###
        data = {}

        tDict = {}
        for line in open(filename, "r").readlines():
            if len(line) == 0:
                tDict = None
                continue
            
            ###
            # Matching patterns to lines and sorting them into dict
            ###
            m = self.CATTIME_PAT.match(line)
            n = self.DATA_PAT.match(line)
            if m is not None:
                cat = m.group(1)
                time = datetime.datetime.strptime(m.group(2),
                                                  "%Y-%m-%d %H:%M:%S.%f")
                if not cat in data:
                    data[cat]={}
            elif n is not None:
                scat = n.group(2)
                if not scat in data[cat]:
                    data[cat][scat]={}
                data[cat][scat][time] = self._get_value(n.group(3))

            #print >>sys.stderr, "Bad line: " + line
        
        return data

    def dump_data(self):
        ###
        # Return data in dict form
        ###
        return self._data