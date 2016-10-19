/*TerMITe_qt1010 Interface
     Please make sure that you include the following link in Arduinos Boards Manager.
     This allows you to install the neccessary ESP8266 libraries:
     http://arduino.esp8266.com/stable/package_esp8266com_index.json

    Settings:

    ------------TerMITe _ v9 ----------------
    Board-----------> "Generic ESP8266 Module"
    Flash Mode------> "DIO"
    Flash Frequency-> "80 MHz"
    CPU Frequency---> "160 MHz"
    Flash Size------> "4MB (3mb Spiffs)"

                             . GND
        RESET  .             . GPIO_0
          ADC  .             . TXD
    ^ GPIO_14  .             . RXD
    ^ GPIO_12  .             . GPIO_5 ^
    ^ GPIO_13  .             . GPIO_4 ^ --> Digital Read QT1010
      3.3V     .             . GPIO_3
      GPIO_9   .             . GPIO_10
                   . . . .
               GND USBp USBm 5V
                    [USB]

              B               A

      [To Upload codes press A, then B]

    ------------AT42QT1010 _ Touch IC----------

             OUT  .    . MODE (normally off)
             GND  .    . VCC
            SNSK  .    . SNS



*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <CapacitiveSensor.h>
#include <PubSubClient.h>

String tChipID =  String(ESP.getChipId());

const char* ssid = "MIT";
const char* password = "";
const char* mqtt_server = "replace.media.mit.edu";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int msgDelay = 500;
int inputPin = 4;
boolean wasReleased = true;

int threshold = 0;

int samples = 1000;
CapacitiveSensor sensor0 = CapacitiveSensor(4, 5);

//Low pass butterworth filter order=1 alpha1=0.01 
class  Filter
{
  public:
    Filter()
    {
      v[0]=0.0;
    }
  private:
    float v[2];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = (3.046874709125380054e-2 * x)
         + (0.93906250581749239892 * v[0]);
      return 
         (v[0] + v[1]);
    }
};

Filter filter;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //digitalWrite(inputPin, LOW);
  //pinMode(inputPin, INPUT);
  filter = Filter();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
 * Remote settup 
 * th=100 -> sets threshold
 * md=100 -> sets msgDelay (ms)
 */
 
void callback(char* topic, byte* payload, unsigned int length) {
  //Payload to String
  payload[length] = '\0';
  String receivedString = String((char*)payload);
  Serial.println("STRING = " +receivedString); 
  if(receivedString.substring(0,2) == "th"){
    receivedString.remove(0, 3);  
    threshold = receivedString.toInt();
    Serial.println("New Threshold = " + String(threshold));
  }
  else if(receivedString.substring(0,2) == "td"){
    receivedString.remove(0, 3);  
    msgDelay = receivedString.toInt();
    Serial.println("New msgDelay = " + String(msgDelay));
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {      
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...             
      client.publish("outTopic", "hello world"); 
      // ... and resubscribe
      String inTopic = "sandbox/"+ tChipID +"/settings";
      client.subscribe(inTopic.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
int valueSensor = 0;

void loop() {

  //Reading capacitive sensor
  valueSensor =  filter.step(sensor0.capacitiveSensor(samples));

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > msgDelay && valueSensor > threshold) {
    
    lastMsg = now;
    //valueSensor = digitalRead(inputPin);
                  Serial.println(valueSensor);
    String packet = "";
    packet += tChipID;
    packet += "&";
    packet += String(getBatteryLevel());
    packet += "&";
    packet += String(valueSensor);
    packet.toCharArray(msg, 50);
    
    
    if (valueSensor != 0) {
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("sandbox/keytouch/sensor", msg);
      wasReleased = false;
    }
    else
      wasReleased = true;

    
  }

}

float getBatteryLevel() {
  ///////////////////////////////////////////////
  ////////////Get voltage from ADC///////////////
  ///////////////////////////////////////////////

  float tBatteryLevel = analogRead(A0);
  //Serial.print("Battery ADC: ");
  //Serial.println(tBatteryLevel);
  //convert back into battery voltage from ADC

  ///// Voltage Divider to lower 4.2v to 0.955v
  ///// R1 = 680 kOhm
  ///// R2 = 200 kOhm

  tBatteryLevel = tBatteryLevel / 0.955; // for esp8266 ADC
  //tBatteryLevel = tBatteryLevel * 1.1518; // convert back to 1.1v ADC

  tBatteryLevel /= 1024.0;

  //convert back to 4.2v
  tBatteryLevel = tBatteryLevel * 4.2;

  // print out the battery level
  //Serial.print("Battery Level: ");
  //Serial.println(tBatteryLevel);

  return tBatteryLevel;


}


