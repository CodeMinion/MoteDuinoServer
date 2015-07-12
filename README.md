MoteDuino Server
- Script to be loaded on an Arduino to allow you to send commands from the MoteDuino Android app over Bluetooth and be able to control your TV.

Note: This contains the TV codes for my Sanyo TV since you probably have a different TV you'll need to replace the codes with your own TV codes. The LIRC project is a good place to find the codes for your TV (http://lirc-remotes.sourceforge.net/remotes-table.html).

MoteDuino Android App (https://play.google.com/store/apps/details?id=codeminion.moteduino)

Arduino Setup:
1- IR LED on Pin 9 as per the requirements of the library Arduino-IRremote.
2- Bluetooth module RX connected to Arduino TX (Pin 1)
3- Bluetooth module TX connected to Arduino RX (Pin 0)
4- Bluetooth module PWR to Arduino 5V
5- Bluetooth module GND to Arduino GND
6- Arduino powered through USB but attached to wall outlet and not a PC. I found problems receiving the signal from the BT module while powering from a PC, probably due to the USB being wired to pin 1 and 0 as well.

