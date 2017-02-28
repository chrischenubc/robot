# G5_A_P1

##Android App

The code was based off of and was modified from the source code at https://github.com/Mayoogh/Arduino-Bluetooth-Basic

LED buttons were removed from the original source code. 3 buttons for the modes, and 4 buttons for movement are added.

###Problems

During serial communication, if buttons are pressed too fast, when Serial.readString() is called, the outputs from the buttons are all put together side by side when they should be separated with a delay.

This problem will be demonstrated on Wednesday’s class on 29 Feb 17.
