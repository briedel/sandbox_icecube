###-------------------------------
# file: test.py
# simple demonstration of the Tkinter notebook

import Tkinter as tk
from notebook import *
data = {"Field 1": {"Field A":{"Field E":1,},},
        "Field 2": {"Field B":{"Field F":2,},},
        "Field 3": {"Field C":{"Field G":3,},},
        "Field 4": {"Field D":{"Field H":4,},},
}

class selectmonitype(tk.Tk):
    def __init__(self, *args, **kwargs):
        tk.Tk.__init__(self, *args, **kwargs)
        self.variables ={}
        self.subvariables={}
        n=notebook(tk.Tk(), LEFT)
        for label in sorted(data.keys()):
            self.variables[label] = tk.IntVar()
            f=Frame(n())
            n.add_screen(f, label)
            for sublabel in sorted(data[label][data[label].keys()[0]].keys()):
                self.subvariables[sublabel]=tk.IntVar()
                cb = tk.Checkbutton(self, text=sublabel, 
                                onvalue=data[label][data[label].keys()[0]][sublabel], offvalue=0, 
                                variable=self.subvariables[sublabel])
                cb.pack(side="top", fill="x")
            

            #b=tk.Button(self, text=label, 
            #            onvalue=data[label], offvalue=0,
            #            variable=self.variables[label])
            
            
            

'''
a = Tk()
n = notebook(a, LEFT)

# uses the notebook's frame
f1 = Frame(n())
b1 = Button(f1, text="Button 1")
e1 = Entry(f1)
# pack your widgets before adding the frame 
# to the notebook (but not the frame itself)!
b1.pack(fill=BOTH, expand=1)
e1.pack(fill=BOTH, expand=1)

f2 = Frame(n())
# this button destroys the 1st screen radiobutton
b2 = Button(f2, text='Button 2', command=lambda:x1.destroy())
b3 = Button(f2, text='Beep 2', command=lambda:Tk.bell(a))
b2.pack(fill=BOTH, expand=1)
b3.pack(fill=BOTH, expand=1)

f3 = Frame(n())

# keeps the reference to the radiobutton (optional)
x1 = n.add_screen(f1, "Screen 1")
n.add_screen(f2, "Screen 2")
n.add_screen(f3, "dummy")
'''
if __name__ == "__main__":
    selectmonitype().mainloop()
