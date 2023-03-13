#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "IT_Kwantum";
const char* password = "06081407";

//имя домена
String serverName = "http://192.168.0.3:8000/create";

unsigned long lastTime = 0;

unsigned long timerDelay = 5000;

// Напряжение батареи при 100% заряда
const float BATTERY_VOLTEGE = 1.5;

// Напряжение батареи при 100% заряда
const float BATTERY_VOLTEGE_ZERO = 1.2;

//Процент заряда
int percentOfCharge = 0;
// напряжение
float voltage = 0;

// время начало зарядки
int start_time = 0;  
// время длительность зарядки
int charge_time = 0;

// начальный статус зарядки 2 - свободно
int ChargeStatus = 2;

//1- занята 2- свободна 3- на ТО 4- закрыта
boolean newChargeRedy = true;

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  
  uint32_t sec = millis() / 1000ul;      // полное количество секунд
  int timeHours = (sec / 3600ul);        // часы
  int timeMins = (sec % 3600ul) / 60ul;  // минуты
  int timeSecs = (sec % 3600ul) % 60ul;  // секунды



int tempVoltage = analogRead(A0);

// расчет напряжения
//float voltage = tempVoltage/20.48;
float voltage = (float)(3.0*tempVoltage)/1024;
//Serial.println(tempVoltage);
//Serial.println(voltage);

// расчет проценнта заряда 0% < 1.2 вольт 
// 0,1 вольт = 33,3 %; 0,01 вольт = 3,3 %
float ChargePercent = (BATTERY_VOLTEGE - voltage) * 10 * 33;
// расчет сколько осталось до конца зарядки. 0 до 100% батарея заряжается за 10 часов
float endTime = (ChargePercent/100)*10;

// если напряжение более 0,4 вольт считать началом зарядки
if (voltage > 0.4 && newChargeRedy) {
  start_time = timeMins*1.0;  
  ChargeStatus = 1;
  newChargeRedy = false;
  Serial.println("В больше 04  реди тру");
}
else if (voltage > 0.4 && !newChargeRedy) {
  charge_time = timeMins-start_time; 
  ChargeStatus = 1;
     Serial.println("В больше 04  реди фелз");
}
// если напряжение упало снова до 0  конец зарядки
// если сменилось название авто считать конец зарядки ||
if (voltage <= 0.4 ) {
  charge_time = 0; 
  ChargeStatus = 2;
  newChargeRedy = true;
   Serial.println("В меньше либо раверн 04  реди тру");
}
  // отправка запроса
  if ((millis() - lastTime) > timerDelay) {
    //проверка WI FI соеденения
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      String serverPath = serverName + "?id_charge=1&title=Заправка%20возле%20кванториума&task=Заправка%20возле%20кванториума%20на%20одно%20парковочное%20место&slot_number=1&car_title=tesla&battery_voltage=" + (String)voltage+"&start_charge_time=" + (String)charge_time + "&status="+(String)ChargeStatus+"&charge_percent="+(String)ChargePercent+"&end_charge_time="+(String)endTime;
       Serial.println(serverPath);  
      http.begin(client, serverPath.c_str());
  
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  //end http
delay(10000);

}
