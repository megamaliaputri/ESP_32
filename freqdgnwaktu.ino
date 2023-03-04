#include <DHT.h>
#define input 33
#define DHTPIN 13
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
float temp = 0;
float hum = 0;
hw_timer_t *Timer0_Cfg = NULL;
bool Measurement_InProgress = false;
uint64_t Measured_Time = 0;
uint64_t Measured_Freq = 0;
 
void IRAM_ATTR Ext_INT1_ISR()
{
    if(Measurement_InProgress == false)
    {
      Measurement_InProgress = true;
      timerWrite(Timer0_Cfg, 0);
      timerStart(Timer0_Cfg);
    }
    else
    {
      Measured_Time = timerRead(Timer0_Cfg);
      timerStop(Timer0_Cfg);
      Measurement_InProgress = false;
      if(Measured_Time == 0)
      {
        Measured_Freq = 0;
      }
      else
      {
        Measured_Freq = (40000000/Measured_Time);
      }
    }
}
 
void setup()
{

    Serial.begin(115200);
    pinMode(input, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(input), Ext_INT1_ISR, RISING);
    Timer0_Cfg = timerBegin(0, 2, true);
    dht.begin();
}
 
void loop()
{
  Serial.println(Measured_Freq);
  sensordht11();
  Measured_Freq = 0;
  delay(500);
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