#include "DHT.h"

#define DHTPIN 3
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

boolean started;
int sampling_interval;
long previous_millis; 

char buffer[5];
int rx_position;

void setup() {
  
  Serial.begin(9600); 
  dht.begin();
  
  started = false;
  sampling_interval = 10000;
  previous_millis = 0; 
  rx_position = 0;
}

void loop() {

  if (Serial.available() > 0) {
    buffer[rx_position] = Serial.read();
    rx_position++;

    // I received 4 characters: the command is complete
    if(rx_position == 4) {
      
      // add string terminator
      buffer[rx_position] = '\0';
           
      // parse START/STOP commands
      if(strcmp(buffer, "STAR") == 0) {
        started = true;
        Serial.println("Sampling started");
      }
      else if(strcmp(buffer, "STOP") == 0) {
        Serial.println("Sampling stopped");
        started = false;
      }
      
      // search for new sampling value (S=)
      else if(strstr(buffer, "S=") != NULL) {
        int new_sampling_interval = atoi(buffer + 2);
        sampling_interval = new_sampling_interval * 1000;        
        Serial.print("Sampling values every ");
        Serial.print(new_sampling_interval);
        Serial.println(" seconds");
      }
      
      else {
        Serial.print("Unknown command ");
        Serial.println(buffer);
      }
      
      // reset buffer
      rx_position = 0;      
    }
  }
  
  if(started && millis() > previous_millis + sampling_interval) {
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
  
    Serial.print("{TIMEPLOT:Room|data|temperature|T|");
    Serial.print(t);
    Serial.println("}");
    Serial.print("{TIMEPLOT:Room|data|humidity|T|");
    Serial.print(h);
    Serial.println("}"); 
   
    previous_millis = millis(); 
  }
}
