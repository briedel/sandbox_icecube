#!/usr/bin/env python

#-----------------------------------------------------------------------------------
# GUI for pDAQ moni tool
#
# -briedel
# Last Changed: 11/10/2011
#
# 11/10/2011 - briedel 
# * Beta version done
# * Open Items: Mutiple Plots look bad
#
# 11/03/2011 - briedel
# * Got everything to work accordint to the dictionary keys. Had to rearrange the
#   dict to get everything to work and more easily get the data out later
#
#-----------------------------------------------------------------------------------

import os, sys, wx, math
import numpy as np
from sortmonifile import sort
import matplotlib
matplotlib.use('WXAgg')
from matplotlib.ticker import MaxNLocator
from matplotlib.figure import Figure
from matplotlib.figure import SubplotParams
from matplotlib.backends.backend_wxagg import \
    FigureCanvasWxAgg as FigCanvas, \
    NavigationToolbar2WxAgg as NavigationToolbar

#########################################################################
class plotFrame(wx.Frame):
    ###
    #Frame for the pop out plot
    ###
    def __init__(self, clicked, data):

        wx.Frame.__init__(self, wx.GetApp().TopWindow, title='pDAQ moni plot')
        
        self.l = len(clicked)

        self.p=wx.Panel(self)
        self.dpi=100
        self.fig = Figure()
        self.canvas = FigCanvas(self.p, -1, self.fig)
        clock=matplotlib.dates.DateFormatter('%H:%M:%S')
        matplotlib.font_manager.FontProperties(size=10)
        #Checking how many Checkboxes have been clicked and adjusting plotting accordingly
        if self.l == 1:
            times=sorted(data[clicked[0][0]][clicked[0][1]][clicked[0][2]][clicked[0][3]].keys())
            if clicked[0][3] == 'SNProfile': #Exeption handling for SN profiler dictionaries
                d=[]
                k=data[clicked[0][0]][clicked[0][1]][clicked[0][2]][clicked[0][3]][times[0]].keys()[0]
                for i in times:
                    d.append(data[clicked[0][0]][clicked[0][1]][clicked[0][2]][clicked[0][3]][i][k])
            else:
                d=[]
                for i in times:
                    d.append(data[clicked[0][0]][clicked[0][1]][clicked[0][2]][clicked[0][3]][i])
            self.splt = self.fig.add_subplot(1,1,1, ylabel=clicked[0][3])
            self.plt=self.splt.plot(times,d)
            self.splt.xaxis.set_minor_locator(matplotlib.ticker.AutoMinorLocator())
            self.splt.xaxis.set_major_formatter(clock)
        else:
            for i in np.arange(self.l):
                times=sorted(data[clicked[i][0]][clicked[i][1]][clicked[i][2]][clicked[i][3]].keys())
                if clicked[i][3] == 'SNProfile':
                    d=[]
                    k=data[clicked[i][0]][clicked[i][1]][clicked[i][2]][clicked[i][3]][times[0]].keys()[0]
                    for j in times:
                        d.append(data[clicked[i][0]][clicked[i][1]][clicked[i][2]][clicked[i][3]][j][k])
                else:
                    d=[]
                    for j in times:
                        d.append(data[clicked[i][0]][clicked[i][1]][clicked[i][2]][clicked[i][3]][j]) 
                if self.l==2:
                    self.splt=self.fig.add_subplot(2,1,1+i, ylabel=clicked[i][3])
                elif self.l%2==0 and self.l!=2:
                    self.splt=self.fig.add_subplot(math.ceil(math.sqrt(self.l)),math.ceil(math.sqrt(self.l)),1+i, ylabel=clicked[i][3])
                else:   
                    self.splt=self.fig.add_subplot(3,int(math.sqrt(self.l)),1+i, ylabel=clicked[i][3])
                self.splt.xaxis.set_minor_locator(matplotlib.ticker.AutoMinorLocator())
                self.splt.xaxis.set_major_formatter(clock)
                self.plt=self.splt.plot(times, d)
        self.splt.set_xlabel('Time') 
        
        

        self.toolbar = NavigationToolbar(self.canvas)

        #self.cb_grid = wx.CheckBox(self.p, -1, "Show Grid",style=wx.ALIGN_RIGHT)
        self.q=wx.Button(self.p, -1, "Quit", style=wx.ALIGN_BOTTOM)
        
        #self.Bind(wx.EVT_CHECKBOX, self.on_cb_grid, self.cb_grid)
        self.Bind(wx.EVT_BUTTON, self.OnQuit, self.q)

        self.s=wx.BoxSizer(wx.VERTICAL)
        self.h=wx.BoxSizer(wx.HORIZONTAL)

        flags = wx.ALIGN_LEFT | wx.ALL | wx.ALIGN_CENTER_VERTICAL
        self.s.Add(self.canvas, 1, wx.LEFT | wx.TOP | wx.GROW)
        self.h.Add(self.toolbar, 0, flags)
        self.h.Add(self.q, 0, flags)
        #self.h.Add(self.cb_grid, 0,  flags)
        self.h.AddSpacer(10)
        self.s.Add(self.h, 0, wx.TOP)
        self.p.SetSizer(self.s)
        self.s.Fit(self)  

    def OnQuit(self, event):
        self.Close()

    def on_cb_grid(self, event):
        self.draw_figure()

