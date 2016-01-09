# create a Periodic Table with multiple wxButton() widgets

import wx

class MyFrame(wx.Frame):
    def __init__(self, parent, mytitle, list_pt1, list_pt2):
        wx.Frame.__init__(self, parent, wx.ID_ANY, mytitle, pos=(0,20),
        size=(640, 330))
        self.SetBackgroundColour('green')

        # main sizer
        vsizer = wx.BoxSizer(wx.VERTICAL)

        # wx.GridSizer(rows, cols, vgap, hgap)
        # sizer for standard periodic table elements
        gsizer1 = wx.GridSizer(8, 18)

        font = wx.Font(10, wx.MODERN, wx.NORMAL, wx.BOLD)
        self.buttons1 = []
        for ix, symbol in enumerate(list_pt1):
            # forms a list of button objects, make certain id is unique
            btn_id = 100 + ix
            label = symbol
            self.buttons1.append(wx.Button(self, btn_id, label))
            self.Bind(wx.EVT_BUTTON, self.clickedButton, id=btn_id)
            self.buttons1[ix].SetFont(font)
            if symbol == " ":
                self.buttons1[ix].SetBackgroundColour("green")
            else:
                self.buttons1[ix].SetBackgroundColour("yellow")
            # the gridsizer fills left to right one row at a time
            gsizer1.Add(self.buttons1[ix], 0, wx.ALL|wx.EXPAND, border=1)

        # sizer for Lanthanides and Actinides
        gsizer2 = wx.GridSizer(3, 15)

        self.buttons2 = []
        for ix, symbol in enumerate(list_pt2):
            # forms a list of button objects, make certain id is unique
            btn_id = 300 + ix
            label = symbol
            #print label, ix, btn_id  # testing
            self.buttons2.append(wx.Button(self, btn_id, label))
            self.Bind(wx.EVT_BUTTON, self.clickedButton, id=btn_id)
            self.buttons2[ix].SetFont(font)
            self.buttons2[ix].SetBackgroundColour("pink")
            self.buttons2[ix].ClearBackground()
            # the gridsizer fills left to right one row at a time
            gsizer2.Add(self.buttons2[ix], 0, wx.ALL|wx.EXPAND, border=1)

        vsizer.Add(gsizer1, 0, wx.EXPAND)
        # spacer
        vsizer.Add((0, 30), 0, wx.EXPAND)
        vsizer.Add(gsizer2, 0, wx.EXPAND)
        #self.SetSizerAndFit(vsizer)
        self.SetSizer(vsizer)

    def clickedButton(self, event):
        # get the event id and then the associated button label
        buttons = self.buttons1 + self.buttons2
        for btn in buttons:
            if btn.GetId() == event.GetId():
                btn_label = btn.GetLabel()
        if btn_label == " ":
            message = " "
        elif btn_label == "*":
            message = "Position for the Lanthanides"
        elif btn_label == "**":
            message = "Position for the Actinides"
        else:
            message = "You clicked " + element_dict.get(btn_label, btn_label)
        #print event.GetId(), btn.GetId(), btn_label  # testing
        self.SetTitle(message)


# standard periodic table elements (q symbolizes an empty space)
#  * --> Lanthanides
# ** --> Actinides
# 18 items per line
pt1 = """\
H,q,q,q,q,q,q,q,q,q,q,q,q,q,q,q,q,He,
Li,Be,q,q,q,q,q,q,q,q,q,q,B,C,N,O,F,Ne,
Na,Mg,q,q,q,q,q,q,q,q,q,q,Al,Si,P,S,Cl,Ar,
K,Ca,Sc,Ti,V,Cr,Mn,Fe,Co,Ni,Cu,Zn,Ga,Ge,As,Se,Br,Kr,
Rb,Sr,Y,Zr,Nb,Mo,Tc,Ru,Rh,Pd,Ag,Cd,In,Sn,Sb,Te,I,Xe,
Cs,Ba,*,Hf,Ta,W,Re,Os,Ir,Pt,Au,Hg,Ti,Pb,Bi,Po,At,Rn,
Fr,Ra,**,Rf,Db,Sg,Bh,Hs,Mt,Ds,Rg,q,q,q,q,q,q,q"""

