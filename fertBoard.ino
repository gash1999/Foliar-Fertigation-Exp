
#include<WiFi.h>

int leftSolenoid = D2;
int rightSolenoid = D3;
int pump = D5;
const int wet_sensor = A0; 
int val_wet ;  // variable to store the value read
float voltage;
const int maxLeaf =0.5;

// ThingSpeak settings
    char server[] = "api.thingspeak.com";
    unsigned long  channelID = 2820739;
    const char* apiReadKey = "PXZI6OG13O5QMZW5";
    String writeAPIKey = "B8FJ36JXSB115B8K";

// Create an instance of the client library
    WiFiClient client;  
      

// WiFi parameters
char* ssid = "agrotech";
const char* password = "1Afuna2gezer";
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
//function to check wet sensor
 bool wetAlert() {
                val_wet = analogRead(wet_sensor);  // read the wet sensor 
                voltage = (2.5 / 4095) * val_wet;  // convert to voltage according to ADC
                Serial.print("Voltage: ");
                Serial.println(voltage);
                return voltage > maxLeaf;
            }

// the Setup function runs once 
void setup() {
  Serial.begin(115200); //
  // initialize digital pin as an output.
  pinMode(leftSolenoid, OUTPUT);
  pinMode(rightSolenoid, OUTPUT);
  pinMode(pump, OUTPUT);
  digitalWrite(pump,LOW);
  digitalWrite(leftSolenoid,LOW);
  digitalWrite(rightSolenoid,LOW);


}

  // reading instractions from web server
  bool readFertStatus() {
    if (client.connect(server, 80)) {
        client.print("GET /channels/");
        client.print(channelID);
        client.print("/fields/1/last?api_key=");
        client.print(apiReadKey);
        client.println();
        while (client.connected() && !client.available()){

        }
        delay(1); // wait for data
        String line = client.readStringUntil('\n');
        client.stop();
        Serial.println(line);
        return line == "1"; 
    }
    return false; 
    } 

 
    
    // reading instractions from web server
  bool readFertStatus2() {
    if (client.connect(server, 80)) {
        client.print("GET /channels/");
        client.print(channelID);
        client.print("/fields/2/last?api_key=");
        client.print(apiReadKey);
        client.println();
        while (client.connected() && !client.available()){
        } 
        delay(1); // wait for data
        String line = client.readStringUntil('\n');
        client.stop();
        Serial.println(line);
        return line == "1"; 
    }
    return false; 
    }  

// the loop function runs over and over again forever
void loop() {

    // In each loop, make sure there is an Internet connection.
        if (WiFi.status() != WL_CONNECTED) { 
            connectWiFi(); }

        if(!(readFertStatus() and !wetAlert()) and !(readFertStatus2())) {
            digitalWrite(pump,LOW);
            digitalWrite(rightSolenoid,LOW);
            digitalWrite(leftSolenoid,LOW);
        // post stop to serial monitor
        Serial.println("stopped from line 137");
        }
        else
        {
          Serial.println("One of the valves is open");
                if (readFertStatus() and !wetAlert()) {
                  digitalWrite(rightSolenoid,HIGH);
                  digitalWrite(pump,HIGH);
            
                  // post start to serial monitor
                  Serial.println("started from line 116");
      }
                else {
                  Serial.println("Right Solenoid Off");
                  digitalWrite(rightSolenoid,LOW);
                }
        }
  // use information from web server to the right_solenoid

        
        // use instractions from web server to the left_solenoid (stuiped)
        if (readFertStatus2()) {
            digitalWrite(leftSolenoid,HIGH);
            digitalWrite(pump,HIGH);
            // post start to serial monitor
            Serial.println("started from line 131");
            }
            else {
              Serial.println("Left Solenoid Off");
              digitalWrite(leftSolenoid,LOW);
            }

            
      }

       
                
