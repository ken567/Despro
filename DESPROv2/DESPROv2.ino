#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define DEBUG_ON
#define DHTPIN1 4
#define DHTPIN2 5
#define DHTTYPE DHT22
#ifndef HAVE_HWSERIAL1
SoftwareSerial Serial1(10, 11); 
#endif
#ifndef HAVE_HWSERIAL2
SoftwareSerial Serial2(12, 13);
#endif

String date1,date2,time1,hData,tData,ipAdd,latData,longData,mes,mm,hr,mes1,in,id,hr2;  
String mac = "0A-00-00-00-25-73";
char ssid[] = "Dulay56";            
char pass[] = "123456798"; 
char buf1[20];       
int status = WL_IDLE_STATUS;
char mhiServer[] = "mhi.x10host.com";    
const int rs = 53, en = 51, d4 = 49, d5 = 47, d6 = 45, d7 = 43;
float latitude,longitude;
WiFiEspClient client;
TinyGPSPlus gps;
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','=','D'}
};
//pin 1 -> 52
byte rowPins[ROWS] = {38,40,42,44}; 
byte colPins[COLS] = {46,48,50,52}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
 
void setup(){
  
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial.begin(9600);
  dht1.begin();
  dht2.begin();
  lcd.begin(20,4);
  analogWrite(6,350);
  int len = in.length() + 1; 
  wifi4();
}
void loop(){
 char key =  keypad.getKey();
 unsigned long timeElapsed = millis();
 getTime();
 if(WiFi.status() == WL_CONNECTED){
   Serial.println(timeElapsed); 
   getTempHumid();
   displayInfo();
   user(key);
   if(timeElapsed > 1800000){
    transmitData();
    resetCounter();
   } 
 }
 else if(WiFi.status() == WL_DISCONNECTED){
  wifi4();
 }
}
void getTime(){
  String mon,dy,yr;
  while(Serial2.available()>0)
  if (gps.encode(Serial2.read()))
  
  /*if(gps.location.isValid()){
    latData = String(gps.location.lat(), 6);
    longData = String(gps.location.lng(), 6);
  }*/
  if (gps.date.isValid()){
    mon = String(gps.date.month());
    dy = String(gps.date.day());
    yr = String(gps.date.year());
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    //latData = String(gps.location.lat(), 6);
    //longData = String(gps.location.lng(), 6);
    date1 = mon+"/"+dy+"/"+yr;
  }
  if (gps.time.isValid())
  {
    if((gps.time.hour()+8) < 10) {
      hr = "0" + String(gps.time.hour()+8);
      hr2 = "0" + String(gps.time.hour() - 8);
    }
    if((gps.time.hour()+8) > 9){
      hr = String(gps.time.hour()+8);
      hr2 = String(gps.time.hour() - 8);
    }
    if(gps.time.minute() < 10){
      mm = "0" + String(gps.time.minute());
    }
    if(gps.time.minute() > 9){
      mm = String(gps.time.minute());
    }
    time1 = hr+":"+mm;
  }
}
void getTempHumid() {

  float temperature,humidity;
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();
  
  if(isnan(temp1) || isnan(temp2) || isnan(hum1) || isnan(hum2)){ 
    mes = "Check Sensor";
  }else{
    temperature = (temp1 + temp2)/2;
    humidity = (hum1 + hum2)/2;
    
    hData = String(humidity, 2);
    tData = String(temperature, 1);
  
    mes = "RH:" +hData +"% Temp:" + tData+"C";
  }
}
void transmitData(){
  String data = "temp=" + tData + "&humidity=" + hData;
  String data2 = "&long="+longData+"&lat="+latData;
  lcd.clear();
  if(client.connect(mhiServer,80)){
    lcd.setCursor(0,0);
    lcd.print("Transmitting Data");
    Serial.println("Connected to Server");
    client.print("GET /pro/test2.php?");
    client.print(data);
    client.print("&ipadd=");
    client.print(WiFi.localIP());
    client.print(data2);
    client.println(" HTTP/1.1");
    client.println("Host: mhi.x10host.com");
    client.println("Connection: close");
    client.println();
    client.println();
    client.stop();  
    delay(90000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Data Sent");
    lcd.clear();  
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("Unable to connect");
    lcd.clear();
    Serial.println("--> connection failed\n");
  }

}
void displayInfo(){
  Serial.println("RH:" +hData +"Temp:" + tData);

  lcd.setCursor(0,0);
  lcd.print(mes);
  if(Serial2.available()){
    Serial.println(date1+" "+time1);
    Serial.println(mes1);
    Serial.println(latData);
    Serial.println(longData);
    lcd.setCursor(0,1);
    lcd.print(date1+" "+time1);
      latData = String(latitude, 6);
      longData = String(longitude, 6);
      lcd.setCursor(0,2);
      lcd.print(latData);
      lcd.setCursor(0,3);
      lcd.print(longData);
    
}
void wifi4(){
  Serial1.begin(9600);
    WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  
  //WiFi.disconnect();
  // attempt to connect to WiFi network
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  //status = WL_CONNECTED;
  Serial.println("You're connected to the network");
}
void user(char key){
  String pass2 = "*";
   if(key != NO_KEY){
   if(key == 'A'){
       lcd.clear();
       delay(500);
       loop();
     }
     else if(key == 'B'){
       lcd.clear();
       delay(200);
       resetFunc();
     }
     else if(key == 'C'){
       transmitData();
       //resetCounter();
       loop();
     }
   }
}
