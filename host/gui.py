"""
GUI for the host, utilizing PyGObject
"""

from gi.repository import Gtk, GLib

import device

MAIN_MENU = """
<ui>
  <menubar name='MenuBar'>
    <menu action='FileMenu'>
      <menu action='FileNew'>
        <menuitem action='FileNewStandard' />
        <menuitem action='FileNewFoo' />
        <menuitem action='FileNewGoo' />
      </menu>
      <separator />
      <menuitem action='FileQuit' />
    </menu>
  </menubar>
</ui>
"""

class ConnectDialog(Gtk.Dialog):
    """
    Dialog asking for a connection to the device
    """
    def __init__(self, parent):
        Gtk.Dialog.__init__(self, "Connecting to device", parent, 0,
            (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL))

        self.set_default_size(150, 100)

        content = self.get_content_area()
        label = Gtk.Label.new("Waiting for device connection")
        content.add(label)

        self.done = False
        self.device = None
        GLib.timeout_add_seconds(1, self.poll_device)

        self.show_all()

    def poll_device(self):
        """
        Polls to see if a device is connected. If one is, it issues an OK
        response from this dialog
        """
        if self.done:
            return False
        try:
            self.device = device.Device.open()
            if self.device is not None:
                self.response(Gtk.ResponseType.OK)
        except IOError as ex:
            print("Failed to connect to device", ex)
        return True

    def do_response(self, response):
        """
        When a response happens, we are done
        """
        self.done = True

class MainWindow(Gtk.Window):
    """
    Main application window
    """
    def __init__(self):
        Gtk.Window.__init__(self, title="Teensy 3.1 Oscilloscope")

        action_group = Gtk.ActionGroup("my_actions")

        self.add_file_menu_actions(action_group)

        uimanager = self.create_ui_manager()
        uimanager.insert_action_group(action_group)

        menubar = uimanager.get_widget("/MenuBar")

        box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        box.pack_start(menubar, False, False, 0)

        self.__device = None

        self.add(box)
        self.show_all()

    def add_file_menu_actions(self, action_group):
        action_filemenu = Gtk.Action("FileMenu", "File", None, None)
        action_group.add_action(action_filemenu)

        action_filenewmenu = Gtk.Action("FileNew", None, None, Gtk.STOCK_NEW)
        action_group.add_action(action_filenewmenu)

        action_new = Gtk.Action("FileNewStandard", "_New",
            "Create a new file", Gtk.STOCK_NEW)
        action_new.connect("activate", self.on_menu_file_new_generic)
        action_group.add_action_with_accel(action_new, None)

        action_group.add_actions([
            ("FileNewFoo", None, "New Foo", None, "Create new foo",
             self.on_menu_file_new_generic),
            ("FileNewGoo", None, "_New Goo", None, "Create new goo",
             self.on_menu_file_new_generic),
        ])

        action_filequit = Gtk.Action("FileQuit", None, None, Gtk.STOCK_QUIT)
        action_filequit.connect("activate", self.on_menu_file_quit)
        action_group.add_action(action_filequit)

    def create_ui_manager(self):
        uimanager = Gtk.UIManager()

        # Throws exception if something went wrong
        uimanager.add_ui_from_string(MAIN_MENU)

        # Add the accelerator group to the toplevel window
        accelgroup = uimanager.get_accel_group()
        self.add_accel_group(accelgroup)
        return uimanager

    def on_menu_file_new_generic(self, widget):
        print("A File|New menu item was selected.")

    def on_menu_file_quit(self, widget):
        Gtk.main_quit()

    def get_device(self):
        """
        Returns the currently connected device or quits if one is not available
        """
        while self.__device is None:
            d = ConnectDialog(self)
            resp = d.run()
            if resp is Gtk.ResponseType.OK:
                self.device = d.device
            elif resp is Gtk.ResponseType.CANCEL:
                Gtk.main_quit() # well that's an extreme response...
            d.destroy()
        return self.__device

def run():
    """
    Main GUI method which exits when the GUI has finished running
    """
    win = MainWindow()
    Gtk.main()
