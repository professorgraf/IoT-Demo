#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <DHT.h>

#define DHT_PIN 14
#define DHT_TYPE DHT22

DHT dhtSensor(DHT_PIN, DHT_TYPE);

// connection details
String clientId = "ESP8266-01";
const char* SSID = "IoT-Gateway";
const char* PSK = "smartproducts";
const char* MQTT_BROKER = "10.3.141.1";
const char* mqtt_user = "IoT-Gateway";
const char* mqtt_password = "smartproducts";

WiFiClient espClient;
PubSubClient client(espClient);
long last_sent_ms = millis();

// MQTT message topics
#define status_topic "dht/status"
#define temperature_topic "dht/temperature"
#define humidity_topic "dht/humidity"
#define answer_topic "dht/answer"

void setup_wifi() {
  delay(10);
  Serial.print("Connecting to "); Serial.println(SSID);
  WiFi.begin(SSID, PSK);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected with IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Establishing MQTT connection...");
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("success: connected");
      client.publish(status_topic, "ESP01 alive");
      Serial.println("Subscribing to ");
      Serial.println( answer_topic );
      client.subscribe(answer_topic);
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println("... trying again in 5 seconds");
        delay(5000);
    }
  }
}

void mqtt_callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Nachricht eingetroffen [");
  Serial.print(topic);
  Serial.print("]: ");
  String completeMessage;

  for (int i=0;i<length;i++) {
    completeMessage += (char)message[i];
  }
  Serial.println("Message received: ");
  Serial.println( completeMessage );
  Serial.println();
}


void setup() {
  Serial.begin( 115200 );
  setup_wifi();
  dhtSensor.begin();
  client.setServer( MQTT_BROKER, 1883 );
  client.setCallback(mqtt_callback);  
}


void loop() {
  if ( !client.connected() )
  {
    reconnect();
  }
  client.loop();
    
  long current_ms = millis();

  if ( current_ms - last_sent_ms > 1000 )
  {
    float temperature = dhtSensor.readTemperature();
    float humidity = dhtSensor.readHumidity();

    client.publish( temperature_topic, String(temperature).c_str() );
    client.publish( humidity_topic, String(humidity).c_str() );
    last_sent_ms = current_ms; 
  }
}
