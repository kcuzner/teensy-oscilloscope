"""
Device connection for the teensy scope host
"""

import usb.core, usb.util

ID_VENDOR = 0x16c0
ID_PRODUCT = 0x05dc
MANUFACTURER = "kevincuzner.com"
PRODUCT = "Teensy3.1 Scope"

def reload(f):
    """
    The decorated method will attempt to reload its device if a ENODEV occurrs
    """
    def wrapped(*args):
        try:
            return f(*args)
        except usb.core.USBError as err:
            if err.errno == errno.ENODEV:
                if args[0].reload_device():
                    return f(*args) # try again, no exception wrapping
            raise
    return wrapped

class Device(object):
    """
    Represents a teensy-scope device
    """
    @staticmethod
    def get_device():
        """
        Finds the first connected ae-2015 device
        """
        devs = usb.core.find(idProduct=ID_PRODUCT, idVendor=ID_VENDOR,\
            find_all=True)
        for dev in devs:
            if dev.manufacturer == MANUFACTURER and dev.product == PRODUCT:
                dev.set_configuration()
                return dev
        return None
    @staticmethod
    def open():
        """
        Finds the first connected ae-2015 device (16c0:05dc), matching manufacturer
        and product, and returns a turret object
        """
        dev = Device.get_device()
        if dev is None:
            return None
        return Device(dev)

    def __init__(self, dev):
        self.__dev = dev
    def reload_device(self):
        """
        Reconnects this device
        """
        print("Reconnecting device...")
        newdev = Turret.get_device()
        if newdev is not None:
            self.__dev = newdev
            return True
        return False
