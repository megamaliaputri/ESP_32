#include <DHT.h>
#define input 12
#define DHTPIN 13
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
float temp = 0;
float hum = 0;
volatile uint32_t pulseCount = 0 ;
//volatile byte pulseCount;
volatile uint32_t pulse1Sec = 0;
//byte pulse1Sec = 0;
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  pinMode(input, INPUT_PULLUP);
  dht.begin();
  previousMillis = 0;
  pulseCount = 0;
  attachInterrupt(digitalPinToInterrupt(input), pulseCounter, RISING);
}

void loop() {
  currentMillis = millis();
  if((currentMillis - previousMillis) == interval){
    pulse1Sec = pulseCount;
    pulseCount = 0;
    previousMillis = millis();
    frequency();
    }
}

void frequency(){
  Serial.print("Frequency: ");
  Serial.println(pulse1Sec);
  sensordht11();
}

void sensordht11(){
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" C");
  Serial.print(" - Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
}
