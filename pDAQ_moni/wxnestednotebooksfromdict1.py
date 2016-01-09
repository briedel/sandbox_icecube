#!/usr/bin/python
#!/usr/bin/env python
import os
import sys
import datetime
from readmonifile import MonitorFile
from sortmonifile import sort
import wx

class PanelOne(wx.Panel):
    """
    This will be the first notebook tab
    """
    #----------------------------------------------------------------------
    def __init__(self, parent):

        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

class NestedPanel(wx.Panel):
    """
    This will be the first notebook tab
    """
    #----------------------------------------------------------------------
    def __init__(self, parent, slabel, data):

        wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)

        sizer = wx.BoxSizer(wx.VERTICAL)

        # Create some nested tabs on the first tab
        nestedNotebook = wx.Notebook(self, wx.ID_ANY)
        for sslabel in sorted(data[slabel].keys()):
            tab = PanelOne(nestedNotebook)
            print sslabel
            nestedNotebook.AddPage(tab,sslabel)
            for ssslabel in sorted(data[slabel][sslabel][data[slabel][sslabel].keys()[0]].keys()):
                print ssslabel, 
                print data[slabel][sslabel][data[slabel][sslabel].keys()[0]].keys()
                wx.CheckBox(tab, -1,ssslabel)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(nestedNotebook, 1, wx.ALL|wx.EXPAND, 5)

        self.SetSizer(sizer)


########################################################################
class NestedNotebookDemo(wx.Notebook):
    """
    Notebook class
    """

    #----------------------------------------------------------------------
    def __init__(self, parent, data):
        wx.Notebook.__init__(self, parent, id=wx.ID_ANY, style=
                             wx.BK_DEFAULT
                             #wx.BK_TOP 
                             #wx.BK_BOTTOM
                             #wx.BK_LEFT
                             #wx.BK_RIGHT
                             )
        
        for label in sorted(data.keys()):
            print label
            tab = NestedPanel(self,label, data)
            self.AddPage(tab, label)

        self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.OnPageChanged)
        self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGING, self.OnPageChanging)

    def OnPageChanged(self, event):
        old = event.GetOldSelection()
        new = event.GetSelection()
        sel = self.GetSelection()
        print 'OnPageChanged,  old:%d, new:%d, sel:%d\n' % (old, new, sel)
        event.Skip()

    def OnPageChanging(self, event):
        old = event.GetOldSelection()
        new = event.GetSelection()
        sel = self.GetSelection()
        print 'OnPageChanging, old:%d, new:%d, sel:%d\n' % (old, new, sel)
        event.Skip()


########################################################################
class DemoFrame(wx.Frame):
    """
    Frame that holds all other widgets
    """

    #----------------------------------------------------------------------
    def __init__(self,data):
        """Constructor"""
        wx.Frame.__init__(self, None, wx.ID_ANY,
                          "pDAQ monitoring plotting tool",
                          size=(600,400)
                          )
        panel = wx.Panel(self)

        notebook = NestedNotebookDemo(panel, data)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(notebook, 1, wx.ALL|wx.EXPAND, 5)
        panel.SetSizer(sizer)
        self.Layout()

        self.Show()

#----------------------------------------------------------------------
if __name__ == "__main__":
    '''
    data = {"Field 1": {"Field A":{"Field P":1,},},
        "Field 2": {"Field B":{"Field R":2,},},
        "Field 3": {"Field C":{"Field S":3,},},
        "Field 4": {"Field D":{"Field T":4,},},
        "Field 5": {"Field E":{"Field U":5,},},
        "Field 6": {"Field F":{"Field V":6,},},
        "Field 7": {"Field G":{"Field W":7,},},
        "Field 8": {"Field H":{"Field X":8,},},
        "Field 9": {"Field I":{"Field Y":9,},},
        "Field 10": {"Field J":{"Field Z":10,},},
    }'''
    if len(sys.argv) == 1:
        raise SystemExit("Please specify a file to process")

    for f in sys.argv[1:]:
        data=sort(f).out()
        print sorted(data['stringHub']['1001'].keys()),data['stringHub'].keys()
        print data.keys()
        print data[data.keys()[0]].keys()
    
    print 'test'
    app = wx.PySimpleApp()
    frame = DemoFrame(data)
    app.MainLoop()
    print 'testend'