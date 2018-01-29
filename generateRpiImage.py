import gi
gi.require_version('Gtk', '3.0')

from gi.repository import Gtk
from gi.repository import Pango

taskColumns = ["Task Name","State"]

taskList = [("Download latest Rapbian","Not selected"),
	        ("Configure hostname","Not selected"),
	        ("Configure Wifi","Not selected"),
	        ("Enable SSH","Not selected"),
	        ("Enable SAMBA","Not selected"),
            ("Update Raspbian","Not selected"),
            ("Configure audio","Not selected"),
            ("Enable X","Not selected"),
            ("Task1","Not selected"),
            ("Task2","Not selected")]

class Task(object):

  def __init__(self, name):
    self.name = name

class HostnameDialog(Gtk.Dialog):

  def __init__(self, parent):
    Gtk.Dialog.__init__(self, "Configure Hostname", parent, 0,
            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
             Gtk.STOCK_OK, Gtk.ResponseType.OK))

    self.set_default_size(150, 100)

    label = Gtk.Label("This is a dialog to display additional information")

    box = self.get_content_area()
    box.add(label)
    self.show_all()
        
class MyWindow(Gtk.Window):

  def __init__(self):
    Gtk.Window.__init__(self, title="Generate Rpi Image")
    self.set_default_size(480,500)
    self.scrolledList = Gtk.ScrolledWindow()
    self.scrolledList.set_border_width(10)
    self.scrolledList.set_policy(Gtk.PolicyType.NEVER, Gtk.PolicyType.AUTOMATIC)

    self.vbox = Gtk.VBox(homogeneous=False, spacing=4)
    self.add(self.vbox)

    self.taskModel = Gtk.ListStore(str, str)
    for i in range(len(taskList)):
      self.taskModel.append(taskList[i])

    self.taskTree = Gtk.TreeView(model=self.taskModel)

    for i, column in enumerate(taskColumns):
      cell = Gtk.CellRendererText()
      if i == 0:
        cell.props.weight_set = True
        cell.props.weight = Pango.Weight.BOLD
      col = Gtk.TreeViewColumn(column, cell, text=i)
      self.taskTree.append_column(col)

    self.scrolledList.add_with_viewport(self.taskTree)
    self.vbox.pack_start(self.scrolledList, True, True, 0)
    
    self.taskTree.get_selection().connect("changed", self.on_select_change)

    self.buttonBox = Gtk.HBox(spacing=6)
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

  def on_select_change(self, selection):
    print("Selected!")
    (model, iter) = selection.get_selected()
    # set the label to a new value depending on the selection
    print("Selected %s %s" % (model[iter][0], model[iter][1]))
    return True

win = MyWindow()
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()

