#include <Wire.h>

#define SDA_PIN         4
#define SCL_PIN         5
#define STRING_LENGTH   19
#define INT_PIN         2
#define I2C_ADDRESS     0x0A
#define I2C_BAUD_RATE   1000000

char movString[STRING_LENGTH] = "A90 B90 C90 D90 E90";

void setup()
{
  //pinMode(INT_PIN, OUTPUT);
  //digitalWrite(INT_PIN, LOW);
  
  Wire.begin(I2C_ADDRESS);
  Wire.setClock(I2C_BAUD_RATE);       
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendData);
  
  Serial.begin(115200); 
  Serial.println("Programa iniciado");      
}

void loop() 
{
  noInterrupts();
  delay(1000);
  interrupts();
}

void receiveEvent(size_t numBytes)
{   
    (void)numBytes;
    Serial.println("A mensagem recebida foi:");
    while (Wire.available()) {          
      char c = Wire.read();         
      Serial.print(c);                   
    }
    Serial.println("");
    
}

void sendData() {
  
  Wire.write(movString, 19);
  
}
