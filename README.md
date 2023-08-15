# AQAD41 (and more Volvo Penta engines) - RPM data To SignalK
First off, Kudos to @Boatingwiththebaileys that crated the shell and idea for this project!

This projects get your Volvo Penta RPM data into SignalK and still keep your Original tachometers fully working.
It also calculates (very roughly) the fuel comsumption at that very moment, in m3/s, SignalK standard.

**Note 1** I've tested this on my own engines, works nicely. However, I won't guarantee anything, all tests is made at own risk!!

**Note 2** Number of items, electric schedule and code is based on a Dual Engine Setup. Works just as fine on Single Engine Setup, just skip half of parts and comment out code (not necessary, no signal = 0 rpm on "missing" engine.)

Tested on Volvo Penta AQAD41B with Tachometer 860420 (with hourmeter) and Tachometer (ArtNo unknown) without hourmeter. 
Should be compatible with AQAD31, AQAD41, TAMD31, TAMD41, KAD42, KAMD42 and more models. And with custom cabling, more or less all Volvo Penta engines (and also other brands) that takes Alternator W signal as Tacho signal.

## Hardware Principle

The projects is based on SensESP, and therefor an ESP32 microcontroller is needed.
The principle is as follows:
- W signal and GND is picked from original tachometer cabling, from a custom built adapter
- A simple circuit collects the signal and transfers it through an optocoupler and into a GPIO on the ESP32
- The ESP32 counts all impulses and after some math rpm is changed to Hz and sent to SignalK

## Parts List

Parts bought from www.electrokit.com (Art.no in paranteses):
- 2pcs OptoCoupler - SFH617A-4 DIP-4 70V 50m (41017353)
- 2pcs Zener Diode - BZX55C3V3 DO-35 3.3V 500mW (40310015)
- 2pcs Resistor - 4,7Kohm (41015986)
- 2pcs	Resistor - 220ohm (41015982)
- 2pcs	Resistor - 10Kohm (41011682)
- 2pcs	Connector Housing Fastin-Faston Female 4-pole (41017438)
- 2pcs	Connector Housing Fastin-Faston Male 4-pole (41011051)
- 4pcs	Connector Fastin-Faston (41003584)
- 4pcs	Connector Fastin-Faston (41003585)

Mandatory other items:
- ESP32 D1 Mini or other ESP32 Dev board

Optional items:
- Some cables and connectors for the build
- some DIY circuit board 
- 12V->5V step down circuit

## Electric Schedule

Based on the part list above, follow this schedule to build the electric circuit.

![ElectricSchedule](https://github.com/LundSoftwares/AQAD41-TachoToSignalK/assets/23386303/b7aa5950-1433-4f2b-ab88-8660a66f81c6)

With the 4 pole Fastin-Faston connectors, build through-adapters and take W and GND signal to your circuit. Image below shows the Pinout for the Tachometers.

![TachometerBack](https://github.com/LundSoftwares/AQAD41-TachoToSignalK/assets/23386303/5e56d8b8-a16e-4fc1-baa6-6919d1384510)

If your Tachometer looks different, find the pinout and build own cable adapter, or just piggyback the original cabling.

## Code and config

Code is based on SensESP and built with PlatformIO (More information on SensESP, WiFi setup, SignalK Auth and so on [Here](https://signalk.org/SensESP/pages/getting_started/))

**Google how to use and upload code with PlatformIO, this won't be covered in this instruction.**

Two parts may be configured based on Engine type:
- FuelInterpreter is a Lookup Table for Fuel consumption that converts rpm to m3/s. Info in the table is for TAMD41/AQAD41 and may need to be adjusted for other types.
- multiplier_port and multiplier_starboard needs to be adjusted based on flywheel to Alternator W ratio.
  - Example: 1 turn on an AQAD41 makes 2.45 turns on the Alternator, and the Alternator has 6 poles which makes a total of 2.45 x 6 = 14.7 Hz per turn. SignalK needs info in Hz so we need to divide the incoming value with 14.7, or as in our case multiply with (1/14.7) = 0,06803
  - If Ratio is unknown, the original Tachometer might have a max Impulses/min written on it, divide that with max rpm on the meter and you'll get the ratio. Tachometer 860420 is marked with 73500Imp/min and has a scale to 5000rpm. 73500 divided with 5000 equals 14,7, Tada!
- If above multiplier needs to be recalculated, the FuelMultipliers needs to be recalculated as well, they're both based on AQAD41 values right now.
  - AQAD41 example:  60 divided with FlyWheel To W Ratio (60 / 14,7 = 4,08) 
    

