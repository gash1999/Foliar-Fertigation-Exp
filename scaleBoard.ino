/**
 * TCA9548 I2CScanner.ino -- I2C bus scanner for Arduino
 *
 * Based on https://playground.arduino.cc/Main/I2cScanner/
 *
 */

#include "Wire.h"
#include <Adafruit_NAU7802.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TCAADDR 0x70
Adafruit_NAU7802 nau1;
Adafruit_NAU7802 nau2;
int32_t val;
const float timeGap = 0.15; // Time gap between readings in minutes
const int count = 30; // Number of readings to takei
// calibration factors
float aCoef = 0.0;
float bCoef = 0.0;

// Create an instance of the client library
    WiFiClient client; 
    PubSubClient mqtt(client);

// Network information
    char* ssid = "agrotech";
    const char* password = "1Afuna2gezer";

// ThingSpeak settings
    char server[] = "api.thingspeak.com";
    unsigned long  channelID = 2820739;
    String writeAPIKey = "0E2ETUHX1YEB5H7P";
    const char* apiReadKey = "PXZI6OG13O5QMZW5";

//switch between chanels on the TCA9548A (multiplexer)
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

// MQTT broker settings
const char* mqtt_server = "192.168.0.102"; // IP address of home assistant
const int mqtt_port = 1883; // Port of the MQTT broker
const char* mqtt_user = "mqtt-user"; // MQTT username
const char* mqtt_password = "1234"; // MQTT password
const char* mqtt_topic = "/greenhouse/outside/irrigation/solenoid8"; // MQTT topic


void setup()
{
  delay(1000);  // Allow devices to stabilize
  mqtt.setServer(mqtt_server, mqtt_port);
  Wire.begin();  // Initialize I2C communication
  Serial.begin(115200);  // Start serial communication
  Serial.println("\nTCAScanner ready!");

  // Initialize first NAU7802 sensor
  tcaselect(0);
  Serial.println("NAU7802");
  if (!nau1.begin()) {
    Serial.println("Failed to find NAU7802");
    while (1) delay(10);  // Halt if the sensor is not found
  }
  Serial.println("Found NAU7802 1");

  // Configure LDO, Gain, and Rate
  nau1.setLDO(NAU7802_3V0);  // Set LDO voltage to 3.0V
  Serial.println("LDO voltage set to 3.0V");

  nau1.setGain(NAU7802_GAIN_128);  // Set gain to 128x
  Serial.println("Gain set to 128x");

  nau1.setRate(NAU7802_RATE_10SPS);  // Set conversion rate to 10 SPS
  Serial.println("Conversion rate set to 10 SPS");

  // Skip calibration steps for raw absolute values
  Serial.println("Skipping all calibrations for absolute raw values.");

  // Initialize second NAU7802 sensor
  tcaselect(1);
  Serial.println("NAU7802");
  if (!nau2.begin()) {
    Serial.println("Failed to find NAU7802");
    while (1) delay(10);  // Halt if the sensor is not found
  }
  Serial.println("Found NAU7802 2");

  // Configure LDO, Gain, and Rate for the second sensor
  nau2.setLDO(NAU7802_3V0);  // Set LDO voltage to 3.0V
  Serial.println("LDO voltage set to 3.0V");

  nau2.setGain(NAU7802_GAIN_128);  // Set gain to 128x
  Serial.println("Gain set to 128x");

  nau2.setRate(NAU7802_RATE_10SPS);  // Set conversion rate to 10 SPS
  Serial.println("Conversion rate set to 10 SPS");

  // Skip calibration steps for the second sensor as well
  Serial.println("Skipping all calibrations for absolute raw values.");
}

//colect data from the web server   
void httpRequest(float field1, float field2) {
    if (!client.connect(server, 80)) {
        Serial.println("Connection to ThingSpeak failed.");
        return;
    }

    String data = "field1=" + String(field1) + "&field2=" + String(field2);
    Serial.println("Sending data: " + data);

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
int timeout = 5000; // 5 seconds timeout
    long start = millis();
    while (client.connected() && (millis() - start < timeout)) {
        if (client.available()) {
            String response = client.readString();
            Serial.println("Server Response:");
            Serial.println(response); // Print the entire response
            break;
        }
    }
    client.stop();
}

// reading watering instructions from web server
  float readwaterStatus() {
    if (client.connect(server, 80)) {
        client.print("GET /channels/");
        client.print(channelID);
        client.print("/fields/5/last?api_key=");
        client.print(apiReadKey);
        client.println();
        while (client.connected() && !client.available()){

        }
        delay(1); // wait for data
        String line = client.readStringUntil('\n');
        client.stop();
        Serial.println(line);
        return float (line); 
    }
    return 0; 
    }




void connectWiFi() {
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

void readScales(int scale1[], int scale2[],int length) {
  for (int i = 0; i < length;i++){
  //scale1
  tcaselect(0);
  while (! nau1.available()) {
    delay(1);
  }
  // Read the raw value from the sensor to array
  scale1[i] = nau1.read();
  Serial.print("Read Scale 1 "); Serial.println(scale1[i]);
  //scale2
  tcaselect(1);
  while (! nau2.available()) {
    delay(1);
  }
  // Read the raw value from the sensor to array
  scale2[i] = nau2.read();
  Serial.print("Read Scale 2 "); Serial.println(scale2[i]);
  
  delay(60000*timeGap); // Wait 1 minute before beginning again
}
}
//sort the array
void sortArray(int data[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (data[j] > data[j + 1]) {
                // Swap the elements
                float temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}

int median(int data[],int length) {
  //sort the array
 sortArray(data,length);
    // Find the median
  if (length % 2 == 0) {
      return (data[length/2] + data[length/2 - 1]) / 2;
    } else {
      return data[length/2];
    } 
}

// result in grams
float convertToGrams(int32_t val,float aCoef,float bCoef) {
  return val * aCoef + bCoef; 
}

// read data from thingspeak
void loop() {
  //`connect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  int scale1data[count],scale2data[count];
  //read data from scales to arrays
  readScales(scale1data,scale2data,count); 
  //calculate median of the datas
  int median1 = median(scale1data,count);
  int median2 = median(scale2data,count);
  // calculate the weight in grams
  float weight1 = convertToGrams(median1,0.0098,-1298);
  float weight2 = convertToGrams(median2,0.0098,1105.5);
  httpRequest(weight1,weight2);
  Serial.print("Median of Scale 1:");
  Serial.println(median1);
  Serial.print("Median of Scale 2:");
  Serial.println(median2);
  Serial.println(weight1);
  Serial.println(weight2);

  delay(6000); // Wait 1 minute before beginning again

  //read data from ThinkSpeak
if(readwaterStatus()>0){
  Serial.println("Watering the plants");
  float volume=readwaterStatus();
  //estimates 2 L/Hour
  float timeToWater=volume/7.2;
  mqtt.publish(mqtt_topic, "1");
    delay(1000*timeToWater); // Wait 2 minutes before beginning again
    mqtt.publish(mqtt_topic, "0");
  
}
  else{
    Serial.println("Not watering the plants");
    mqtt.publish(mqtt_topic, "0");
  }
}
