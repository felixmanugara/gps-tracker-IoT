// inisiasi pin pada ESP32
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26
#define LED                  13
#define RELAY                14
#define BLYNK_PRINT Serial    
#define TINY_GSM_MODEM_SIM800

#include <TinyGPS++.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleSIM800.h>
// set serial monitor untuk modul gsm dan gps
#define SerialMon Serial
// komunikasi serial untuk modul gsm
#define SerialAT Serial1
// variabel untuk menyimpan data GPS
double latitude;
double longitude;
int satellite;
// use this APN settings if you using Indosat
const char apn[]  = "indosatgprs";
const char user[] = "indosat";
const char pass[] = "indosatgprs";
// ini adalah token autentifikasi untuk aplikasi blynk
const char auth[] = "tTRE5J-DSIWucdJyRYgqvQhEolfhHbRf";
//
bool s1;

BlynkTimer timer;

// membuat instance modem untuk modul GSM
TinyGsm modem(SerialAT);

// membuat instance untuk modul GPS
TinyGPSPlus gps;
WidgetMap myMap(V0);

// this function print memory address of given varible value
void GPSData_mem_addr(double *latitude, double *longitude) 
{
  *latitude;
  *longitude;
  Serial.print("latitude mem address: ");
  Serial.println((unsigned int)latitude,HEX);
  Serial.print("longitude mem address: ");
  Serial.println((unsigned int)longitude,HEX);
}

void GPSData() 
{
    while (Serial.available() > 0)
    {
      if (gps.encode(Serial.read()))
        if (gps.location.isValid()) 
        {
          unsigned int index = 1;
          latitude = gps.location.lat();
          longitude = gps.location.lng();
          satellite = gps.satellites.value();
          String j = "jumlah satelit saat ini: ";

       /* Serial.print(latitude,6);
          Serial.print(", ");
          Serial.print(longitude,6);
          Serial.print(", ");
          Serial.print(satellite);
          Serial.print(", ");
          Serial.print(speeds);
          Serial.print(", ");
          Serial.print(alt);
          Serial.print(", ");
          Serial.print(days);
          Serial.print("/");
          Serial.print(months);
          Serial.print("/");
          Serial.println(years);
        */  
          myMap.location(index, latitude, longitude, "Lokasi Terkini");
          // return memory address of given variable
          GPSData_mem_addr(&latitude,&longitude);
        }
    }
}

void GPSCheck()
{
  if ((gps.charsProcessed() < 10) || (!gps.location.isValid()))
  {
    Serial.println("GPS tidak terdeteksi");
    Blynk.virtualWrite(V1, "Gagal mendeteksi lokasi!!");
    Blynk.virtualWrite(V2, "mencari titik lokasi, Mohon tunggu");
    Blynk.notify("jika titik lokasi tidak kunjung ditemukan, cek perkabelan bila perlu");
  }
  return;
}

BLYNK_WRITE(V3)
{
  int pinValue = param.asInt();
  if (pinValue == 1) {
    digitalWrite(RELAY, LOW);
    s1 = true;
  } else {
    digitalWrite(RELAY, HIGH);
    s1 = false;
  }
}

void setup()
{
  // set baud rate untuk serial monitor
  Serial.begin(9600);
  delay(10);

  // Set-up modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  digitalWrite(RELAY, HIGH);

  // pengaturan baudrate untuk modul GSM dan komunikasi UART
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  // ini digunakan untuk restart modul GSM
  SerialMon.println("Inisialsasi GSM modul..");
  modem.restart();

  String modulInfo = modem.getModemInfo();
  SerialMon.print("Modem: ");
  SerialMon.println(modulInfo);

  SerialMon.print("Menunggu jaringan...");
   
  if (!modem.waitForNetwork(240000L)) 
  {
    SerialMon.println(" gagal");
    delay(10000);
    return;
  }
  
  SerialMon.println(" berhasil");

  if (modem.isNetworkConnected()) 
  {
    SerialMon.println("Jaringan terhubung");
  } 
 
  SerialMon.print("Menghubungkan ke APN: ");
  SerialMon.print(apn);
  
  if (!modem.gprsConnect(apn, user, pass)) 
  {
    SerialMon.println(" koneksi ke GPRS gagal");
    delay(10000);
    return;
  }
  SerialMon.println(" berhasil");

  Blynk.begin(auth, modem, apn, user, pass);

  if (Blynk.connected())
  {
    digitalWrite(LED, HIGH);
    delay(2000);
    digitalWrite(LED, LOW);
  }
  
  timer.setInterval(2000L, GPSData);
  timer.setInterval(50000L, GPSCheck);
}
 

void loop()
{
  
  Blynk.run();
  timer.run(); 
  
}



 



  
