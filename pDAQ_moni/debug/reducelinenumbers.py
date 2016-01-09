#!/usr/bin/env python
import os
import sys
import datetime
import numpy as np
from readmonifile import MonitorFile
from sortmonifile import sort
import wx

class NestedPanelOne(wx.Panel):
    #----------------------------------------------------------------------
    # First notebook that creates the tab to select the component number
    #----------------------------------------------------------------------
    def __init__(self, parent, label, data):

        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

        sizer = wx.BoxSizer(wx.VERTICAL)

        #Loop creating the tabs according to the component name
        nestedNotebook = wx.Notebook(self, wx.ID_ANY)
        for slabel in sorted(data[label].keys()):
            tab = NestedPanelTwo(nestedNotebook, label, slabel, data)
            nestedNotebook.AddPage(tab,slabel)


        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(nestedNotebook, 1, wx.ALL|wx.EXPAND, 5)

        self.SetSizer(sizer)

class NestedPanelTwo(wx.Panel):
    #------------------------------------------------------------------------------
    # Second notebook that creates the tab to select the main monitoring variables 
    #------------------------------------------------------------------------------
    def __init__(self, parent, label, slabel, data):

        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

        sizer = wx.BoxSizer(wx.VERTICAL)
        
        nestedNotebook = wx.Notebook(self, wx.ID_ANY)

        for sslabel in sorted(data[label][slabel][data[label][slabel].keys()[0]].keys()):
            tab = NestedPanelThree(nestedNotebook, label, slabel, sslabel, data)
            nestedNotebook.AddPage(tab, sslabel)
                    
        sizer.Add(nestedNotebook, 1, wx.ALL|wx.EXPAND, 5)
        self.SetSizer(sizer)

class NestedPanelThree(wx.Panel):
    #-------------------------------------------------------------------------------
    # Third notebook that creates checkboxes to select the monitoring sub-variables
    #-------------------------------------------------------------------------------
    def __init__(self, parent, label, slabel, sslabel, data):

        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

        labels=[]
        chbox =[]
        chboxdict={}
        for ssslabel in sorted(data[label][slabel][data[label][slabel].keys()[0]][sslabel].keys()):
            labels.append(ssslabel)

        for item in list(set(labels)):
            cb = wx.CheckBox(self, -1, item)
            chbox.append(cb)
            chboxdict[item]=cb

        gridSizer = wx.GridSizer(np.shape(list(set(labels)))[0],3, 5, 5)

        gridSizer.AddMany(chbox)
        self.SetSizer(gridSizer)

########################################################################
class NestedNotebook(wx.Notebook, wx.Panel):
    #---------------------------------------------------------------------------------
    # Main notebook creating tabs for the monitored components
    #---------------------------------------------------------------------------------
    def __init__(self, parent, data):
        #wx.Notebook.__init__(self, parent, id=wx.ID_ANY, style=
        #                     wx.BK_DEFAULT
        #                    )
        wx.Frame.__init__(self,parent,wx.ID_ANY, title,
                            style=wx.DEFAULT_FRAME_STYLE)
        
        self.P=wx.Panel(self, wx.ID_ANY)
        self.nP=wx.Panel(self.P, wx.ID_ANY)
        self.nnP=wx.Panel(self.nP, wx.ID_ANY)

        self.Nb = wx.Notebook(self.P,wx.ID_ANY)
        self.nNb = wx.Notebook(self.nP,wx.ID_ANY)
        self.nnNb = wx.Notebook(self.nnP,wx.ID_ANY)
        
        #panel=wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)
        for label in sorted(data.keys()):
            self.SubNoteBooks[label] = wx.Notebook(self.Nb, wx.ID_ANY)
            self.Notebook.AddPage(self.SubNoteBooks[label], label)
            for slabel in sorted(data[label].keys()):
                self.SubNoteBooks[label][sublabel] = wx.Notebook(self.SubNoteBooks[label], wx.ID_ANY)
                self.Notebook.AddPage(self.SubNoteBooks[label][slabel], slabel)
                for sslabel in sorted(data[label][slabel][data[label][slabel].keys()[0]].keys()):
                    self.SubNoteBooks[label][sublabel][sslabel] = wx.Notebook(self.SubNoteBooks[label][sublabel], wx.ID_ANY)
                    self.Notebook.AddPage(self.SubNoteBooks[label][slabel][sslabel], sslabel)
            #for slabel in sorted(data[label].keys()):
            #    self.AddPage()

########################################################################
class DemoFrame(wx.Frame):
    #----------------------------------------------------------------------
    # Putting it all together
    #----------------------------------------------------------------------
    SubNoteBooks = {}
    def __init__(self,data):

        wx.Frame.__init__(self, None, wx.ID_ANY,
                          "pDAQ monitoring plotting tool",
                          size=(800,400)
                          )

        panel = wx.Panel(self)

        self.Notebook = wx.Notebook(self,wx.ID_ANY)
        self.nNb=wx.Notebook(self,wx.ID_ANY)
        
        #panel=wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)
        for label in sorted(data.keys()):
            self.SubNoteBooks[label] = wx.Notebook(self.Notebook, wx.ID_ANY)
            self.Notebook.AddPage(self.SubNoteBooks[label], label)
            for slabel in sorted(data[label].keys()):
                self.SubNoteBooks[label][slabel] = wx.Notebook(self.SubNoteBooks[label], wx.ID_ANY)
                self.SubNoteBooks[label].AddPage(self.SubNoteBooks[label][slabel], slabel)
                #for sslabel in sorted(data[label][slabel][data[label][slabel].keys()[0]].keys()):
                #    self.SubNoteBooks[label][slabel][sslabel] = wx.Notebook(self.Notebook, wx.ID_ANY)
                #    self.Notebook.AddPage(self.SubNoteBooks[label][slabel][sslabel], sslabel)

        #notebook = NestedNotebook(panel, data)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.Notebook, 1, wx.ALL|wx.EXPAND, 5)
        panel.SetSizer(sizer)
        self.Layout()

        #Menu Bar to be added later
        '''
        menubar = wx.MenuBar()
        file = wx.Menu()
        file.Append(1, '&Quit', 'Exit Tool')
        menubar.Append(file, '&File')
        self.SetMenuBar(menubar)
        self.Bind(wx.EVT_MENU, self.OnClose, id=1)
        '''
        self.Show()
#----------------------------------------------------------------------
if __name__ == "__main__":

    if len(sys.argv) == 1:
        raise SystemExit("Please specify a file to process")

    for f in sys.argv[1:]:
        data=sort.sorting(f)
        print data['stringHub'].keys()
        print data.keys()
        print data[data.keys()[0]].keys()
    
    print 'test'
    app = wx.PySimpleApp()
    frame = DemoFrame(data)
    app.MainLoop()
    print 'testend'