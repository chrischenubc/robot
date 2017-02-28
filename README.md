# G5_A_P1

##Android App

The code was based off of and was modified from the source code at https://github.com/Mayoogh/Arduino-Bluetooth-Basic

LED buttons were removed from the original source code. 3 buttons for the modes, and 4 buttons for movement are added.

Serial communication works at 9600 baud rate. 

###Problems

*During serial communication, if buttons are pressed too fast, when Serial.readString() is called, the outputs from the buttons are all put together side by side when they should be separated with a delay.

*Higher baud rate might solve the problem above. Higher baud rate should be encouraged I think because that means faster connection speed I think. However, testing with higher baud rates made the text communicating via Serial Monitor completely different and unintelligible

All problems above will be demonstrated on Wednesday’s lab 29 Feb 17.

