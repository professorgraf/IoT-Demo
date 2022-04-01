#include <DHT.h>

#define DHT_PIN 14
#define DHT_TYPE DHT22

DHT dhtSensor(DHT_PIN, DHT_TYPE);
long last_read_ms;

void setup()
{
  Serial.begin( 115200 );
  dhtSensor.begin();
  last_read_ms = millis();
}

void loop()
{
  long current_ms = millis(); 
  float temperature, humidity;
  
  if ( current_ms - last_read_ms > 1000 )
  {
    temperature = dhtSensor.readTemperature();
    humidity = dhtSensor.readHumidity();

    Serial.print( "Temperature: ");
    Serial.print( temperature );
    Serial.println( " °C" );
    Serial.print( "Humidity: ");
    Serial.print( humidity );
    Serial.println( " %" );
    last_read_ms = current_ms;  
  }
}
