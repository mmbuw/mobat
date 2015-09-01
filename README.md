# MoBat
is a combined hard- and software system designed to locate and track multiple unmodified mobile devices on any regular table using passive acoustic sensing. 

Barely audible sound pulses are emitted from mobile devices, picked up by four microphones located in the corners of the surface and processed in a low-latency pipeline to extract position data.
It has an average positional accuracy and precision of about 3 cm on a table of 1 m x 2 m size.

The prototype set-up consists of the following:

## Software
* **TTT Sine Emitter** Android app  which lets a smartphone emit a modified sine signal
* **Emitter Locator** Linux application using the **ALSA** sound library which locates the emitted signals

## Hardware
* 4 **t.bone EM700** microphones
*  **Behringer U-PHORIA UMC404** sound-card 
* notebook with an **Intel Core i7-2630QM  2Ghz** CPU. 
* **Nexus 4** devices as soud emitters.