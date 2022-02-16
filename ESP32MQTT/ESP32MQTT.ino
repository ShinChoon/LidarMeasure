#include <WiFi.h>
#include <PubSubClient.h>
// CMP3 subscriber
String countx = "";
String county = "";
char XC[50];
char YC[50];
#define RXp2 16
#define TXp2 17

// Update these with values suitable for your network.
const char* ssid = "Xin";
const char* password = "aaaabbbb";
const char* mqtt_server = "192.168.137.1";

WiFiClient espClient;
PubSubClient client(espClient);

IPAddress server(192,168,137,1);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    String clientID = "arduinoClient";
    clientID += String(random(0xffff), HEX);
    if (client.connect(clientID.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}


void setup() {

  // start the serial connection
  Serial2.begin(115200, SERIAL_8N1, RXp2, TXp2);
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);
  // we are connected
  Serial.println();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("WIFI connected");

}

void loop() {
  Serial.println("Checking");
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // save count to the 'counter' feed on Adafruit IO
//  if (Serial2.available() > 0)  {
      countx = Serial2.readStringUntil('\n');
      //if (countx.length()>=6){
        

      int startX = countx.indexOf("<");
      int endX = countx.indexOf(",");
      int endY = countx.indexOf(">");

      snprintf (XC, 50, "%s", countx.substring(startX+1, endX));
      snprintf (YC, 50, "%s", countx.substring(endX+1,endY));
      
      Serial.println(XC);
      Serial.println(YC);
      client.publish("ZHONGXIN/feeds/x-value", XC);
      client.publish("ZHONGXIN/feeds/y-value", YC);
          //}
//  }
  delay(50);

}
