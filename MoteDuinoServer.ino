/*
 * MoteDuinoServer - Recevier script for your TV commands.
 * It then translates the commands to IR values specific
 * to the Sanyo TV (Protocol: NEC) so you can control 
 * it from your Arduino. 
 *
 * This is intended to be use with the companion Android
 * app MoteDuino. 
 * 
 * Author: Frank E. Hernandez
 * Date: 04/26/2015
 *
 * Libraries - IRremote library 
 * ( https://github.com/shirriff/Arduino-IRremote )
 * to send the codes over to an IR LED connected on 
 * pin 9. (I modified it to include the sendSanyo function
 * which takes an address and the command, but you can
 * just use the send NEC function in the library and
 * pass those two values combined into one in the form
 * 0xAddressCommand [32 bits]) 
 * I've included the modified library with this script.
 *
 * Finding the Codes: You can find many of the codes
 * to existing sets on the LIRC database
 * ( http://lirc-remotes.sourceforge.net/remotes-table.html )
 * pre_data => Address
 * key code => Command (Ex. 0x00000000000048B7 => 0x48B7)
 * 
 * Note: In the cases where LIRC doesn't have the pre_data
 * the address will be part of the key code.
 * Ex: 0x00000000E31C7F80 => 0xE31C7F80 => 0xE31C 0x7F80
 * Address: 0xE31C
 * Command: 0x7F80
 *
 * NEC Protocol: The NEC protocol sends the address and
 * command twice. The first is the address followed by 
 * bitwise complement of the address. It does the same
 * for the command. This is done for error checking.
 * The values that you are getting from
 * from LIRC already include this. 
 * Ex: 
 * Address: 0xE31C (LIRC), the actual address is
 * E3 and its bitwise complement 1C.
 *
 * E3 => 1110 0011 
 * 1C => 0001 1100
 * So the actual address part looks like:
 * 1110 0001 0001 11000
 *
 * The same is done for the commands. In the version
 * of the NEC protocol used by Sanyo it uses 8 bits 
 * for address an 8 bits for the command. 
 * For more information on the NEC protocol see:
 * ( http://www.sbprojects.com/knowledge/ir/nec.php ) 
 *
 * Building: The new version of the library uses 
 * preporcessor directive to ensure that when your
 * script compiles it doesnt also include a lot of
 * functions you are not using and only includes the
 * ones that you need, keeing the final output small.
 * It is state in the site that all you need to do
 * is include the #define for the specific functions
 * at the top of your script as such: 
 * #define SEND_SANYO
 *
 * In my case it seems the library binaries got compiled
 * before my script so adding the define in the script 
 * only allowed me get my script to verify since the 
 * functions in the header that I needed were properly 
 * inlcuded but when the script would fail because the
 * implementations where not in the binary file since
 * these were compile before the flag was set.
 *
 * Solution: Modified IRremote.h and included my defines
 * there, this guaranteed that everything was included 
 * properly and compiled. If you find a better solution
 * let me know. 
 * 
 * Conflit: RobotRemote library has a file that shares
 * the same name as in the Arduino-IRemote library. Make
 * sure to remove it.
 *
 * Setup Notes:
 * IR Pin: Wire to pin 9 if using ATmega1280 or ATmega2560,
 * otherwise wire to Pin 3.
 * BT Module: Wire module Module TX to Arduino RX(Pin0). Wire
 * Module RX to Arduino TX (Pin1). Wire Module Power to Arduino
 * 5V and wire Module Ground to Arduino Ground. 
 */
#include <IRremote.h>
#include <SoftwareSerial.h>

// Sanyo uses the NEC protocol which
// Consist of an Address and Command.
// Address is 16 bits followed by the inverted bits.
// Command is 16 bits followed by the inverted bits.
// This is used for validation. 
// Ex: 1CE3 48B7
// Addr: 1C
// ~Addr: E3
// Command: 48 (Power)
// ~Command: B7
// Ending with a bit mark. 

// This is the address of the TV
// So far all Sanyo TVs seem to have the 
// same address.
#define ADDRESS 0x1CE3

#define KEY_POWER                0x48B7
#define KEY_SLEEP                0xB04F 
#define KEY_INPUT                0xC837
#define KEY_EXIT                 0xCA35
#define KEY_ENTER                0x2AD5
#define KEY_1                    0x807F 
#define KEY_2                    0x40BF 
#define KEY_3                    0xC03F 
#define KEY_4                    0x20DF 
#define KEY_5                    0xA05F 
#define KEY_6                    0x609F 
#define KEY_7                    0xE01F 
#define KEY_8                    0x10EF 
#define KEY_9                    0x906F 
#define KEY_0                    0x00FF 
#define KEY_CHANNEL_UP           0x50AF 
#define KEY_CHANNEL_DOWN         0xD02F 
#define KEY_VOLUME_UP            0x708F 
#define KEY_VOLUME_DOWN          0xF00F 
#define KEY_VIDEO                0xC837 
#define KEY_MUTE                 0x18E7 
#define KEY_SETUP                0xE817 
#define KEY_PIX_SHAPE            0xEA15
#define KEY_CAPTION              0x8877
#define KEY_CHANNEL_RECALL       0x9867
#define KEY_MENU                 0xE817 
#define KEY_MENU_UP              0x728D
#define KEY_MENU_DOWN            0xF20D
#define KEY_MENU_LEFT            0xF807
#define KEY_MENU_RIGHT           0x7887
#define KEY_AUDIO                0x58A7

