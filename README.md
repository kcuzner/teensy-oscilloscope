# Bare Metal Teensy 3.1 "Oscilloscope"
### Kevin Cuzner

## Objective

I need something to measure analog signals with at low frequencies (30-50Khz
tops). The Teensy 3.1 happens to have an analog to digital converter capable
of 500-800ksps at varying resolutions which should be sufficient for my needs.

## Credit

The basic build structure (`crt0`, `sysinit`, etc) is expanded from
[Karl Lunt's](http://www.seanet.com/~karllunt/bareteensy31.html) bare-metal
Teensy solution with some modifications. I used the V-USB driver and the
Teensyduino driver as partial templates for certain aspects of the USB driver,
but the code is all my own.


