/*
 * Sketch: ESP8266_Part8_04_AutoUpdate_DHT11
 * Intended to be run on an ESP8266
 */
 
String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
 
String html_1 = R"=====(
<!DOCTYPE html>
<html>
 <head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body {font-size:100%;} 
    #main {display: table; margin: auto;  padding: 10px 10px 10px 10px; } 
    #content { border: 5px solid blue; border-radius: 15px; padding: 10px 0px 10px 0px;}
    h2 {text-align:center; margin: 10px 0px 10px 0px;} 
    p { text-align:center; margin: 5px 0px 10px 0px; font-size: 120%;}
    #time_P { margin: 10px 0px 15px 0px;}
  </style>
 
  <script> 
    function updateTime() 
    {  
       var d = new Date();
       var t = "";
       t = d.toLocaleTimeString();
       document.getElementById('P_time').innerHTML = t;
    }
 
    function updateTemp() 
    {  
       ajaxLoad('getTemp'); 
    }
 
    var ajaxRequest = null;
    if (window.XMLHttpRequest)  { ajaxRequest =new XMLHttpRequest(); }
    else                        { ajaxRequest =new ActiveXObject("Microsoft.XMLHTTP"); }
 
    function ajaxLoad(ajaxURL)
    {
      if(!ajaxRequest){ alert('AJAX is not supported.'); return; }
 
      ajaxRequest.open('GET',ajaxURL,true);
      ajaxRequest.onreadystatechange = function()
      {
        if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
        {
          var ajaxResult = ajaxRequest.responseText;
          var tmpArray = ajaxResult.split("|");
          document.getElementById('mov').innerHTML = tmpArray[0];
          document.getElementById('wsk').innerHTML = tmpArray[1];
          document.getElementById('balsk').innerHTML = tmpArray[2];
          document.getElementById('strg').innerHTML = tmpArray[3];
          document.getElementById('tgh').innerHTML = tmpArray[4];
          document.getElementById('wnd').innerHTML = tmpArray[5];
          document.getElementById('att').innerHTML = tmpArray[6];
          document.getElementById('lds').innerHTML = tmpArray[7];
          document.getElementById('sv').innerHTML = tmpArray[8];
        }
      }
      ajaxRequest.send();
    }
 
    var myVar1 = setInterval(updateTemp, 5000);  
    var myVar2 = setInterval(updateTime, 1000);  
 
  </script>
 
 
  <title>Unit Monitor</title>
 </head>
 
 <body>
   <div id='main'>
     <h2>Battlesheet</h2>
     <div id='content'> 
       <p id='P_time'>Time</p>
       <h2>Unit Name</h2>
       <p> <span id='name'>--</span> % </p>
       <h2>Unit Statistics</h2>
       <p> Movement: <span id='mov'>-</span>" &nbsp;
       | Weapon Skill: &nbsp; <span id='wsk'>-</span>+ 
       | Ballistic Skill: &nbsp; <span id='balsk'>-</span>+ 
       | Strength: &nbsp; <span id='strg'>-</span> 
       | Toughness: &nbsp; <span id='tgh'>-</span> 
       | Wounds: &nbsp; <span id='wnd'>-</span> 
       | Attacks: &nbsp; <span id='att'>-</span> 
       | Leadership: &nbsp; <span id='lds'>-</span>
       | Save: &nbsp; <span id='sv'>-</span>+</p>
       <h2>Ballistic Weapons</h2>
       <p> <span id='sv'>--</span> % </p>
       <h2>Close-Quarter Weapons</h2>
       <p> <span id='wnd'>--</span> % </p>
     </div>
   </div> 
 </body>
</html>
)====="; 
#include <SPI.h>
#include <MFRC522.h> 
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN 2   
#define LED_COUNT 12
#define RST_PIN         0           // Configurable, see typical pin layout above
#define SS_PIN          15          // Configurable, see typical pin layout above


MFRC522 rfid(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
unsigned long ID;  // convert 3 byte to ulong 
// change these values to match your network
char ssid[] = "FRITZ!Box 7312";       //  your network SSID (name)
char pass[] = "87147510181205287046";          //  your network password

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  
float tempF = 0;
float tempC = 0;
float humid = 0;
 
WiFiServer server(80);
String request = "";
 
 
void setup() 
{
  
    #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
   clock_prescale_set(clock_div_1);
    #endif
    // END of Trinket-specific code.

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }

    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    
    Serial.begin(9600);
    SPI.begin();        // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522
    // definiere KEY (default) FF FF FF FF FF FF
    for (byte i = 0; i < 6; i++) {
       key.keyByte[i] = 0xFF;
    }
    rfid.PCD_SetAntennaGain(rfid.RxGain_max);

    Serial.println();
    Serial.println("Serial started at 9600");
    Serial.println();
    //dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);
    // Connect to a WiFi network
    Serial.print(F("Connecting to "));  Serial.println(ssid);
    WiFi.begin(ssid, pass);
 
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");
        delay(500);
        uint32_t color=strip.Color(255,   0,   0);
          for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
          strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
          strip.show();                          //  Update strip to match
        }
    }
 
    Serial.println("");
    Serial.println(F("[CONNECTED]"));
    Serial.print("[IP ");              
    Serial.print(WiFi.localIP()); 
    Serial.println("]");
 
    // start a server
    server.begin();
    Serial.println("Server started");
 
} // void setup()
 
 
void loop() 
{

    
    uint32_t color=strip.Color(0,  255,   0);
        for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
          strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
          strip.show();                          //  Update strip to match
        }
    WiFiClient client = server.available();     // Check if a client has connected
    if (!client)  {  return;  }
 
    request = client.readStringUntil('\r');     // Read the first line of the request
 
    Serial.println(request);
    Serial.println("");
 
    if ( request.indexOf("getTemp") > 0 )
     { 
          Serial.println("getTemp received");

                // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;
    byte trailerBlock   = 7;
    byte sector         = 1;
    byte blockAddr      = 4;

    uint32_t color=strip.Color(0,  0,   0);
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
          strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
          strip.show();                          //  Update strip to match
    }
   
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    status = (MFRC522::StatusCode) rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
        return;
    }

    status = (MFRC522::StatusCode) rfid.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
    }

          int mov = (buffer[0]);
          int wesk = (buffer[1]);       
          int balsk = (buffer[2]);
          int strg = (buffer[3]);
          int tgh = (buffer[4]);
          int wnd = (buffer[5]);
          int att = (buffer[6]);
          int lds = (buffer[7]);
          int sv = (buffer[8]);

          Serial.println(mov);
           
          if ( !isnan(mov) && !isnan(wesk) && !isnan(balsk)  && !isnan(strg) && !isnan(tgh) && !isnan(wnd) && !isnan(att) && !isnan(lds) && !isnan(sv))
          {
              client.print( header );
              client.print( mov );
              client.print( "|" );
              client.print( wesk );
              client.print( "|" );
              client.print( balsk );
              client.print( "|" ); 
              client.print( strg );
              client.print( "|" );  
              client.print( tgh );  
              client.print( "|" );  
              client.print( wnd ); 
              client.print( "|" );  
              client.print( att ); 
              client.print( "|" );  
              client.print( lds ); 
              client.print( "|" );  
              client.print( sv );
              Serial.println("data sent");
           }
          else
          {
              Serial.println("Error reading the sensor");
          }
     }
 
     else
     {
        client.flush();
        client.print( header );
        client.print( html_1 ); 
        Serial.println("New page served");
     }
 
    delay(5);
 
 
  // The client will actually be disconnected when the function returns and the 'client' object is destroyed
 
} // void loop()
