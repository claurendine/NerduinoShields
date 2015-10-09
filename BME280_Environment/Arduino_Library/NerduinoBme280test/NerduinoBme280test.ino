

#include <Wire.h>
#include <Nerduino_BME280_Library.h>

BME280 *bme;
  
void setup() {
  Serial.begin(9600);
  Serial.println("Nerduino BME280 test");

  bme = new BME280(10, 9);
  //bme = new BME280(9, 10);

  if (!bme->isDetected()) 
  {  
    Serial.println("Missing BME280 sensor.");
    while (1);
  }

  bme->configure(2, 2, 2, 3);
  
  
}
  
void loop() {
    Serial.print("Temperature = ");
    Serial.print(bme->readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bme->readPressure());
    Serial.println(" Pa");
    
    Serial.print("Humidity = ");
    Serial.print(bme->readHumidity());
    Serial.println(" %");
    
    Serial.println();
    delay(500);
}