# Lanthanides and Actinides ...
# 15 items per line
pt2 = """\
La,Ce,Pr,Nd,Pm,Sm,Eu,Gd,Tb,Dy,Ho,Er,Tm,Yb,Lu,
Ac,Th,Pa,U,Np,Pu,Am,Cm,Bk,Cf,Es,Fm,Md,No,Lr"""

# convert standard periodic table elements into a list
list_pt1 = pt1.replace('\n', "").replace('q', " ").split(",")

# convert Lanthanides and Actinides into a list
list_pt2 = pt2.replace('\n', "").split(",")

element_dict = \
{'Ru': 'Ruthenium', 'Re': 'Rhenium', 'Ra': 'Radium', 'Rb': 'Rubidium',
'Rn': 'Radon', 'Rh': 'Rhodium', 'Be': 'Beryllium', 'Ba': 'Barium',
'Bi': 'Bismuth', 'Br': 'Bromine', 'H': 'Hydrogen', 'P': 'Phosphorus',
'Os': 'Osmium', 'Hg': 'Mercury', 'Ge': 'Germanium', 'Gd': 'Gadolinium',
'Ga': 'Gallium', 'Pr': 'Praseodymium', 'Pt': 'Platinum',
'Pu': 'Plutonium', 'C': 'Carbon', 'Pb': 'Lead ', 'Pa': 'Proctactinium',
'Pd': 'Palladium', 'Cd': 'Cadmium', 'Po': 'Polonium', 'Pm': 'Promethium',
'Ho': 'Holmium', 'Hf': 'Hafnium', 'K': 'Potassium', 'He': 'Helium',
'Mg': 'Magnesium', 'Mo': 'Molybdenum', 'Mn': 'Manganese', 'O': 'Oxygen',
'S': 'Sulfur', 'W': 'Tungsten', 'Zn': 'Zinc', 'Eu': 'Europium',
'Zr': 'Zirconium', 'Er': 'Erbium', 'Ni': 'Nickel', 'Na': 'Sodium',
'Nb': 'Niobium', 'Nd': 'Neodymium', 'Ne': 'Neon', 'Np': 'Neptunium',
'Fr': 'Francium', 'Fe': 'Iron', 'B': 'Boron', 'F': 'Fluorine',
'Sr': 'Strontium', 'N': 'Nitrogen', 'Kr': 'Krypton', 'Si': 'Silicon',
'Sn': 'Tin', 'Sm': 'Samarium', 'V': 'Vanadium', 'Sc': 'Scandium',
'Sb': 'Antimony', 'Se': 'Selenium', 'Co': 'Cobalt', 'Cl': 'Chlorine',
'Ca': 'Calcium ', 'Ce': 'Cerium', 'Xe': 'Xenon', 'Lu': 'Lutetium',
'Cs': 'Cesium', 'Cr': 'Chromium', 'Cu': 'Copper', 'La': 'Lanthanum',
'Li': 'Lithium', 'Tl': 'Thallium', 'Tm': 'Thulium', 'Th': 'Thorium',
'Ti': 'Titanium', 'Te': 'Tellurium', 'Tb': 'Terbium', 'Tc': 'Technetium',
'Ta': 'Tantalum', 'Yb': 'Ytterbium', 'Dy': 'Dysprosium', 'I': 'Iodine',
'U': 'Uranium', 'Y': 'Yttrium', 'Ac': 'Actinium', 'Ag': 'Silver',
'Ir': 'Iridium', 'Am': 'Americium', 'Al': 'Aluminum', 'As': 'Arsenic',
'Ar': 'Argon', 'Au': 'Gold', 'At': 'Astatine', 'In': 'Indium'}


app = wx.App(0)
# create the MyFrame instance and then show the frame
caption = "A muliple buttons periodic table"
MyFrame(None, caption, list_pt1, list_pt2).Show(True)
app.MainLoop()