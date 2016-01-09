#!/usr/bin/env python
import os
import sys
from readmonifile import MonitorFile

sHnumoverall=[]
moni={}
#log={}
data={}
 
for f in sys.argv[1:]:
    #Splitting up the file name
	name = os.path.splitext(f)[0]
	ext = os.path.splitext(f)[1]
	print f
	if 'stringHub' in name:
		sHnum=name.partition('-')[2]
		sHnumoverall.append(int(sHnum))
		if 'moni' in ext:
			moni[sHnum] = MonitorFile(f).dump_data()
#		elif 'log' in ext:
#			log[sHnum] = MonitorFile(f).dump_data()
		else:
			print "No stringHub moni files" 
		data['stringHub'] = moni		
	if 'inIceTrigger' in name:
		if 'moni' in ext:
			data['inIceTrigger'] = MonitorFile(f).dump_data()
	if 'globalTrigger' in name:
		if 'moni' in ext:
			data['globalTrigger'] = MonitorFile(f).dump_data()
	if 'eventBuilder' in name:
		if 'moni' in ext:
			data['eventBuilder'] = MonitorFile(f).dump_data()
	if 'secondaryBuilders' in name:
		if 'moni' in ext:
			data['secondaryBuilders'] = MonitorFile(f).dump_data()

print moni
print sHnum