#!/usr/bin/env python
import sys
import matplotlib
matplotlib.use('TkAgg')
from pylab import *
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure
from matplotlib.pyplot import *
from Tkinter import *
from scipy.stats import norm
import matplotlib.pyplot as plt
import scipy,sys,struct,os
import Tkinter as Tk


class App:
    def __init__(self,master,listOfBaseLineFiles):
        self.listName=listOfBaseLineFiles
        self.listOfFiles=[]
        f=open(self.listName,'r')
        for line in f:
            self.listOfFiles.append(line.split())
        self.index=0
        self.master,self.frame=master,master
        master.bind('<Return>',self.writeSW)
        master.bind('n',self.write9W)
        master.bind('g',self.write0W)
        master.bind('b',self.write1W)
        #graphing stuff
        self.graphSurface=Tk.Canvas(self.frame)
        self.f = Figure(figsize=(6,3),dpi=100)
        self.canvas = FigureCanvasTkAgg(self.f, master=self.master)
        self.canvas.show()
        self.canvas.get_tk_widget().pack(side=Tk.LEFT, fill=Tk.BOTH, expand=1)
        self.toolbar = NavigationToolbar2TkAgg( self.canvas, self.master)
        self.toolbar.update()
        self.canvas._tkcanvas.pack(side=Tk.LEFT, fill=Tk.BOTH, expand=1)

        #input and continue surface
        self.IOsurface=Tk.Canvas(master=self.frame)
        
        self.classLabel = Tk.Label(self.IOsurface,text='Classification')
        self.classLabel.grid(row=1,column=0,columnspan=3)
        self.Button9=Tk.Button(self.IOsurface,text='  -9  ',command=self.write9)
        self.Button1=Tk.Button(self.IOsurface,text='  1  ',command=self.write1)
        self.Button0=Tk.Button(self.IOsurface,text='  0  ',command=self.write0)
        self.ButtonS=Tk.Button(self.IOsurface,text='Skip',command=self.writeS)
        self.Button9.grid(row=2,column=0)
        self.Button1.grid(row=2,column=2)
        self.Button0.grid(row=2,column=1)
        self.ButtonS.grid(row=2,column=3)
        self.skipLabel=Tk.Label(self.IOsurface,text="Skip to line number")
        self.skipEntry=Tk.Entry(self.IOsurface)
        self.skipEntry.insert(0,"0")
        self.skipLabel.grid(row=3,column=0, columnspan=2)
        self.skipEntry.grid(row=3,column=2)
        self.infoString=StringVar()
        self.infoLabel=Tk.Label(self.IOsurface,textvariable=self.infoString)
        self.infoLabel.grid(row=0,column=0,columnspan=3)
        
#*#*#*#*#*#*#**#*#*#*#*#*#*#*#*#*
        
        l=r"""Line number starts at 1 (as does emacs).
             Enter in -9 to go to the first entry with -9.
             you can hit 'g' for 0, 'b' for 1 and 'n' fo -9.
             <return> will skip the current graph and retain the value"""
        t=Tk.Label(self.IOsurface,text=l)
        t.grid(row=20,columnspan=3)
        self.IOsurface.pack()    
        self.graphNext()
    def write9(self): self.write(-9)
    def write1(self): self.write(1)
    def write0(self): self.write(0)
    def writeS(self): self.write(-9999)
    def writeSW(self,t): self.writeS()
    def write0W(self,t): self.write0()
    def write1W(self,t): self.write1()
    def write9W(self,t): self.write9()
    def write(self,n):
        print self.index,"INDEX"
        if n!=-9999:
            self.listOfFiles[self.index-1][3]=str(n)
        f=open(self.listName,'w')
        for line in self.listOfFiles:
            print >>f, line[0]+'\t',
            print >>f, line[1]+'\t',
            print >>f,line[2]+'\t',
            print >>f, line[3]+'\t'
        f.close()
        self.graphNext()
    def graphNext(self):
        if self.index==len(self.listOfFiles):
            print 'End of list of files!',self.index,len(self.listOfFiles)
            #showwarning('End of list of files!')
        elif self.skipEntry.get()=='':
            pass
        elif self.index==int(self.skipEntry.get()):
            self.index+=1
            self.skipEntry.delete(0,len(self.skipEntry.get()))
            self.skipEntry.insert(0,self.index)
       
        else:
            try:
                self.index=int(self.skipEntry.get())
            except:
                self.master.showwarning('skip is not an integer? current index is '+str(self.index))
                print 'skip is not an integer? current index is '+str(self.index)
            if int(self.skipEntry.get())==-9:
                for i in range(len(self.listOfFiles)):
                    #print self.listOfFiles[i][3]+0,"<<<<"
                    if int(self.listOfFiles[i][3])==-9:
                        
                        self.index=i+1
                        break
                self.skipEntry.delete(0,len(self.skipEntry.get()))
                self.skipEntry.insert(0,str(self.index))
        if self.index>=len(self.listOfFiles)-1: return

        fileName,s,e,currentClass=self.listOfFiles[self.index-1]
        self.infoString.set(' '.join((fileName,s,e,currentClass)))
        s,e,currentClass=int(float(s)),int(float(e)),int(currentClass)
        f=open(fileName,'rb')
        fff=struct.calcsize('fff')
        fileStart=int(struct.unpack('<fff',f.read(fff))[0])
        
        s+=-fileStart
        e+=-fileStart
        f.seek(s*fff)
        d=zeros([e-s,2])
        for i in range(e-s):
            t=struct.unpack('<fff',f.read(fff))[0:2]
            d[i]=t
        self.f.clf()
        self.ax=self.f.add_subplot(111)
        self.ax.plot(d[:,0],d[:,1],'r-')
        
        self.canvas.show()
        
if __name__=='__main__':
    master=Tk.Tk()
    app=App(master,sys.argv[1])
    master.mainloop()
