#include "WiFi.h"
#include "PubSubClient.h" //pio lib install "knolleary/PubSubClient"
#include "Button.h"

#define SSID "NETGEAR68"
#define PWD "excitedtuba713"

#define MQTT_SERVER "192.168.1.2" // could change if the setup is moved
#define MQTT_PORT 1883

#define LED_PIN 2

int aantalPressed;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void callback(char *topic, byte *message, unsigned int length);

// function for establishing wifi connection, do not touch
void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi..");

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// general setup, add lines if needed
void setup()
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  // This statement will declare pin 15 as digital input 
  pinMode(PushButton, INPUT_PULLUP);

  // Attach an interrupt to the ISR vector
  attachInterrupt(15, pin_ISR, CHANGE);

  Serial.begin(115200);
  while(!Serial) {}

  aantalPressed = 0;

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  Serial.println("Setup ready");
}

// callback function, only used when receiving messages
void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  
  if (String(topic) == "esp32/fitness")
  {
    Serial.print("Changing state to ");
    if (messageTemp == "start")
    {
      Serial.println("start");
      digitalWrite(LED_PIN, HIGH);
    }
    else if (messageTemp == "stop")
    {
      Serial.println("stop");
      digitalWrite(LED_PIN, LOW);
    }
    else if (messageTemp == "reset")
    {
      Serial.println("reset");
      setup();
    }
    }
}

// function to establish MQTT connection
void reconnect()
{
  delay(10);
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
      Serial.println("connected");
      // Publish
      client.publish("esp32/fitness/OKmessage", "OK");
      // ... and resubscribe
      
      int getal = esp_random() %5+1;
      String nummer = (String) getal;
      const char* nmr = nummer.c_str();
      client.publish("esp32/fitness/nmrOef", nmr);
      client.subscribe("esp32/fitness");
     
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// loop function for recurring functionality
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  delay(500);

  if(buttonPressed == true && aantalPressed==0){
     client.publish("esp32/fitness", "Button");
     delay(1000); 
     buttonPressed = false;
     aantalPressed ++;
  }

  else if(buttonPressed == true && aantalPressed != 0 ){
     client.publish("esp32/fitness", "oefOK");
     delay(1000);
      buttonPressed = false;
     aantalPressed ++;
 
  }

  // if(buttonPressed == true && aantalPressed==1){
  //    client.publish("esp32/fitness", "oef");
  //    buttonPressed = false;
  //    aantalPressed ++;
  // delay(1000);
  // }


}