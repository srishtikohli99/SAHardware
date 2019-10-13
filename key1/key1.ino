#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

#include <ESP8266HTTPClient.h>

#include <Keypad.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include<HX711.h>
#include<Wire.h>
#include<SD.h>
const int LOADCELL_DOUT_PIN = 10;
const int LOADCELL_SCK_PIN = D8;

HX711 scale;

int prev = 0;
 
const byte n_rows = 4;
const byte n_cols = 4;
 
char keys[n_rows][n_cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
 
byte colPins[n_rows] = {D3, D2, D1, D0};
byte rowPins[n_cols] = {D7, D6, D5, D4};
 
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols); 
 
void setup(){
  Serial.begin(115200);    
  float calibration_factor = -21.1;
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(1200);//Serial connection
  WiFi.begin("Airtel-Hotspot-7651", "66r85ffr");   //WiFi connection
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
 
    delay(500);
    Serial.println("Waiting for connection");
 
  }
}
 
void loop(){
 // Serial.println("Wifi Connected");
   char myKey = myKeypad.getKey();

  if (myKey != NULL){
    Serial.print("Key pressed: ");
    Serial.println(myKey);
    HTTPClient http;    //Declare object of class HTTPClient 
    String base = "http://192.168.1.100:8000/firstapp/fire/";
    String hit = base + myKey + '/';
   http.begin(hit);      //Specify request destination
   int httpCode = http.GET();
   //http.addHeader("Content-Type", "text/plain");  //Specify content-type header
 
   /*int httpCode = http.POST("Message from ESP8266");   //Send the request
   String payload = http.getString();                 //Get the response payload
 
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println(payload);    //Print request response payload */
 
   http.end();  //Close connection
 }

  if (scale.is_ready()) {
    int reading = scale.read();
    Serial.print("HX711 reading: ");
    reading = reading - 156000;
    
    if((reading > 15000) && (prev < 15000)){
    Serial.println(reading);  
    HTTPClient http;
    String base = "http://192.168.1.100:8000/firstapp/load/";
    String hit = base +"loadyes" + '/';
    Serial.println(hit);
   http.begin(hit);      //Specify request destination
   int httpCode = http.GET();
    }
    else if((reading < 15000) && (prev > 15000)){
      HTTPClient http;
      String base = "http://192.168.1.100:8000/firstapp/load/";
      Serial.println(reading);
    String hit = base + "loadno" + '/';
    Serial.println(hit);
   http.begin(hit);      //Specify request destination
   int httpCode = http.GET();
    }
    prev = reading;
  } 
  else {
    Serial.println("HX711 not found.");
  }

 
 if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
    //Serial.println("hit");
   
 
 }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
 //delay(200);
 

}
