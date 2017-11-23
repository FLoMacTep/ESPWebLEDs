# ESPWebLEDs

ESP controlled WS2812B LED strip with several effects managed by Web page and GET requests

To setup environment you will need:

      ☼ ESP8266 
      ☼ WS2812B
      ☼ Power Supply 5V (for a large number of LEDS)
      ☼ Arduino IDE with next libraries:
        -  FastLED
        -  ESP8266WiFi
	  
To adapt code please edit this lines -

      LEDEffects.h
	
            11  #define DATA_PIN      5   // Input data pin that you will use - review PINS layout for your ESP board
            14  #define NUM_LEDS      64  // Number of LEDs in strip
     
     WebServer.h
     
     	05   const char* ssid = "Wi-Fi Network Name";  //Input your Wi-Fi SSID 
          	06   const char* password = "Wi-Fi password";  //Input your Wi-Fi password
            24  WiFi.hostname("----"); // Your ESP board network name
		  
 Open in browser http://"ESP board IP address"/
 
 GET query is looks like http://192.168.1.134/?sCmd=OFF or http://192.168.1.134/?sCmd=Rainbow
 
 Executing a random effect from Majordomo server:
 
	$ip=$this->getProperty('IP'); // Get ESP IP Address
	$effect=getRandomLine("EffectList"); //Get effect name from text file
	$cmdURL="http://".$ip."/?sCmd=".$effect.""; // Construct Get Request
	geturl($cmdURL); //EXEC Get request
 
 Full list of effects is in file "EffectList"
 
 Please help me to improve arduino code and fix some bugs!
 
 Scheme
![espwebleds_bb](https://user-images.githubusercontent.com/26871772/32985582-a31b1060-cccf-11e7-9ef3-79955ffe4da7.jpg)
