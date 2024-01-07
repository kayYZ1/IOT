#define BLYNK_TEMPLATE_ID "..."
#define BLYNK_TEMPLATE_NAME "..."
#define BLYNK_AUTH_TOKEN "..."

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

int ledPin = 2; // choose the pin for the LED
int inputPin = 5; // choose the input pin (for PIR sensor)
int pirState = true; // we start, assuming no motion detected
int val = 0; // variable for reading the pin status
int minimummSecsLowForInactive = 5000; // If the sensor reports low for
long unsigned int timeLow;
boolean takeLowTime;
 
//30 seconds for PIR sensor to calibrate
int calibrationTime = 30;

const char* ssid     = "..";
const char* password = "..";

void initWifi() 
{
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  int timeout = 10 * 4; // 10 seconds
  while (WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) 
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: ");
  Serial.print(millis());
  Serial.print(", IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() 
{
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT); 
 
  Serial.begin(115200);
 
  //Initialize Wifi connection
  initWifi();
  //Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  //Calibrate sensor
  Serial.print("Calibrating sensor[");
  for(int i = 0; i < calibrationTime; i++)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("]done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}
 
void loop()
{
  Blynk.run();
  val = digitalRead(inputPin);
  if (val == HIGH) 
  { 
    digitalWrite(ledPin, HIGH); 
    if (pirState) 
    {
      pirState = false;
      Serial.println("Motion detected!");
      Blynk.logEvent("motion_alert", "Motion detected");
      delay(50);
    }
    takeLowTime = true;
  } 
  else 
  {
    digitalWrite(ledPin, LOW);
 
    if (takeLowTime)
    {
      timeLow = millis();
      takeLowTime = false;
    }
 
    if(!pirState && millis() - timeLow > minimummSecsLowForInactive) 
    {
      pirState = true;
      Serial.println("Motion ended!");
      delay(50);
    }
  }
}