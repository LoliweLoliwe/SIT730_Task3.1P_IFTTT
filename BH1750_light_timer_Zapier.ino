#include <WiFiNINA.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <BH1750FVI.h>

const char * ssid = "xxx";
const char * password ="xxx";

WiFiClient client;

char   HOST_NAME[] = "hooks.zapier.com";
String PATH_NAME   = "/hooks/catch/xxx/"; // change your EVENT-NAME and YOUR-KEY

// Create the Lightsensor instance
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);

//static uint16_t days;
static uint8_t hours, minutes, seconds;

uint32_t  remainingSeconds = 7200; /* 0d 2h 00m 00s */
int count=0;
int done=1;
bool emailSent = false;

static void TimeCalc(uint32_t  secs)
{
    seconds = secs % (uint32_t)60;
    secs /= (uint32_t)60;
    minutes = secs % (uint32_t)60;
    secs /= (uint32_t)60;
    hours = secs % (uint32_t)24;
    //days = secs / (uint32_t)32;
}

void setup() 
{
  // starts the serial communication:
  Serial.begin(9600);
  delay(10);
  
  // connect to WiFi network
  WiFi.begin(ssid, password);

  LightSensor.begin();
}

void loop()
{
  uint16_t lux = LightSensor.GetLightIntensity();
  
  Serial.print("Light: ");
  Serial.println(lux);
  delay(500);
   
  if (remainingSeconds == 0) {
    Serial.print("Fully charged. ");

    if (emailSent == false) {  // to make sure that Arduino does not send duplicated emails
      sendIFTTTemail(lux);
      delay(3000);
      emailSent = true;
    } else {
      emailSent = false;  // reset to send if the temperature exceeds threshold again
    }
    
  } else {

    if (lux != 0) {
      if (emailSent == false) {  // to make sure that Arduino does not send duplicated emails
        sendIFTTTemail(lux);
        delay(3000);
        emailSent = true;
      } else {
        emailSent = false;  // reset to send if the temperature exceeds threshold again
      }
      
      switch (count) {
        case 0:
          TimeCalc(remainingSeconds);
  
          //Serial.print(days);
          //Serial.print("d ");
          Serial.print(hours);
          Serial.print("h ");
          Serial.print(minutes);
          Serial.print("m ");
          Serial.println(seconds);
          //Serial.print("s");
  
          remainingSeconds --;
          delay(1000);
          break;
     }
    } else {
      Serial.print("Stopped charging!!");

      if (emailSent == false) {  // to make sure that Arduino does not send duplicated emails
        sendIFTTTemail(lux);
        delay(3000);
        emailSent = true;
      } else {
        emailSent = false;  // reset to send if the temperature exceeds threshold again
      }
      
      done = 1; count += done;
    }
  }
}

void sendIFTTTemail(uint16_t lux){
  // make a HTTP request:
  // send HTTP header
  String queryString = String(lux)+"?value1=" + "?value2=";
  client.println("POST " + PATH_NAME + queryString + " HTTP/1.1");
  client.println("Host: " + String(HOST_NAME));
  client.println("Connection: close");
  client.println(); // end HTTP header

  // the server's disconnected, stop the client:
  /*client.stop();
  Serial.println();
  Serial.println("disconnected");*/
}
