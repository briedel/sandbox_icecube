#FIXED eVa nle confusion 5-20-10, added flags

#Flags:


import matplotlib
matplotlib.use('TkAgg')

from pylab import plotfile,show,hist,figure,plot,axis,title
from numpy import array,linspace,where,sqrt, log,argmax,arange
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg,\
NavigationToolbar2TkAgg
from matplotlib.figure import Figure

from matplotlib.pyplot import * 

from Tkinter import *
from scipy.stats import norm
import matplotlib.pyplot as plt
import scipy.optimize
import sys
import scipy
import Tkinter as Tk
import sys
from neighborMethods import *


class App:
    def __init__(self,master,elFileName,dir='',pixel=-99,list=None):
        self.checkFlags()
        self.dir=dir
        self.elFileName=elFileName
        self.list=list
        if pixel!=-99:
            self.pixelNum=pixel
            self.pixel=numToa1_0(self.pixelNum)
        else: self.pixel=elFileName[0:4]
        self.master=master
        self.frame=master
        #self.frame.pack(side=RIGHT)
        self.master.bind('<Return>',self.callbackRegraph)
        
        #Graph surface
        self.graphSurface=Tk.Canvas(self.frame)
        self.f = Figure(figsize=(5,4), dpi=100)
#        self.a = self.f.add_subplot(111)

        self.canvas = FigureCanvasTkAgg(self.f, master=self.graphSurface)
        self.canvas.show()
        self.canvas.get_tk_widget().pack(side=Tk.TOP,fill=Tk.BOTH, expand=1)
        #toolbar = NavigationToolbar2TkAgg( self.canvas, self.graphSurface)
        #toolbar.update()
        
        self.graphSurface.grid(row=0, column=1, rowspan=8)
        #continue surface
        self.continueCanvas=Tk.Canvas(self.frame)
        self.saveAndContintueButton=Tk.Button(self.continueCanvas, \
                            text='Save and Continue',command=self.saveAndContinue)
        self.skipAndContintueButton=Tk.Button(self.continueCanvas,
                            text='Skip Peak and Continue',command=self.skipAndContinue)
        self.countAndContintueButton=Tk.Button(self.continueCanvas,
                            text='Count (w/o Gaussian),save and Continue',command=self.countAndContinue)

        self.saveAndContintueButton.grid(row=1)
        self.skipAndContintueButton.grid(row=2)
        self.countAndContintueButton.grid(row=3)

        self.continueCanvas.grid(row=0,column=0)

        #fitSurface
        self.fitCanvas=Tk.Canvas(self.frame)
        self.xlowLabel = Tk.Label(self.fitCanvas,text='xLow')
        self.xlowLabel.grid(row=0,column=0)
        self.xlowEntry = Tk.Entry(self.fitCanvas)
        self.xlowEntry.grid(row=0,column=1)

        self.xhighLabel = Tk.Label(self.fitCanvas,text='xHigh')
        self.xhighLabel.grid(row=1,column=0)
        self.xhighEntry = Tk.Entry(self.fitCanvas)
        self.xhighEntry.grid(row=1,column=1)

        self.boxesLabel = Tk.Label(self.fitCanvas,text='eV/bins')
        self.boxesLabel.grid(row=2,column=0)
        self.boxesEntry = Tk.Entry(self.fitCanvas)
        self.boxesEntry.grid(row=2,column=1)
        self.boxesLabel = Tk.Label(self.fitCanvas,text='start of Bin (0-1)')
        self.boxesLabel.grid(row=3,column=0)
        self.startBoxesEntry = Tk.Entry(self.fitCanvas)
        self.startBoxesEntry.grid(row=3,column=1)
        
        self.xNoiselowLabel = Tk.Label(self.fitCanvas,text='BKGD xlow')
        self.xNoiselowLabel.grid(row=4,column=0)
        self.xNoiselowEntry = Tk.Entry(self.fitCanvas)
        self.xNoiselowEntry.grid(row=4,column=1)
        self.xNoisehighLabel = Tk.Label(self.fitCanvas,text='BKGD xhigh')
        self.xNoisehighLabel.grid(row=5,column=0)
        self.xNoisehighEntry = Tk.Entry(self.fitCanvas)
        self.xNoisehighEntry.grid(row=5,column=1)


        
        self.xlowEntry.delete(0,Tk.END)
        self.xlowEntry.insert(0,".97")
        self.xhighEntry.delete(0,Tk.END)
        self.xhighEntry.insert(0,"1.03")
        self.xNoiselowEntry.delete(0,Tk.END)
        self.xNoiselowEntry.insert(0,".97")
        self.xNoisehighEntry.delete(0,Tk.END)
        self.xNoisehighEntry.insert(0,"1.03")
        self.boxesEntry.delete(0,Tk.END)
        self.boxesEntry.insert(0,"5")
        self.boxes=5
        self.startBoxesEntry.delete(0,Tk.END)
        self.startBoxesEntry.insert(0,"0")
        self.startBoxes=0


        
        
        self.fitCanvas.grid(row=1,column=0)

        #zoomOut
        self.zoomCanvas=Tk.Canvas(root)

        self.xZlowLabel = Tk.Label(self.zoomCanvas,text='xLow Window')
        self.xZlowLabel.grid(row=0,column=0)
        self.xZlowEntry = Tk.Entry(self.zoomCanvas)
        self.xZlowEntry.grid(row=0,column=1)

        self.xZhighLabel = Tk.Label(self.zoomCanvas,text='xHigh Window')
        self.xZhighLabel.grid(row=1,column=0)
        self.xZhighEntry = Tk.Entry(self.zoomCanvas)
        self.xZhighEntry.grid(row=1,column=1)


        
        self.yZhighLabel = Tk.Label(self.zoomCanvas,text='yHigh Window')
        self.yZhighLabel.grid(row=2,column=0)
        self.yZhighEntry = Tk.Entry(self.zoomCanvas)
        self.yZhighEntry.grid(row=2,column=1)
        
        #self.fullZoomOutButton=Tk.Button(self.zoomCanvas,text='Zoom out', command=self.zoomOut)
        #self.fullZoomOutButton.grid(row=2,column=0)
        self.zoomCanvas.grid(row=2,column=0)
        
        self.xZlowEntry.delete(0,Tk.END)
        self.xZlowEntry.insert(0,'0')
        self.xZhighEntry.delete(0,Tk.END)
        self.xZhighEntry.insert(0,"4000")
        self.yZhighEntry.delete(0,Tk.END)
        self.yZhighEntry.insert(0,"30")
      
        self.fitAll()
    def checkFlags(self):
        self.rateVersion=True
        return

    def fitAll(self):
        self.lineMap={}
        self.lineMap['C']=[277,0,150] #center, how much to look to right in eV
        self.lineMap['O']=[525,0,65]
        self.lineMap['F']=[677,0,600]
        self.lineMap['Al']=[1487,50,100]
        self.lineMap['Si']=[1740,150,50]
        self.lineMap['Zra']=[2041,75,25]
        self.lineMap['Zrb']=[2124,50,400]
        self.lineMap['Ka']= [3312.9,50,200]
        self.lineMap['Kb']=[3590,50,200]
        self.y=self.loadData(self.elFileName,self.pixel)
        y=self.y
        self.lines=['C','O','F','Al','Si','Zra','Zrb','Ka','Kb'] #old version
        self.lines=['C','O','F','Ka']
        #self.lines=['Ka','Kb']
        self.maxLineIndex=len(self.lines)
        if self.list!=None:
            self.lineIndex=self.lines.index(self.list[0])
            self.maxLineIndex=self.lines.index(self.list[0])+1
        elif '-s' in sys.argv:
            self.lineIndex=4
        else:
            self.lineIndex=0

        self.makeHistoGram(y)
        line,leftEV,rightEV=self.lineMap[self.line]
        dE=self.bincenters[1]-self.bincenters[0]
        self.dE=dE
        rightBins=int(round(rightEV/dE))
        leftBins=max(2,int(round(leftEV/dE)))
        
        def findClosestBin(bins,x):
            for index in range(len(bins)):
                if x<=bins[index]: return index
        lineIndex=findClosestBin(self.bincenters,line)
        ll,ul=self.bincenters[max(0,lineIndex-leftBins)],\
               self.bincenters[min(lineIndex+rightBins,len(self.bincenters)-1)]
        self.xlowEntry.delete(0,Tk.END)
        self.xlowEntry.insert(0,str(int(ll)))
        self.xhighEntry.delete(0,Tk.END)
        self.xhighEntry.insert(0,str(int(ul)))

        self.xNoiselowEntry.delete(0,Tk.END)
        self.xNoiselowEntry.insert(0,str(int(ll)))
        self.xNoisehighEntry.delete(0,Tk.END)
        self.xNoisehighEntry.insert(0,str(int(ul)))
        
        self.xZlowEntry.delete(0,Tk.END)
        self.xZlowEntry.insert(0,str(int(ll-(ul-ll)/3.)))
        self.xZhighEntry.delete(0,Tk.END)
        self.xZhighEntry.insert(0,str(int(ul+(ul-ll)/3.)))
        
        self.makeHistoGram(y)


    def makeHistoGram(self,y,fit=True):
        #print "FIT:",fit
        #w,h=self.f.get_figwidth(),f.get_figheight()
        self.y=y
        w,h=self.canvas.get_tk_widget().cget('width'), \
             self.canvas.get_tk_widget().cget('height')
        self.f.clf()

        self.ax=self.f.add_subplot(111)
        self.ax.grid(True)
        self.ax.set_ylabel("Counts per Bin")
        self.ax.set_xlabel("eV")
        if self.lineIndex>=len(self.lines) or \
               self.lineIndex>=self.maxLineIndex: #for single line fits
            print "all lines found, exiting."
            sys.exit(1)
        title=self.lines[self.lineIndex]+'--'+str(self.lineMap[self.lines[self.lineIndex]][0])+' eV'
        self.ax.set_title(title)
        ll=float(self.xlowEntry.get())
        ul=float(self.xhighEntry.get())

        nll=float(self.xNoiselowEntry.get())
        nul=float(self.xNoisehighEntry.get())        
        binsN=int(self.boxesEntry.get())
        if binsN==-1:
            binsN=self.findOptimalBinSize(y)
        self.binsN=binsN
        self.startBoxes=float(self.startBoxesEntry.get())
        if int(self.startBoxes)==-1:
            self.findOptimalShift(y)
            self.startBoxes=float(self.startBoxesEntry.get())
        binseV=arange(self.startBoxes*binsN,y.max()+binsN,binsN)
        n,bins,patches=self.ax.hist(y,bins=binseV)
        
        bincenters=.5*(bins[1:]+bins[:-1])#add best fit
        self.bincenters=bincenters
        tmp=where((ll<bincenters)&(ul>bincenters))
        n2=n[tmp]
        self.n2=n2
        self.n, self.bins, self.ll, self.ul = n,bins,ll,ul
        self.nll,self.nul=nll,nul
        if self.lineIndex<len(self.lines) and fit: self.fitGaussian()
        if self.boxes!=binsN: #changed bin number, redo y
            self.yZhighEntry.delete(0,Tk.END)
            self.yZhighEntry.insert(0,max(self.n2)*1.5)
            self.boxes=binsN
        self.ax.axis([float(self.xZlowEntry.get()),
                      float(self.xZhighEntry.get()),0,
                      float(self.yZhighEntry.get())])

        self.canvas.get_tk_widget().config(height=h,width=w)
        self.canvas.show()
    def findOptimalShift(self,y):
        print "finding Optimum shift!"
        ll=float(self.xlowEntry.get())
        ul=float(self.xhighEntry.get())

        nll=float(self.xNoiselowEntry.get())
        nul=float(self.xNoisehighEntry.get())        
        binsN=int(self.boxesEntry.get())
        self.binsN=binsN
        startBoxBest,residMin=0,1e6
        
        for startBox in arange(0,1,.05):
            
            binseV=arange(startBox*binsN,y.max()+binsN,binsN)
            clf()
            n,bins,patches=hist(y,bins=binseV)
            
            bincenters=.5*(bins[1:]+bins[:-1])#add best fit
            tmp=where((ll<bincenters)&(ul>bincenters))
            n2=n[tmp]
            self.n2=n2
            self.n, self.bins, self.ll, self.ul = n,bins,ll,ul
            self.nll,self.nul=nll,nul

            fitfunc = lambda p, x: p[3]+p[0]*scipy.exp(-(x-p[1])**2/(2.0*p[2]**2))
            errfunc = lambda p, x, y: fitfunc(p,x)-y
            self.fitfunc=fitfunc
            self.line=self.lines[self.lineIndex]
            n,bins,ll,ul=self.n,self.bins,self.ll,self.ul
            nll,nul=self.nll,self.nul
            line,leftEV,rightEV=self.lineMap[self.line]

            bincenters=.5*(bins[1:]+bins[:-1])#add best fit
            
            self.dE=bincenters[1]-bincenters[0]
            tmp=where((ll<bincenters)&(ul>bincenters))
            self.bkgdn2=n[where((nll<bincenters)&(nul>bincenters))]
            n2,b2=n[tmp],bincenters[tmp]
            self.n2=n2
            if len(n2)==0:
                n2,b2=n,bincenters
            sigma=self.findSigmaGuess(n2,self.dE)
            p0=array([max(n2),line,sigma,0.])
            if sum(n2)!=0:
                p1, success = scipy.optimize.leastsq(errfunc, p0.copy(), \
                                         args=(b2,n2))
                #print p1
            else: p1=array([max(n2),line,0,0.])
            self.p1=p1
            #print p1
            centerLine=abs(b2-line).argmin()
            residual=sum([errfunc(p1,b2[i],n2[i])**2 \
                          for i in range(centerLine-2,centerLine+3)])
            if residual<residMin:
                startBoxBest=startBox
                residMin=residual
                print startBox,residMin
        self.startBoxesEntry.delete(0,Tk.END)
        self.startBoxesEntry.insert(0,str(startBoxBest))
         
    def findOptimalBinSize(self,y):
        print "finding Optimum bin size!"
        ll=float(self.xlowEntry.get())
        ul=float(self.xhighEntry.get())

        nll=float(self.xNoiselowEntry.get())
        nul=float(self.xNoisehighEntry.get())        
        binsN=int(self.boxesEntry.get())
        self.binsN=binsN
        binBest,LriskMin=0,1e6
        for binTest in arange(5,20,1):
            
            binseV=arange(0,y.max()+binTest,binTest)
            n,bins,patches=hist(y,bins=binseV)

            bincenters=.5*(bins[1:]+bins[:-1])#add best fit
            tmp=where((ll<bincenters)&(ul>bincenters))
            n2=n[tmp]
            self.n2=n2
            self.n, self.bins, self.ll, self.ul = n,bins,ll,ul
            self.nll,self.nul=nll,nul

            fitfunc = lambda p, x: p[3]+p[0]*scipy.exp(-(x-p[1])**2/(2.0*p[2]**2))
            errfunc = lambda p, x, y: fitfunc(p,x)-y
            self.fitfunc=fitfunc
            self.line=self.lines[self.lineIndex]
            n,bins,ll,ul=self.n,self.bins,self.ll,self.ul
            nll,nul=self.nll,self.nul
            line,leftEV,rightEV=self.lineMap[self.line]

            bincenters=.5*(bins[1:]+bins[:-1])#add best fit
            
            self.dE=bincenters[1]-bincenters[0]
            tmp=where((ll<bincenters)&(ul>bincenters))
            self.bkgdn2=n[where((nll<bincenters)&(nul>bincenters))]
            n2,b2=n[tmp],bincenters[tmp]
            #print p1
            centerLine=abs(b2-line).argmin()
            leftLine,rightLine=abs(b2-(line-200)).argmin(), \
                               abs(b2-(line+200)).argmin()
            b2Gauss,n2Guass=b2[leftLine:rightLine],n2[leftLine:rightLine]
            #from wikipedia
            mbar=float(sum(n2Guass))/len(n2Guass)
            v=sum([(n2[i]-mbar)**2 for i in range(len(n2Guass))])/len(n2Guass)
            Lrisk=(2*mbar-v)/binTest**2
            print binTest,binBest,Lrisk
            if Lrisk<LriskMin:
                binBest=binTest
                LriskMin=Lrisk
                
        self.boxesEntry.delete(0,Tk.END)
        self.boxesEntry.insert(0,str(binBest))
        print "DONE",binBest
        return binBest
                        
    def fitGaussian(self):
        fitfunc = lambda p, x: p[3]+p[0]*scipy.exp(-(x-p[1])**2/(2.0*p[2]**2))
        errfunc = lambda p, x, y: fitfunc(p,x)-y
        self.fitfunc=fitfunc
        self.line=self.lines[self.lineIndex]
        n,bins,ll,ul=self.n,self.bins,self.ll,self.ul
        nll,nul=self.nll,self.nul
        line,leftEV,rightEV=self.lineMap[self.line]

        bincenters=.5*(bins[1:]+bins[:-1])#add best fit
        self.bincenters=bincenters
        self.dE=bincenters[1]-bincenters[0]
        tmp=where((ll<bincenters)&(ul>bincenters))
        self.bkgdn2=n[where((nll<bincenters)&(nul>bincenters))]
        n2,b2=n[tmp],bincenters[tmp]
        self.n2=n2
        if len(n2)==0:
            n2,b2=n,bincenters
        sigma=self.findSigmaGuess(n2,self.dE)
        p0=array([max(n2),line,sigma,0.])
        if sum(n2)!=0:
            p1, success = scipy.optimize.leastsq(errfunc, p0.copy(), \
                                     args=(b2,n2))
            #print p1
        else: p1=array([max(n2),line,0,0.])
        self.p1=p1
        #print p1
        counts,res,bkgd=self.getFitData(p1)
        print "Counts:", counts, "res:",res,"bkgd:",bkgd,"centroid:",p1[1]

        xG=linspace(ll,ul,500)
        self.ax.plot(xG,fitfunc(p1,xG),'r-')
        center=p1[1]
        self.ax.plot([line,line],[0,min(8,max(n2))*20],'y-')
        self.ax.plot([ll,ll],[0,min(8,max(n2))],'g--')
        self.ax.plot([ul,ul],[0,min(8,max(n2))],'g--')
        self.ax.plot([nll,nll],[0,min(8,max(n2))],'b--')
        self.ax.plot([nul,nul],[0,min(8,max(n2))],'b--')
    def findSigmaGuess(self,n2,dE):
        maxLineIn=argmax(n2)
        maxLine=max(n2)
        l=0.
        for i in range(0,len(n2)):
            if n2[i]>=maxLine/2.: l+=1
        fwhm=l*dE
        sigma=fwhm/(2.*sqrt(2*log(2)))
        #print "fwhm Guess:", fwhm
        return sigma
            

        
    def callbackRegraph(self,event):
        self.makeHistoGram(self.y)
        pass
    def saveAndContinue(self):
        self.saveData(self.p1)
        self.cntue()
        pass
    def skipAndContinue(self):
        self.saveData([-9999,-9999,-9999,-9999])
        self.cntue()
        pass
    def countAndContinue(self):
        #get bkgd counts
        print self.bkgdn2,"bkdgn2"
        bkgd=sum(self.bkgdn2)/(self.nul-self.nll)
        print self.nul-self.nll,bkgd
        suma=sum(self.n2)-bkgd*(self.ul-self.ll)
        self.saveData([suma,-9999,-9999,bkgd],getFit=False,count=True)

        
        self.cntue()
        pass
    def cntue(self):
        self.lineIndex+=1
        
        self.makeHistoGram(self.y,fit=False)
        self.line=self.lines[self.lineIndex]
        line,leftEV,rightEV=self.lineMap[self.line]
        dE=self.bincenters[1]-self.bincenters[0]
        rightBins=int(round(rightEV/dE))
        leftBins=max(2,int(round(leftEV/dE)))
        
        def findClosestBin(bins,x):
            for index in range(len(bins)):
                if x<=bins[index]: return index
        lineIndex=findClosestBin(self.bincenters,line)
        ll,ul=self.bincenters[max(0,lineIndex-leftBins)],\
               self.bincenters[min(lineIndex+rightBins,len(self.bincenters)-1)]
        print ll,ul,"<--ll,ul"
        self.xlowEntry.delete(0,Tk.END)
        self.xlowEntry.insert(0,str(int(ll)))
        self.xhighEntry.delete(0,Tk.END)
        self.xhighEntry.insert(0,str(int(ul)))

        self.xNoiselowEntry.delete(0,Tk.END)
        self.xNoiselowEntry.insert(0,str(int(ll)))
        self.xNoisehighEntry.delete(0,Tk.END)
        self.xNoisehighEntry.insert(0,str(int(ul)))

        self.xZlowEntry.delete(0,Tk.END)
        self.xZlowEntry.insert(0,str(int(ll-(ul-ll)/3.)))
        self.xZhighEntry.delete(0,Tk.END)
        self.xZhighEntry.insert(0,str(int(ul+(ul-ll)/3.)))
        
        self.yZhighEntry.delete(0,Tk.END)
        self.yZhighEntry.insert(0,max(self.n2)*1.5)
        self.boxes=0 #HACK, to make sure it rescales y
        self.makeHistoGram(self.y)
    def zoomOut(self):
        pass
    def loadData(self,elFileName,pixel=-1):
        import elFileMethods as el
        data=el.readEL(self.dir+elFileName)
        hash=el.makeHashLookUp()
        if self.pixelNum>=0:
            print data.shape,'OLD',
            data=data[where(data[:,hash['pix']]==self.pixelNum)]
            print data.shape, 'NEW'
        y=data[:,hash['nle']]
        index=where((data[:,hash['xmn']]<data[:,hash['ppi']]) & \
                        (data[:,hash['ppi']]<data[:,hash['xmx']]) & \
                        ((data[:,hash['suc']]==1) | (data[:,hash['suc']]==2)))
                        #(data[:,hash['suc']]==1))
        y=y[index]

        return y
    def getFitData(self,p):
        if p[0]==-9999: return -9999.,-9999.,-9999.
        counts=0
        bkgd=abs(p[3])
        #print "====== getFitData",p
        fitfunc = lambda p, x: p[0]*scipy.exp(-(x-p[1])**2/(2.0*p[2]**2))
        #note no bkdg in fitfunc
        for i in self.bincenters:
            counts+=fitfunc(p,i)
        fwhm=2*sqrt(2*log(2))*p[2]
        return counts,fwhm,bkgd/self.dE

     
    def saveData(self,p,getFit=True,count=False):
        #self.line
        try:
            
            f=open('rateData_in.txt','r')
            
        except:
            
            f=open('rateData_in.txt','w')
            print >>f,"#HEARDER"
            f.close()
            f=open('rateData_in.txt','r')
         
        a=[]
        i=0
        pI=-1
        for fline in f:
            if fline[0]=='#':
                continue
            s=fline.split()
            for i2 in range(1,len(s)):s[i2]=float(s[i2])
            a.append(s)
            if len(s)!=0 and s[0]==self.pixel:
                print "found line!"
                pI=i
            i+=1
        f.close()
        if pI==-1:
            b=[self.pixel]
            for i in range(len(self.lines)*3):b.append(float(-9999.))
            a.append(b)
            pI=len(a)-1
        if getFit:
            counts,fwhm,bkgd=self.getFitData(p)
        else:
            counts,fwhm,bkgd=p[0],-9999,p[3]
        i=self.lineIndex*3+1
        print len(a[pI]),i,self.lineIndex
        a[pI][i],a[pI][i+1],a[pI][i+2]=counts,fwhm,bkgd
        a.sort()

        f=open('rateData_in.txt','w')
        
        head="#HEADER"
        print >>f,head
        for i in a:
            print i
            ituple=tuple(i)
            #for index in range(1,len(ituple)):
            #    ituple[index]=float(ituple[index])
            #ituple=tuple(ituple)
            format='%s'+'\t%5.3f'*(len(i)-1)
            #print ituple
            print >>f,format % ituple
        f.close()

        
        #
        # Write out fit Parameters
        # Bin size, bin start xminFit, xmaxFit,xminNoise,xmaxNoise
        #
        #
        try:
            
            f=open('fitParamData_in.txt','r')
            
        except:
            
            f=open('fitParamData_in.txt','w')
            print >>f,"#Bin size, bin start, xminFit, xmaxFit,xminNoise,xmaxNoise"
            f.close()
            f=open('fitParamData_in.txt','r')  
        a=[]
        i=0
        pI=-1
        for fline in f:
            if fline[0]=='#':
                continue
            s=fline.split()
            for i2 in range(1,len(s)):s[i2]=float(s[i2])
            a.append(s)
            if len(s)!=0 and s[0]==self.pixel:
                print "found line!"
                pI=i
            i+=1
        f.close()
        if pI==-1:
            b=[self.pixel]
            for i in range(len(self.lines)*6):b.append(float(-9999.))
            a.append(b)
            pI=len(a)-1
        if getFit:
            counts,fwhm,bkgd=self.getFitData(p)
        else:
            counts,fwhm,bkgd=p[0],-9999,p[3]
        i=self.lineIndex*6+1
        a[pI][i],a[pI][i+1]= self.binsN,self.startBoxes
        a[pI][i+2],a[pI][i+3]=float(self.xlowEntry.get()),\
                               float(self.xhighEntry.get())
        a[pI][i+4],a[pI][i+5]=float(self.xNoiselowEntry.get()),\
                               float(self.xNoisehighEntry.get())
        a.sort()

        f=open('fitParamData_in.txt','w')
        
        head="#HEADER"
        print >>f,head
        for i in a:
            #print i
            ituple=tuple(i)
            #for index in range(1,len(ituple)):
            #    ituple[index]=float(ituple[index])
            #ituple=tuple(ituple)
            format='%s'+'\t%5.3f'*(len(i)-1)
            #print ituple
            print >>f,format % ituple
        f.close()


        #
        # Write out fit Centroids
        # centroid
        #
        #
        try:
            
            f=open('lineCentroids.txt','r')
            
        except:
            
            f=open('lineCentroids.txt','w')
            print >>f,"#Bin size, bin start, xminFit, xmaxFit,xminNoise,xmaxNoise"
            f.close()
            f=open('lineCentroids.txt','r')  
        a=[]
        i=0
        pI=-1
        for fline in f:
            if fline[0]=='#':
                continue
            s=fline.split()
            for i2 in range(1,len(s)):s[i2]=float(s[i2])
            a.append(s)
            if len(s)!=0 and s[0]==self.pixel:
                print "found line!"
                pI=i
            i+=1
        f.close()
        if pI==-1:
            b=[self.pixel]
            for i in range(len(self.lines)*1):b.append(float(-9999.))
            a.append(b)
            pI=len(a)-1

        i=self.lineIndex*1+1
        if count:
            a[pI][i]=average(self.y[where((self.y>self.ll) & (self.y<self.ul))])
        else: a[pI][i]=self.p1[1]
        a.sort()

        f=open('lineCentroids.txt','w')
        
        head="#pix C O F KKa centroids  "
        print >>f,head
        for i in a:
            #print i
            ituple=tuple(i)
            #for index in range(1,len(ituple)):
            #    ituple[index]=float(ituple[index])
            #ituple=tuple(ituple)
            format='%s'+'\t%5.3f'*(len(i)-1)
            #print ituple
            print >>f,format % ituple
        f.close()
        pass
    
