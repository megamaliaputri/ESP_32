#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>


float temp = 0;
float hum = 0;
unsigned long pulseCount;
unsigned long pulse1Sec = 0;
// Timer variables
long currentMillis = 0;
long previousMillis = 0;
long lastTime = 0;
int interval = 1000;
char freq[ 16 ];

#define input 12
#define DHTPIN 13
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

bool deviceConnected = false;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"
// Temperature Characteristic
BLECharacteristic pTemperatureCelsiusCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
//BLEDescriptor pTemperatureCelsiusDescriptor(BLEUUID((uint16_t)0x2902));
// Humidity Characteristic
BLECharacteristic pHumidityCharacteristics("ca73b3ba-39f6-4ab3-91ae-186dc9577d99", BLECharacteristic::PROPERTY_NOTIFY);
//BLEDescriptor pHumidityDescriptor(BLEUUID((uint16_t)0x2903));
// frequency Characteristic
BLECharacteristic pFrequencyCharacteristics("a2acf095-5bba-4b51-a773-e0bdcd9faece", BLECharacteristic::PROPERTY_NOTIFY);
//BLEDescriptor pFrequencyCelsiusDescriptor(BLEUUID((uint16_t)0x2902));
//Setup callbacks onConnect and onDisconnect

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};
void setup() {
  // Start serial communication 
  Serial.begin(115200);
  pinMode(input, INPUT_PULLUP);
  BLEDevice::init("ESP32_DHT11");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  // Create BLE Characteristics and Create a BLE Descriptor
  // Temperature
  pService->addCharacteristic(&pTemperatureCelsiusCharacteristics);
  pTemperatureCelsiusCharacteristics.addDescriptor(new BLE2902());
  // Humidity
  pService->addCharacteristic(&pHumidityCharacteristics);
  pHumidityCharacteristics.addDescriptor(new BLE2902());
  // Frequency
  pService->addCharacteristic(&pFrequencyCharacteristics);
  pFrequencyCharacteristics.addDescriptor(new BLE2902());
  // Start the service
  pService->start();
  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pServer->getAdvertising()->start();
  pulseCount = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(input), pulseCounter, RISING);
  Serial.println("Waiting a client connection to notify...");
  dht.begin();
}

void loop() {
  currentMillis = millis();
  if((currentMillis - previousMillis) == interval){
    pulse1Sec = pulseCount;
    sensordht11();
    Serial.println(pulse1Sec);
    sprintf(freq, "%lu", pulse1Sec);
    if (deviceConnected) {
    kirimdata();
    }
    pulseCount = 0;
    previousMillis = millis();
    }
}

void sensordht11(){
  temp = dht.readTemperature();
  hum = dht.readHumidity();
}

void kirimdata(){
  pTemperatureCelsiusCharacteristics.setValue(temp);
  pTemperatureCelsiusCharacteristics.notify();
  Serial.print("Temperature Celsius: ");
  Serial.print(temp);
  Serial.print(" C");
  pHumidityCharacteristics.setValue(hum);
  pHumidityCharacteristics.notify();   
  Serial.print(" - Humidity: ");
  Serial.print(hum);
  Serial.println(" %");
  pFrequencyCharacteristics.setValue(freq);
  pFrequencyCharacteristics.notify();
  Serial.print("frequency: ");
  Serial.println(freq);
      
}