const String CMD_POWER = "P";
const String CMD_VOL_UP = "V+";
const String CMD_VOL_DOWN = "V-";
const String CMD_VOL_MUTE = "T";
const String CMD_CHANNEL_UP ="C+";
const String CMD_CHANNEL_DOWN = "C-";
const String CMD_CHANNEL_RECALL = "R";
const String CMD_AUDIO = "A";
const String CMD_INPUT = "I";
const String CMD_SLEEP = "S";
const String CMD_EXIT = "X";
const String CMD_ENTER = "E";
const String CMD_CAPTION = "CA";
const String CMD_PIX_SHAPE = "PX";
const String CMD_0 = "0";
const String CMD_1 = "1";
const String CMD_2 = "2";
const String CMD_3 = "3";
const String CMD_4 = "4";
const String CMD_5 = "5";
const String CMD_6 = "6";
const String CMD_7 = "7";
const String CMD_8 = "8";
const String CMD_9 = "9";
const String CMD_MENU = "M";
const String CMD_MENU_UP = "MU";
const String CMD_MENU_DOWN = "MD";
const String CMD_MENU_LEFT = "ML";
const String CMD_MENU_RIGHT = "MR";



IRsend irsend;
String cmdIn = "";
boolean cmdComplete = false;

//SoftwareSerial mySerial(10,11);
void setup() 
{
  Serial.begin(9600);
  while(!Serial)
  {
   ; // Wait for serial port to be open. 
  }
  cmdIn.reserve(200);
  Serial.println("MoteDuino Ready\n");
  
  //mySerial.begin(9600);
  //mySerial.println("Serial Ready");
  
}


void loop() 
{
  
  if(!cmdComplete)
  {
    return;
  }  
  
  if(CMD_POWER == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_POWER);
  }
  else if(CMD_INPUT == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_INPUT); 
  }
  else if(CMD_SLEEP == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_SLEEP);
  }
  else if(CMD_ENTER == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_ENTER);
  }
  else if(CMD_EXIT == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_EXIT); 
  }
  else if(CMD_CAPTION == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_CAPTION); 
  }
  else if(CMD_PIX_SHAPE == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_PIX_SHAPE); 
  }
  else if(CMD_AUDIO == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_AUDIO);
  }
  
  handleVolumeCommand(cmdIn);
  handleChannelCommand(cmdIn);
  handleNumberPadCommand(cmdIn);
  handleMenuCommand(cmdIn);
  
  delay(60);  
  cmdIn = "";
  cmdComplete = false;
}

boolean handleVolumeCommand(String cmdIn)
{
  if(CMD_VOL_UP == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_VOLUME_UP);
    return true;
  }
  else if (CMD_VOL_DOWN == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_VOLUME_DOWN);
    return true;
  }
  else if(CMD_VOL_MUTE == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_MUTE);
    return true; 
  }
  
  return false;
}

boolean handleChannelCommand(String cmdIn)
{
  if(CMD_CHANNEL_UP == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_CHANNEL_UP);
    return true;
  }
  else if(CMD_CHANNEL_DOWN == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_CHANNEL_DOWN);
    return true;
  }
  else if(CMD_CHANNEL_RECALL == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_CHANNEL_RECALL);
    return true; 
  }

  return false;  
}

boolean handleNumberPadCommand(String cmdIn)
{
  if(CMD_0 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_0);
    return true;
  }
  else if(CMD_1 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_1);
    return true;
  }
  else if(CMD_2 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_2);
    return true;
  }
  else if(CMD_3 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_3);
    return true;
  }
  else if(CMD_4 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_4);
    return true;
  }
  else if(CMD_5 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_5);
    return true;
  }
  else if(CMD_6 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_6);
    return true;
  }
  else if(CMD_7 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_7);
    return true;
  }
  else if(CMD_8 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_8);
    return true;
  }
  else if(CMD_9 == cmdIn)
  {
    irsend.sendSanyo(ADDRESS, KEY_9);
    return true;
  }
  
  return false;
}

boolean handleMenuCommand(String cmdIn)
{
 if(CMD_MENU == cmdIn)
 {
   irsend.sendSanyo(ADDRESS, KEY_MENU);
   return true;
 }
 else if(CMD_MENU_UP == cmdIn)
 {
   irsend.sendSanyo(ADDRESS, KEY_MENU_UP);
   return true;
 } 
 else if(CMD_MENU_DOWN == cmdIn)
 {
   irsend.sendSanyo(ADDRESS, KEY_MENU_DOWN);
   return true;
 } 
 else if(CMD_MENU_LEFT == cmdIn)
 {
   irsend.sendSanyo(ADDRESS, KEY_MENU_LEFT);
   return true;
 }
 else if(CMD_MENU_RIGHT == cmdIn)
 {
   irsend.sendSanyo(ADDRESS, KEY_MENU_RIGHT);
   return true;
 } 
 return false;
}
void retrieveCommand()
{
  while(Serial.available())
  {
    char inChar = (char)Serial.read();
    if(inChar == '\n' || inChar =='\r')
    {
      cmdComplete = true;
      Serial.println(cmdIn);
      break; 
    }
    else
    {
      cmdIn += inChar;
    }
  }   
}
/*
void retrieveCommand2()
{
  while(mySerial.available())
  {
    char inChar = (char)mySerial.read();
    Serial.println(cmdIn);
    if(inChar == '\n' || inChar =='\r')
    {
      cmdComplete = true;
      mySerial.println(cmdIn);
      //Serial.println(cmdIn);
      break; 
    }
    else
    {
      cmdIn += inChar;
    }
  }   
}
*/

void serialEvent() 
{
    //retrieveCommand2();
    retrieveCommand();
}
