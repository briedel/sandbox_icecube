import wx
import wx.lib.colourselect as csel

import CustomCheckBox as CCB


#----------------------------------------------------------------------
def GetMondrianData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x08\x06\x00\
\x00\x00szz\xf4\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\x00qID\
ATX\x85\xed\xd6;\n\x800\x10E\xd1{\xc5\x8d\xb9r\x97\x16\x0b\xad$\x8a\x82:\x16\
o\xda\x84pB2\x1f\x81Fa\x8c\x9c\x08\x04Z{\xcf\xa72\xbcv\xfa\xc5\x08 \x80r\x80\
\xfc\xa2\x0e\x1c\xe4\xba\xfaX\x1d\xd0\xde]S\x07\x02\xd8>\xe1wa-`\x9fQ\xe9\
\x86\x01\x04\x10\x00\\(Dk\x1b-\x04\xdc\x1d\x07\x14\x98;\x0bS\x7f\x7f\xf9\x13\
\x04\x10@\xf9X\xbe\x00\xc9 \x14K\xc1<={\x00\x00\x00\x00IEND\xaeB`\x82' 

def GetMondrianBitmap():
    return wx.BitmapFromImage(GetMondrianImage())

def GetMondrianImage():
    import cStringIO
    stream = cStringIO.StringIO(GetMondrianData())
    return wx.ImageFromStream(stream)

def GetMondrianIcon():
    icon = wx.EmptyIcon()
    icon.CopyFromBitmap(GetMondrianBitmap())
    return icon


#---------------------------------------------------------------------------
# Show how to derive a custom wxLog class

class MyLog(wx.PyLog):

    def __init__(self, textCtrl, logTime=0):

        wx.PyLog.__init__(self)
        self.tc = textCtrl
        self.logTime = logTime


    def DoLogString(self, message, timeStamp):

        if self.tc:
            self.tc.AppendText(message + '\n')

#---------------------------------------------------------------------------

class CustomCheckBoxDemo(wx.Frame):

    def __init__(self, parent, id=wx.ID_ANY, title="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE):

        wx.Frame.__init__(self, parent, id, title, pos, size, style)
        self.SetIcon(GetMondrianIcon())

        self.mainPanel = wx.Panel(self, -1)

        # Set up a log window
        self.logWindow = wx.TextCtrl(self.mainPanel, -1, size=(-1, 100),
                                     style = wx.TE_MULTILINE|wx.TE_READONLY|wx.HSCROLL)
        # Set the wxWidgets log target to be this textctrl
        wx.Log_SetActiveTarget(MyLog(self.logWindow))

        # Create some CustomCheckBoxes
        self.ccb1 = CCB.CustomCheckBox(self.mainPanel, -1, "Hello wxPython!")
        self.ccb2 = CCB.CustomCheckBox(self.mainPanel, -1, "CustomCheckBox 1")
        self.ccb3 = CCB.CustomCheckBox(self.mainPanel, -1, "CustomCheckBox 2")
        self.ccb4 = CCB.CustomCheckBox(self.mainPanel, -1, "CustomCheckBox 3")

        # Create a button that enables/disables the first CustomCheckBox
        self.buttonEnabler = wx.Button(self.mainPanel, -1, "Disable")

        # Create a button that "toggles" the CustomCheckBox state
        self.buttonToggler = wx.Button(self.mainPanel, -1, "Toggle")

        # Create a button that changes the font of self.ccb2
        self.buttonFont = wx.Button(self.mainPanel, -1, "Set Font")

        # Create a button that changes the background colour of self.ccb3
        backColour = self.ccb3.GetBackgroundColour()
        self.buttonBack = csel.ColourSelect(self.mainPanel, -1, "Background",
                                            backColour)

        # Create a button that changes the foreground colour of self.ccb4
        foreColour = self.ccb4.GetForegroundColour()
        self.buttonFore = csel.ColourSelect(self.mainPanel, -1, "Foreground",
                                            foreColour)
        
        # Layout the items with sizers
        self.LayoutItems()

        # Bind the events
        self.BindEvents()


    def LayoutItems(self):

        mainSizer = wx.BoxSizer(wx.VERTICAL)
        panelSizer = wx.BoxSizer(wx.VERTICAL)
        centerSizer = wx.BoxSizer(wx.HORIZONTAL)
        leftSizer = wx.BoxSizer(wx.HORIZONTAL)
        buttonSizer = wx.BoxSizer(wx.VERTICAL)
        rightSizer = wx.BoxSizer(wx.VERTICAL)
        checkSizer1 = wx.BoxSizer(wx.HORIZONTAL)
        checkSizer2 = wx.BoxSizer(wx.HORIZONTAL)
        checkSizer3 = wx.BoxSizer(wx.HORIZONTAL)

        # Add the buttons to the right sizer
        buttonSizer.Add(self.buttonEnabler, 0, wx.ALL, 5)
        buttonSizer.Add(self.buttonToggler, 0, wx.ALL, 5)
        leftSizer.Add(self.ccb1, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 10)
        leftSizer.Add(buttonSizer, 0, wx.ALIGN_CENTER_VERTICAL)

        # Add the other 3 CustomCheckBoxes and their buttons
        checkSizer1.Add(self.ccb2, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL, 10)
        checkSizer1.Add(self.buttonFont, 0)
        checkSizer2.Add(self.ccb3, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL, 10)
        checkSizer2.Add(self.buttonBack, 0)
        checkSizer3.Add(self.ccb4, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL, 10)
        checkSizer3.Add(self.buttonFore, 0)
        
        rightSizer.Add(checkSizer1, 0, wx.BOTTOM, 5)
        rightSizer.Add(checkSizer2, 0, wx.BOTTOM, 5)
        rightSizer.Add(checkSizer3, 0)

        centerSizer.Add(leftSizer, 0, wx.ALL, 30)
        centerSizer.Add(rightSizer, 0, wx.TOP|wx.LEFT, 25)

        panelSizer.Add(centerSizer, 1, wx.EXPAND)
        panelSizer.Add(self.logWindow, 0, wx.EXPAND)

        # Set the sizer to the panel        
        self.mainPanel.SetSizer(panelSizer)

        # Add the panel to the frame sizer        
        mainSizer.Add(self.mainPanel, 1, wx.EXPAND)
        
        self.SetSizer(mainSizer)
        mainSizer.Layout()


    def BindEvents(self):

        # Ok, let's bind some events...
        
        # All the CustomCheckBoxes go to the same event handler, as we don't
        # need a separate handler for every CustomCheckBox
        self.Bind(wx.EVT_CHECKBOX, self.OnCheckBox, self.ccb1)
        self.Bind(wx.EVT_CHECKBOX, self.OnCheckBox, self.ccb2)
        self.Bind(wx.EVT_CHECKBOX, self.OnCheckBox, self.ccb3)
        self.Bind(wx.EVT_CHECKBOX, self.OnCheckBox, self.ccb4)

        # Now we take care of the buttons
        self.Bind(wx.EVT_BUTTON, self.OnEnable, self.buttonEnabler)
        self.Bind(wx.EVT_BUTTON, self.OnToggle, self.buttonToggler)
        self.Bind(wx.EVT_BUTTON, self.OnFont, self.buttonFont)

        self.buttonBack.Bind(csel.EVT_COLOURSELECT, self.OnBackground)
        self.buttonFore.Bind(csel.EVT_COLOURSELECT, self.OnForeground)


    def OnCheckBox(self, event):

        # Grab the CustomCheckBox that generated the event
        control = event.GetEventObject()

        # Get its label
        label = control.GetLabel()

        # Get the checked/unchecked value
        value = event.IsChecked()

        # Display the label and some info in the wx.LogTextCtrl
        self.logWindow.AppendText("CustomCheckBox event from ==> " + label + \
                                  ", checked = " + repr(value) + "\n")


    def OnEnable(self, event):

        if self.ccb1.IsEnabled():
            # we are enabled, so let's disable ourselves and also change the
            # button label to "Enable"
            self.ccb1.Enable(False)
            self.buttonEnabler.SetLabel("Enable")
        else:
            # we are disabled, so let's enable ourselves and also change the
            # button label to "Disable"
            self.ccb1.Enable(True)
            self.buttonEnabler.SetLabel("Disable")
            

    def OnToggle(self, event):

        if self.ccb1.IsChecked():
            # we are checked, so let's go for unchecking...
            self.ccb1.SetValue(0)
        else:
            # we are unchecked, so let's go for checking...
            self.ccb1.SetValue(1)
            

    def OnFont(self, event):

        initialFont = self.ccb2.GetFont()
        if not initialFont:
            font = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
            
        data = wx.FontData()
        data.SetInitialFont(initialFont)
        dlg = wx.FontDialog(self, data)
        
        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetFontData()
            font = data.GetChosenFont()
            self.ccb2.SetFont(font)
            self.ccb2.GetContainingSizer().Layout()
            
        # Don't destroy the dialog until you get everything you need from the
        # dialog!
        dlg.Destroy()

        
    def OnBackground(self, event):

        # We change the background colour
        colour = event.GetValue()        
        self.ccb3.SetBackgroundColour(colour) 
        

    def OnForeground(self, event):

        # We change the foreground colour
        colour = event.GetValue()        
        self.ccb4.SetForegroundColour(colour) 


def main():

    app = wx.PySimpleApp()
    frame = CustomCheckBoxDemo(None, -1, "CustomCheckBox wxPython Demo ;-)",
                               size=(600, 400))
    frame.CenterOnScreen()
    frame.Show()
    app.MainLoop()


if __name__ == "__main__":
    main()

    