lineMap={}
lineMap['C']=[277,0,150] #center, how much to look to right in eV
lineMap['O']=[525,0,65]
lineMap['F']=[677,0,600]
lineMap['Al']=[1487,50,100]
lineMap['Si']=[1740,150,50]
lineMap['Zra']=[2041,75,25]
lineMap['Zrb']=[2124,50,400]
lineMap['Ka']= [3312.9,50,200]
lineMap['Kb']=[3590,50,200]
       
if __name__=="__main__":

    from tkFileDialog import askopenfilename
    '''
    elFull=askopenfilename(title='Open el File', filetypes=[("cel file",'.cel'),
                                                          ('el file','.el')])
    selFull=elFull.split('/')
    el=selFull[-1]

    dire=elFull[0:len(elFull)-1-len(el)]
    dire+='/'
    print dire'''
    dire='./'
    el='prefl.cel'
    print el
    if '.cel' in el:
        s=raw_input('Enter in Pixl Number (-1 for all): ')
        i=int(s)
    else: i=-99
    line=None
    if '-l' in sys.argv:
        line=[sys.argv[sys.argv.index('-l')+1]]
        
    root=Tk.Tk()
    app=App(root,el,dir=dire,pixel=i,list=line)
    root.mainloop()  
    print "finished with pixel:",i