########################################################################
class NestedPanelOne(wx.Panel):
    ###
    # First notebook that creates the tab to select the component number
    ###
    def __init__(self, parent, label, data):

        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

        sizer = wx.BoxSizer(wx.VERTICAL)

        #Adding tabs from parsed contents
        nestedNotebook = wx.Notebook(self, wx.ID_ANY)
        for slabel in sorted(data[label].keys()):
            tab = NestedPanelTwo(nestedNotebook, label, slabel, data)
            nestedNotebook.AddPage(tab,slabel)

        
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(nestedNotebook, 1, wx.ALL|wx.EXPAND, 5)
        self.SetSizer(sizer)

########################################################################
class NestedPanelTwo(wx.Panel):
    ###
    # Second notebook that creates the tab to select the main monitoring variables 
    ###
    def __init__(self, parent, label, slabel, data):

        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

        sizer = wx.BoxSizer(wx.VERTICAL)
        
        nestedNotebook = wx.Notebook(self, wx.ID_ANY)

        for sslabel in sorted(data[label][slabel].keys()):
            tab = NestedPanelThree(nestedNotebook, label, slabel, sslabel, data)
            nestedNotebook.AddPage(tab, sslabel)
                    
        sizer.Add(nestedNotebook, 1, wx.ALL|wx.EXPAND, 5)
        self.SetSizer(sizer)

########################################################################
class NestedPanelThree(wx.Panel):
    ###
    # Third notebook that creates checkboxes to select the monitoring sub-variables
    ###
    def __init__(self, parent, label, slabel, sslabel, data):

        
        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

        self.chbox=[]

        # Adding Checkboxes
        for ssslabel in sorted(data[label][slabel][sslabel].keys()):
            self.cb=wx.CheckBox(self, -1, ssslabel)
            chboxdict[str(self.cb.GetId())]=[label,slabel,sslabel,ssslabel]
            wx.EVT_CHECKBOX(self, self.cb.GetId(), self.OnClick)
            self.chbox.append(self.cb)
        
        gs = wx.GridSizer(0,3)

        gs.AddMany(self.chbox)

        self.SetSizer(gs)

    def OnClick(self, event):
        if chboxdict[str(event.GetEventObject().GetId())] not in clicked:
            clicked.append(chboxdict[str(event.GetEventObject().GetId())])
        else:
            clicked.remove(chboxdict[str(event.GetEventObject().GetId())])

    #def OnClear(self):

       
########################################################################
class NestedNotebook(wx.Notebook):
    ###
    # Main notebook creating tabs for the monitored components
    ###
    def __init__(self, parent, data):
        wx.Notebook.__init__(self, parent, id=wx.ID_ANY, style=
                             wx.BK_DEFAULT
                            )
        
        for label in sorted(data.keys()):
            tab = NestedPanelOne(self,label, data)
            self.AddPage(tab, label)

########################################################################
class mainFrame(wx.Frame):
    ###
    # Putting it all together
    ###
    def __init__(self, data):

        wx.Frame.__init__(self, None, wx.ID_ANY,
                          "pDAQ monitoring plotting tool",
                          size=(800,400)
                          )

        panel = wx.Panel(self)
        notebook = NestedNotebook(panel, data)
        
        sizer = wx.BoxSizer(wx.VERTICAL)
        bsizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(notebook, 1, wx.ALL|wx.EXPAND, 5)

        g=wx.Button(panel, -1, label='Graph')
        q=wx.Button(panel, -1, label='Quit')
        gclear=wx.Button(panel, -1, label='Clear All')
        bsizer.Add(g, 0, wx.ALL, 5)
        bsizer.Add(q, 0, wx.ALL, 5)
        bsizer.Add(gclear,0,wx.ALL, 5)
        sizer.Add(bsizer, 0 , wx.ALL|wx.CENTER,5)

        self.Bind(wx.EVT_BUTTON, self.OnQuit,q)
        self.Bind(wx.EVT_BUTTON, self.OnGraph,g)
        #self.Bind(wx.EVT_BUTTON, self.OnClear,gclear)
        panel.SetSizer(sizer)
        self.Layout()

        self.Show()

    def OnQuit(self, event):
        self.Close()
    
    def OnGraph(self, event):
        plotFrame(clicked, data).Show()
    
    #def OnClear(self,event):


#----------------------------------------------------------------------
if __name__ == "__main__":
    if len(sys.argv) == 1:
        raise SystemExit("Please specify a file to process")

    for f in sys.argv[1:]:
        data=sort.sorting(f)
    
    #Need to get ride of these global variables
    chboxdict={}
    clicked=[]
    app = wx.PySimpleApp()
    frame = mainFrame(data)
    app.MainLoop()