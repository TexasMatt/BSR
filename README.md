# BSR
Arduino code for a DIY stitch regulator

This code is work in progress.
Current functionality:
Reads X and Y values from PS/2 mouse
Calculates distance traveled
Smooths distance readings (dynamic average up to 3 values)
Sets the value of the MCP4131 digital potentiometer.

No software license is provided at this time.

The PS/2 library was written by Written by Chris J. Kiick, January 2008.
- with a minor change to the header file to make it Arduino compatible.