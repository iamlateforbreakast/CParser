import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk

taskList = [("Download latest Rapbian","Not selected"),
	    ("Configure hostname","Not selected"),
	    ("Configure Wifi","Not selected"),
	    ("Enable SSH","Not selected"),
	    ("Enable SAMBA","Not selected")]

class Task(object):

  def __init__(self, str name):
    self.name = name

class MyWindow(Gtk.Window):

  def __init__(self):
    Gtk.Window.__init__(self, title="Generate Rpi Image")

    self.vbox = Gtk.VBox(spacing=6)
    self.add(self.vbox)

    self.taskTree = Gtk.TreeView(taskList)
    self.vbox.pack_start(self.taskTree, True, True, 0)

    self.buttonBox = Gtk.HBox()
    self.vbox.pack_start(self.buttonBox, True, True, 0)

    self.selectButton = Gtk.Button(label="Select")
    self.selectButton.connect("clicked", self.on_select_clicked)
    self.generateButton = Gtk.Button(label="Generate")
    self.generateButton.connect("clicked", self.on_generate_clicked)

    self.buttonBox.pack_start(self.selectButton, True, True, 0)
    self.buttonBox.pack_start(self.generateButton, True, True, 0)

  def on_generate_clicked(self, widget):
    print("Generate")

  def on_select_clicked(self, widget):
    print("Select")

win = MyWindow()
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()
