#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <fs.h>
#include "rootpage.h"
#include "bootstrap.h"



/**
 *
 * HX711 Snow Weight example file
 *
 *
 * 
 *
**/
#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D2;
const int LOADCELL_SCK_PIN = D3;

const char* ssid     = "RoofScale";
const char* password = "123456789";
IPAddress IP(192,168,002,001);
IPAddress GW(192,168,002,001);

IPAddress MASK(255,255,255,000);

HX711 scale;
ESP8266WebServer server(80);
DNSServer dnsServer;

const byte DNS_PORT = 53;

int printTime = 500;
long elapsedTime = 0;
float accumulator = 0;
float alpha = .3;
float average = 0;
float number = 0;
float n = 10;
float w = 0;
float error = 1;

void handleWeight(){
  Serial.println("attempted to handle weight");
  server.send(200, "text/plain",String(average));
//    server.send(200, "text/html", "<h1>You are connected to weight</h1>");
}
void handleRoot(){
  String s = MAIN_page;
  server.send(200,"text/html",s);
  Serial.println("attempted to handle root");
//  server.send(200, "text/html", "<h1>You are connected to main</h1>");
}

void handleTare(){
  scale.tare();
  server.send(200, "text/plain","Scale zeroed");
}
void handleCSS(){
  String s = simple_css;
  Serial.println("attempted to send css");
  Serial.println(s);

  server.send(200, "text/css",s);
//    server.send(200, "text/css", "You are downloading css");
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.setDebugOutput(true);
//  WiFi.disconnect();
  

//  WiFi.begin("heatmor-secure","9XwZetTxcE3e");
//  // Wait for connection
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//  }

  Serial.println("");
  
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(IP, GW, MASK)? "Ready" : "Failed!");
  
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid, password)? "Ready" : "Failed!");
  delay(2000);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", IP);
  
  Serial.println(WiFi.softAPIP());
  Serial.println(WiFi.localIP());
  if (MDNS.begin("scale")) {              // Start the mDNS responder for scale.local
  Serial.println("mDNS responder started");
  } else {
  Serial.println("Error setting up MDNS responder!");
  }

  server.on("/",HTTP_GET, handleRoot);
  server.on("/getWeight", HTTP_GET, handleWeight);
  server.on("/tare", HTTP_GET, handleTare);
  //server.on("/bootstrap.css",handleCSS);

    // replay to all requests with same HTML
  server.onNotFound([]() {
    server.send(200, "text/html", MAIN_page);
  });
  server.begin();

  Serial.println("");
  Serial.println("Initializing the scale");

  // Initialize library 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("scale initalized");
  // Set round scale with plexiglas top to weigh grams
  scale.set_scale(371.f); // 1 gram
  Serial.println("scale set to 1 gram");
  //  1g = 0.00220462 lb
  //scale.set_scale(165168.f);
  //  1g = 0.035274 oz
  //scale.set_scale(10518.f);
  
  
  scale.tare();				        // reset the scale to 0
}



void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  
  // simple moving average
  w = scale.get_units();
  error = (abs(w-average)*.4);
  if(error<1)
    error=1;
  if(error>2)
    error=2;
  average -= average/(n);
  average += (w/n);

  //weighted moving average
  float alpha = .1;
  accumulator = (alpha * w)+(1-alpha)*accumulator;
  

  
  if(elapsedTime < millis()){
    
//    Serial.print("one reading:\t");
//    Serial.print(scale.get_units(), 1);
//    float number = scale.get_units(20);
    Serial.print("running average:\t");
    Serial.print(average, 1);
    Serial.print("\t| moving average:\t");
    Serial.print(accumulator, 3);
//    Serial.print("\t| get_value:\t");
//    Serial.print(scale.get_units(20));
//    Serial.print("\t| read_average:\t");
//    Serial.println(scale.read_average(20));
    Serial.print("\t| conntected devices: \t");
    Serial.print(WiFi.softAPgetStationNum());
    Serial.println("");
    elapsedTime=millis()+printTime;

  }

  if (Serial.available()>0){
    int incomingByte = Serial.read();
    if(char(incomingByte)=='t');
      scale.tare();
      average = 0;
    if (char(incomingByte == 'c')){
      while(Serial.available()>0){
        char t = Serial.read();
      }
      Serial.println("Clear Scale...");
//      delay(500);
      scale.tare();
      Serial.println("Place known weight on scale and enter weight...");
      float weight = 10;
      while(Serial.available()>0){
        char t = Serial.read();
        Serial.print("data found: ");
        Serial.println(t);
      }

      Serial.setTimeout(15000);
      weight = Serial.parseFloat();
      Serial.println(weight);
      scale.set_scale(scale.get_value(20)/weight);

      Serial.println("Clear Scale...");
      delay(5000);
      scale.tare();

    }
    
  }
  //scale.power_down();			        // put the ADC in sleep mode
 
 
}
