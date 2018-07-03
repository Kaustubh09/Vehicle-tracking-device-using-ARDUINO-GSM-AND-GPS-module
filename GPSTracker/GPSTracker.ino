#include <TinyGPS++.h>
#include <gprs.h>
#include <SoftwareSerial.h>

#define SOFT_RX 11
#define SOFT_TX 12
#define SOFT_BAUD 9600

#define GPS_RX 9
#define GPS_TX 10

#define MOBILE "9790544908"

GPRS gprs(SOFT_RX, SOFT_TX);
TinyGPSPlus gps;

void setup() {
  Serial.begin(SOFT_BAUD);
  Serial.print("GPS Vechile Tracker\r\n");
  gprs.preInit(SOFT_BAUD);
  
  while(0 != gprs.init()) {
      delay(1000);
      Serial.println("GSM ERROR");
  }

  Serial.println(F("Latitude   Longitude   Date       Time     Speed"));
  Serial.println(F("(deg)      (deg)                           (KMH)"));
  Serial.println(F("------------------------------------------------"));
  gprs.deleteAllSMS();
}

char *ftoa(char *a, double f, int precision)
{
 long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};
 
 char *ret = a;
 long heiltal = (long)f;
 itoa(heiltal, a, 10);
 while (*a != '\0') a++;
 *a++ = '.';
 long desimal = abs((long)((f - heiltal) * p[precision]));
 itoa(desimal, a, 10);
 return ret;
}

void loop() {
char lat[12];
char lng[13];
char buff[50];

  if(gprs.readSMS(1, buff, 50)){
    Serial.println(buff);
    if(!strcmp("LOCATION", buff)){
      ftoa(lat, gps.location.lat(), 6);
      ftoa(lng, gps.location.lng(), 6);
      strcpy(buff, lat);
      strcat(buff,":");
      strcat(buff,lng);
      gprs.sendSMS(MOBILE, buff);
    }
    gprs.deleteSMS(1);
  }
  
  printFloat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printFloat(gps.location.lng(), gps.location.isValid(), 12, 6);
  printDateTime(gps.date, gps.time);
  printFloat(gps.speed.kmph(), gps.speed.isValid(), 6, 2);

  Serial.println();
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do{
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec){
  if (!valid){
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else{
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len){
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t){
  if (!d.isValid()){
    Serial.print(F("********** "));
  }
  else{
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d ", d.month(), d.day(), d.year());
    Serial.print(sz);
  }
  
  if (!t.isValid()){
    Serial.print(F("******** "));
  }
  else{
    char sz[32];
    sprintf(sz, "%02d:%02d:%02d ", t.hour(), t.minute(), t.second());
    Serial.print(sz);
  }

  //printInt(d.age(), d.isValid(), 5);
  smartDelay(0);
}

static void printStr(const char *str, int len){
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartDelay(0);
}
