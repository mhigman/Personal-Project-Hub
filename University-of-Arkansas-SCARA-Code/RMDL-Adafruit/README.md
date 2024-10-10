# Arduino RMDL Adafruit driver

Fork of a fork of Matthieu Vigne's RMDX-Arduino library that is compatable with RMDL's and the Adafruit M4 CAN express chipset.

A library to drive [My Actuator's RMD-L brushless motors](https://www.myactuator.com/).
These are geared brushless motors, with integrated encoder & driver, performing field-oriented control.

These motors use a CAN bus for communication: the Adafruit M4 CAN Express does have a CAN bus adapter on the chip, making use of the SAME51 chipset. This library
makes use of SAME51 chipset.

This library was made mostly with simple, "Hello World", applications in mind. More advanced applications would require additional development to unlock all features.
