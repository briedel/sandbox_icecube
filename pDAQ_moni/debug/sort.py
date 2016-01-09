import os
import sys
from readmonifile import MonitorFile
 
class sort(object):
 	sHnumoverall=[]
	moni={}
	#log={}
	data={}
	
	def __init__(self, filename):
	
		self.data=self._sorting(filename)

	def _sorting(self, filename):
		
	    #Splitting up the file name
		name = os.path.splitext(filename)[0]
		ext = os.path.splitext(filename)[1]
		print filename
		if 'stringHub' in name:
			sHnum=name.partition('-')[2]
			self.sHnumoverall.append(int(sHnum))
			if 'moni' in ext:
				self.moni[sHnum] = MonitorFile(filename).dump_data()
	#		elif 'log' in ext:
	#			log[sHnum] = MonitorFile(f).dump_data()
			else:
				print "No stringHub moni files" 
			self.data['stringHub'] = self.moni		
		if 'inIceTrigger' in name:
			if 'moni' in ext:
				self.data['inIceTrigger'] = MonitorFile(filename).dump_data()
		if 'globalTrigger' in name:
			if 'moni' in ext:
				self.data['globalTrigger'] = MonitorFile(filename).dump_data()
		if 'eventBuilder' in name:
			if 'moni' in ext:
				self.data['eventBuilder'] = MonitorFile(filename).dump_data()
		if 'secondaryBuilders' in name:
			if 'moni' in ext:
				self.data['secondaryBuilders'] = MonitorFile(filename).dump_data()
		return self.data
	
	def out(self):
		return self.data