// Websever setup -----------------------------------------------------
#include <ESP8266WiFi.h>


const char* ssid = "Wi-Fi Network Name";   
const char* password = "Wi-Fi password";

unsigned long ulReqcount;
unsigned long ulReconncount;

WiFiServer server(80);  // Create an instance of the server on Port 80


/// complete web server /////////////////////////////////////////////////////////
void WiFiStart()
{
  ulReconncount++;
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname("SkyNet-ESP-10-L");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void webserver() {   /// complete web server /////////////////////////////////////////////////////////

  
  // check if WLAN is connected
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFiStart();
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  unsigned long ultimeout = millis()+250;
  while(!client.available() && (millis()<ultimeout) )
  {
    delay(1);
  }
  if(millis()>ultimeout) 
  { 
    Serial.println("client connection time-out!");
    return; 
  }
  
  // Read the first line of the request
  String sRequest = client.readStringUntil('\r');
  //Serial.println(sRequest);
  client.flush();
  
  // stop client, if request is empty
  if(sRequest=="")
  {
    Serial.println("empty request! - stopping client");
    client.stop();
    return;
  }
  
  // get path; end of path is either space or ?
  // Syntax is e.g. GET /?pin=MOTOR1STOP HTTP/1.1
  String sPath="",sParam="", sCmd="";
  String sGetstart="GET ";
  int iStart,iEndSpace,iEndQuest;
  iStart = sRequest.indexOf(sGetstart);
  if (iStart>=0)
  {
    iStart+=+sGetstart.length();
    iEndSpace = sRequest.indexOf(" ",iStart);
    iEndQuest = sRequest.indexOf("?",iStart);
    
    // are there parameters?
    if(iEndSpace>0)
    {
      if(iEndQuest>0)
      {
        // there are parameters
        sPath  = sRequest.substring(iStart,iEndQuest);
        sParam = sRequest.substring(iEndQuest,iEndSpace);
      }
      else
      {
        // NO parameters
        sPath  = sRequest.substring(iStart,iEndSpace);
      }
    }
  }
  
  ///////////////////////////////////////////////////////////////////////////////
  // output parameters to serial, you may connect e.g. an Arduino and react on it
  ///////////////////////////////////////////////////////////////////////////////
  if(sParam.length()>0)
  {
    int iEqu=sParam.indexOf("=");
    if(iEqu>=0)
    {
      sCmd = sParam.substring(iEqu+1,sParam.length());
      Serial.println(sCmd);
    }
  }
   
  ///////////////////////////
  // format the html response
  ///////////////////////////
  String sResponse,sHeader;
  
  ////////////////////////////
  // 404 for non-matching path
  ////////////////////////////
  if(sPath!="/")
  {
    sResponse="<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
    
    sHeader  = "HTTP/1.1 404 Not found\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }
  ///////////////////////
  // format the html page
  ///////////////////////
  else
  {
    ulReqcount++;
    sResponse  = "<html><head><title>ESP8266 control for WS2812B FASTLed library</title></head><body>";
    sResponse += "<font color=\"#FFFFF0\"><body bgcolor=\"#000000\">";  // first is background, second is font color
    sResponse += "<FONT SIZE=-1>"; 
    sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
    sResponse += "<h1>ESP8266 control for for WS2812B FASTLed library</h1>";

/*  this creates a list with ON / OFF buttons 
    // </a>&nbsp is a non-breaking space; moves next character over
    sResponse += "<p>Rainbow &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"?pin=FUNCTION1ON\"><button>--ON--</button></a>&nbsp;<a href=\"?pin=FUNCTION1OFF\"><button>--OFF--</button></a><br>";
    sResponse += "<p>Rainbow Glitter<a href=\"?pin=FUNCTION2ON\"><button>--ON--</button></a>&nbsp;<a href=\"?pin=FUNCTION2OFF\"><button>--OFF--</button></a><br>";
    sResponse += "<p>Confetti &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"?pin=FUNCTION3ON\"><button>--ON--</button></a>&nbsp;<a href=\"?pin=FUNCTION3OFF\"><button>--OFF--</button></a><br>";
    sResponse += "<p>Sinelon &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"?pin=FUNCTION4ON\"><button>--ON--</button></a>&nbsp;<a href=\"?pin=FUNCTION4OFF\"><button>--OFF--</button></a><br>";
    sResponse += "<p>Juggle&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"?pin=FUNCTION5ON\"><button>--ON--</button></a>&nbsp;<a href=\"?pin=FUNCTION5OFF\"><button>--OFF--</button></a></p>";
    sResponse += "<p>BPM&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"?pin=FUNCTION6ON\"><button>--ON--</button></a>&nbsp;<a href=\"?pin=FUNCTION6OFF\"><button>--OFF--</button></a></p>";
    sResponse += "<p>Function 7&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href=\"?pin=FUNCTION7ON\"><button>--ON--</button></a>&nbsp;<a href=\"?pin=FUNCTION7OFF\"><button>--OFF--</button></a></p><br>";
*/
//  This is a nice drop down menue
    sResponse += "<FONT SIZE=+1>";
    sResponse += "<form>";
    sResponse += "<p>Select Animation</p>";
    sResponse += "<select name=\"sCmd\" size=\"35\" >";
    sResponse += "<option value=\"OFF\"selected>All OFF</option>";
    sResponse += "<option value=\"Rainbow\">Rainbow</option>";
    sResponse += "<option value=\"Rainbow_Glitter\">Rainbow Glitter</option>";
    sResponse += "<option value=\"Confetti\">Confetti</option>";
    sResponse += "<option value=\"Sinelon\">Sinelon</option>";
    sResponse += "<option value=\"Juggle\">Juggle</option>";
    sResponse += "<option value=\"BPM\">BPM</option><br>";
    sResponse += "<option value=\"TwinkleRandom\">TwinkleRandom</option><br>";
    sResponse += "<option value=\"SoftTwinkles\">SoftTwinkles</option><br>";
    sResponse += "<option value=\"ColorTemperatureStrip\">ColorTemperatureStrip</option><br>";
    sResponse += "<option value=\"Cylon\">Cylon</option><br>";
    sResponse += "<option value=\"RainbowStripPaletteLB\">RainbowStripPaletteLB</option><br>";
    sResponse += "<option value=\"PurpleandGreenPaletteLB\">PurpleandGreenPaletteLB</option><br>";
    sResponse += "<option value=\"Fire\">Fire</option><br>";
    sResponse += "<option value=\"PartyColorsPaletteLB\">PartyColorsPaletteLB</option><br>";
    sResponse += "<option value=\"BlackAndWhiteStripedLB\">BlackAndWhiteStripedLB</option><br>";
    sResponse += "<option value=\"RedWhiteBluePaletteLB\">RedWhiteBluePaletteLB</option><br>";
    sResponse += "<option value=\"CloudColorsPaletteLB\">CloudColorsPaletteLB</option><br>";
    sResponse += "<option value=\"FirstLight\">FirstLight</option><br>";
    sResponse += "<option value=\"ColourFade\">ColourFade</option><br>";
    sResponse += "<option value=\"RingPair\">RingPair</option><br>";
    sResponse += "<option value=\"DoubleChaser\">DoubleChaser</option><br>";
    sResponse += "<option value=\"TripleBounce\">TripleBounce</option><br>";
    sResponse += "<option value=\"WaveInt\">WaveInt</option><br>";
    sResponse += "<option value=\"Wave\">Wave</option><br>";
    sResponse += "<option value=\"Spark\">Spark</option><br>";
    sResponse += "<option value=\"SparkWH\">SparkWH</option><br>";
    sResponse += "<option value=\"RainbowSpark\">RainbowSpark</option><br>";
    sResponse += "<option value=\"AAnimation\">AAnimation</option><br>";
    sResponse += "<option value=\"BeatWave\">BeatWave</option><br>";
    sResponse += "<option value=\"Blur\">Blur</option><br>";
    sResponse += "<option value=\"Dot\">Dot</option><br>";
    sResponse += "<option value=\"Move\">Move</option><br>";
    sResponse += "<option value=\"Noise\">Noise</option><br>";
    sResponse += "</select>";
    sResponse += "<br><br>";
    sResponse += "<input type= submit>";
    sResponse += "</form>";
    sResponse += "<FONT SIZE=-1>";

    //////////////////////
    // react on parameters
    //////////////////////
    if (sCmd.length()>0)
    {
      // write received command to html page
      sResponse += "Command: " + sCmd + "<BR>";
      
      // switch GPIO
      if(sCmd.indexOf("Rainbow")>=0)
      {
        Serial.println("Rainbow");
        ledMode = 2;
      }
      else if(sCmd.indexOf("OFF")>=0)
      {
        Serial.println("OFF");
        ledMode = 1;
      }

      if(sCmd.indexOf("Rainbow_Glitter")>=0)
      {
         Serial.println("Rainbow_Glitter");
        ledMode = 3;
      }
      else if(sCmd.indexOf("OFF")>=0)
      {
        Serial.println("OFF");
        ledMode = 1;
      }

      if(sCmd.indexOf("Confetti")>=0)
      {
         Serial.println("Confetti");
        ledMode = 4;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
        Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("Sinelon")>=0)
      {
        Serial.println("Sinelon");
        ledMode = 5;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
        Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("Juggle")>=0)
      {
         Serial.println("Juggle");
        ledMode = 6;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
        Serial.println("OFF");
        ledMode = 1;

      }

      if(sCmd.indexOf("BPM")>=0)
      {
         Serial.println("BPM");
        ledMode = 7;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
        Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("TwinkleRandom")>=0)
      {
        Serial.println("TwinkleRandom");
        ledMode = 8;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("SoftTwinkles")>=0)
      {
        Serial.println("SoftTwinkles");
        ledMode = 9;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("ColorTemperatureStrip")>=0)
      {
        Serial.println("ColorTemperatureStrip");
        ledMode = 10;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("Cylon")>=0)
      {
        Serial.println("Cylon");
        ledMode = 11;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("RainbowStripPaletteLB")>=0)
      {
        Serial.println("RainbowStripPaletteLB");
        ledMode = 12;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("PurpleandGreenPaletteLB")>=0)
      {
        Serial.println("PurpleandGreenPaletteLB");
        ledMode = 13;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("Fire")>=0)
      {
        Serial.println("Fire");
        ledMode = 14;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("PartyColorsPaletteLB")>=0)
      {
        Serial.println("PartyColorsPaletteLB");
        ledMode = 15;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("BlackAndWhiteStripedLB")>=0)
      {
        Serial.println("BlackAndWhiteStripedLB");
        ledMode = 16;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("RedWhiteBluePaletteLB")>=0)
      {
        Serial.println("RedWhiteBluePaletteLB");
        ledMode = 17;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("CloudColorsPaletteLB")>=0)
      {
        Serial.println("CloudColorsPaletteLB");
        ledMode = 18;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("FirstLight")>=0)
      {
        Serial.println("FirstLight");
        ledMode = 19;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("ColourFade")>=0)
      {
        Serial.println("ColourFade");
        ledMode = 20;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }

      if(sCmd.indexOf("RingPair")>=0)
      {
        Serial.println("RingPair");
        ledMode = 21;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("DoubleChaser")>=0)
      {
        Serial.println("DoubleChaser");
        ledMode = 22;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("TripleBounce")>=0)
      {
        Serial.println("TripleBounce");
        ledMode = 23;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("WaveInt")>=0)
      {
        Serial.println("WaveInt");
        ledMode = 24;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }

if(sCmd.indexOf("RingPair")>=0)
      {
        Serial.println("RingPair");
        ledMode = 21;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("DoubleChaser")>=0)
      {
        Serial.println("DoubleChaser");
        ledMode = 22;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("TripleBounce")>=0)
      {
        Serial.println("TripleBounce");
        ledMode = 23;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("WaveInt")>=0)
      {
        Serial.println("WaveInt");
        ledMode = 24;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }if(sCmd.indexOf("RingPair")>=0)
      {
        Serial.println("RingPair");
        ledMode = 21;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("DoubleChaser")>=0)
      {
        Serial.println("DoubleChaser");
        ledMode = 22;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("TripleBounce")>=0)
      {
        Serial.println("TripleBounce");
        ledMode = 23;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("WaveInt")>=0)
      {
        Serial.println("WaveInt");
        ledMode = 24;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }if(sCmd.indexOf("RingPair")>=0)
      {
        Serial.println("RingPair");
        ledMode = 21;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("DoubleChaser")>=0)
      {
        Serial.println("DoubleChaser");
        ledMode = 22;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("TripleBounce")>=0)
      {
        Serial.println("TripleBounce");
        ledMode = 23;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("WaveInt")>=0)
      {
        Serial.println("WaveInt");
        ledMode = 24;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }if(sCmd.indexOf("RingPair")>=0)
      {
        Serial.println("RingPair");
        ledMode = 21;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("DoubleChaser")>=0)
      {
        Serial.println("DoubleChaser");
        ledMode = 22;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("TripleBounce")>=0)
      {
        Serial.println("TripleBounce");
        ledMode = 23;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("WaveInt")>=0)
      {
        Serial.println("WaveInt");
        ledMode = 24;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }if(sCmd.indexOf("Wave")>=0)
      {
        Serial.println("Wave");
        ledMode = 25;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("Spark")>=0)
      {
        Serial.println("Spark");
        ledMode = 26;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("SparkWH")>=0)
      {
        Serial.println("SparkWH");
        ledMode = 27;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;
      
      }
      if(sCmd.indexOf("RainbowSpark")>=0)
      {
        Serial.println("RainbowSpark");
        ledMode = 28;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      if(sCmd.indexOf("AAnimation")>=0)
      {
        Serial.println("AAnimation");
        ledMode = 29;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }

      if(sCmd.indexOf("BeatWave")>=0)
      {
        Serial.println("BeatWave");
        ledMode = 30;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }
      
      if(sCmd.indexOf("Blur")>=0)
      {
        Serial.println("Blur");
        ledMode = 31;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }

      if(sCmd.indexOf("Dot")>=0)
      {
        Serial.println("Dot");
        ledMode = 32;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }

      if(sCmd.indexOf("Move")>=0)
      {
        Serial.println("Move");
        ledMode = 33;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }

      if(sCmd.indexOf("Noise")>=0)
      {
        Serial.println("Noise");
        ledMode = 34;

      }
      else if(sCmd.indexOf("OFF")>=0)
      {
         Serial.println("OFF");
        ledMode = 1;

      }


 
    } 
    
    
    //    end sCmd.length()>0
    
    sResponse += "<FONT SIZE=-2>";
    sResponse += "<BR>clicks on page ="; 
    sResponse += ulReqcount;
    sResponse += " - connections to page ="; 
    sResponse += ulReconncount;
    sResponse += "<BR>";
    sResponse += "Based on Gyro Gearloose 02/2016<BR><BR>";
    sResponse += "<font color=\"#FF0000\">";
    sResponse += "Based on DemoReel 100 by Mark Kriegsman<BR>";
    sResponse += "Webserver by Stefan Thesen<BR>";
    sResponse += "</body></html>";
    sHeader  = "HTTP/1.1 200 OK\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }
  
  // Send the response to the client
  client.print(sHeader);
  client.print(sResponse);

  
  // and stop the client
  client.stop();
  Serial.println("Client disonnected");  
  }
/// END of complete web server //////////////////////////////////////////////////////////////////
