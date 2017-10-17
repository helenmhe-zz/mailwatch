#ifndef Primaries_h
#define Primaries_h
#include "Arduino.h"
#include <Wifi_S08_v2.h>

void receive_from_server(String get_data, String path, String server);

class Primaries {
public:
  String date;
  String name;
  int hrs;
  int mins;
  double lat;
  double lon;
  double temp;
  String user;  
  String server;
  String path;
  String skytext;
  String get_data;
  String response;
  public:
      Primaries(double latt, double lonn, String name);
      void initPrims(ESP8266 &wifi);
      void incrTime(ESP8266 &wifi);
      String getTime();
      String getWeather();
      String getSkytext();
      String getPrettyDate();
      elapsedMillis increment_timer;
};

#endif
