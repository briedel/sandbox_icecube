#-----------------------------------------------------------------------------------
# Sorting the data from readmonifile.py into data={} according to filename for the
# pDAQ moni tool
#
# -briedel
# Last Changed: 11/03/2011
#
# 11/03/2011 - briedel 
# * Reduce line numbers by having file number as keys for data[filename]={}
#
# 10/28/2011 - briedel
# * Created new class to sort the MonitorFile() by filename into keys for data={} 
#	and file number as keys for data[filename]={}
#
#-----------------------------------------------------------------------------------

import os, sys
from readmonifile import MonitorFile
 
class sort(object):
	data={}
	@classmethod
	def sorting(cls, filename):
		###
		# Sorting the data by filename
		###
	    # Splitting up the file name to get keys for dict
		name = os.path.splitext(filename)[0].partition('-')
		ext = os.path.splitext(filename)[1]
		comp=name[0]
		num=name[2]
		if not comp in cls.data:
			cls.data[comp]={}
		# Sorting done here 
		if 'moni' in ext:
			cls.data[comp][num]=MonitorFile(filename).dump_data()
		return cls.data