
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <ArduinoJson.h>

const char* ssid = "bla bla";
const char* password = "bla bla";

const size_t CAPACITY = JSON_ARRAY_SIZE(50);
StaticJsonDocument<CAPACITY> doc;
JsonArray array = doc.to<JsonArray>();
String lastDataSet;

AsyncWebServer server(80);

unsigned int bufferSize = 50;
unsigned int beats[50];
unsigned int currentIndex = 0;
unsigned long nextMilis = 0;
unsigned int frequencyMs = 10;

struct second_time_frame{
  unsigned short values_read[50];
  unsigned short second;
  unsigned short currentIndex = 0;
};

second_time_frame measures[50];

void initBeats(){
    for(int i = 0; i < bufferSize; i++){
        beats[i] = 0;
    }
}

void setupWebServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  server.on("/beat", HTTP_GET, [](AsyncWebServerRequest *request){
    String opa;
    serializeJson(doc, opa);

    currentIndex = 0;
    doc.clear();
    Serial.println("Buffer restarted");

    request->send_P(200, "text/plain", opa.c_str());

    Serial.println(opa);    
    array = doc.to<JsonArray>();
    doc.garbageCollect();

    //StaticJsonDocument<CAPACITY> doc;
    //array = doc.to<JsonArray>();
  });
  
  server.begin();
}

void setup(){
  Serial.begin(115200);

  initBeats();
  
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());
}

unsigned int getMax(second_time_frame * frame){
  return 0;
}

unsigned int getMin(second_time_frame * frame){
  return 0;
}

void measure(second_time_frame * frame){
  second_time_frame * current = &measures[currentIndex];

  int val = analogRead(A0);
  current->values_read[current->currentIndex] = (unsigned short) val;

  current->currentIndex++;  
}

unsigned short getBPM(){

}

void startMeasuring(){
  measure(nullptr);
}

void stopMeasuring(){

}

void loop(){
    if(nextMilis == 0){
        nextMilis = millis() + frequencyMs;
    }

    if(millis() < nextMilis){
        return;
    }

    startMeasuring();

    int val = analogRead(A0);
    if(val > 800){
        array.add(val);
        currentIndex ++;
        nextMilis = millis() + 400;
        Serial.print( "Index: ");
        Serial.print(currentIndex);
        Serial.print( " time: ");
        Serial.print(millis());
        Serial.print( " - ");
        Serial.print(val);
        Serial.println("");
        return;
    }
    //beats[currentIndex] = val;
        
    if(currentIndex >= bufferSize){
        Serial.println("Buffer reached, restarting...");
        currentIndex = 0;
        //initBeats();
        serializeJson(doc, lastDataSet);
        doc.clear();
    }        

    nextMilis = 0;
}