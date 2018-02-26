#include <LiquidCrystal.h>
#include <DHT.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <WiFiEspClient.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define DEBUG_ON
#define DHTPIN1 4
#define DHTPIN2 5
#define DHTTYPE DHT22
#ifndef HAVE_HWSERIAL1
SoftwareSerial Serial1(10,11);
#endif
#ifndef HAVE_HWSERIAL2
SoftwareSerial Serial2(12,13);
#endif
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
//float humidity,temperature;
//String date1,time1;  
IPAddress ip(192,91,81,4);
char ssid[] = "Dulay55";            
char pass[] = "confidential";        
int status = WL_IDLE_STATUS;
const int timeZone = -8;    
unsigned int localPort = 2390;      
char timeServer[] = "0.asia.pool.ntp.org";
char mhiServer[] = "www.mhi.x10host.com"; 
const int NTP_PACKET_SIZE = 48;  
const int UDP_TIMEOUT = 2000;    
byte packetBuffer[NTP_PACKET_SIZE]; 

WiFiEspClient client;
WiFiEspUDP Udp;

void setup(){

  Serial.begin(9600);
  Serial1.begin(9600);
  dht1.begin();
  dht2.begin();
  WiFi.init(&Serial1);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
  Serial.println("You're connected to the network");
  Udp.begin(localPort);
   
}
void loop(){
  //getTime();
  transmitData();
  //getTempHumid();
  //delay(10000);
}
void getTime(){
 sendNTPpacket(timeServer); 
  unsigned long startMs = millis();
  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}
  if (Udp.parsePacket()) {
    Serial.println("packet received");
    Udp.read(packetBuffer, NTP_PACKET_SIZE);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    // epoch -> UTC Timestamp
    unsigned long epoch = secsSince1900 - seventyYears;
    epoch=epoch+28800; //ADD 8 Hours (For GMT+8)
    Serial.print("Date: ");
    Serial.print(month(epoch));
    Serial.print("/");
    Serial.print(day(epoch)) ;
    Serial.print("/");
    Serial.println(year(epoch));
    Serial.print("Time: ");
    Serial.print(hourFormat12(epoch));
    Serial.print(":");
    //Serial.print(minute(epoch));
    if(minute(epoch)<=9){
     Serial.print("0");
     Serial.print(minute(epoch));
   }else{
    Serial.print(minute(epoch));
   }
   if(hour(epoch)<=11){
     Serial.println(" AM");
   }else{
     Serial.println(" PM");
   }
  Serial.println("");

  }

  delay(5000);
}

/*void getTempHumid() {

  humidity1 = dht1.readHumidity();
  temperature1 = dht1.readTemperature();
  humidity2 = dht2.readHumidity();
  temperature2 = dht2.readTemperature();
  humidity = dht1.readHumidity();
  temperature = dht1.readTemperature();
  String hData = String(humidity, 2);
  String tData = String(temperature, 2);

  Serial.println("R.H: " + hData + "%  Temp: " + tData+"*C");
  //lcd.println("R.H: " + hData + "%  Temp: " + tData+"*C");
}*/
void transmitData(){
  String temp = "50"; //For Testing
  String humidity = "50"; //For Testing
  if (client.connect(mhiServer, 80)) {
    Serial.println("Connected to server");
    client.print("GET /test.php?");
    client.print("temp=");
    client.print(temp);
    client.print("&");
    client.print("humidity=");
    client.print(humidity);
    client.println(" HTTP/1.1");
    client.println("Host: www.mhi.x10host.com");
    //client.println( "Connection: close" );
    Serial.println(client.read());
    client.stop();
    
  }
}


