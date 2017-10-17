#include "Arduino.h"
#include <Wifi_S08_v2.h>
#include "Primaries.h"

void receive_from_server(String get_data, String path, String server, ESP8266 &wifi){
  wifi.sendRequest(GET, server, 80, path, get_data);
  Serial.println(get_data);
  elapsedMillis time_since_get = 0;
  while (!wifi.hasResponse() && time_since_get < 5000);
  delay(1000); // Give Wifi library some extra "breathing room"
}

Primaries::Primaries(double latt, double lonn, String named) {
  name = named;
  lat = latt;
  lon = lonn;
  server = "iesc-s2.mit.edu";
  date = "";
  hrs = 0;
  mins = 0;
  temp = 0;
  user = "";
  path = "/6S08dev/tramo/final/sb1.py";
  skytext = "";
  get_data = "";
  response = "";
  increment_timer = 0;
}

void Primaries::initPrims(ESP8266 &wifi){
  if (!wifi.isBusy()){
    receive_from_server("?&location="+String(lat)+","+String(lon),path,server, wifi);
  }
  if (wifi.hasResponse()){
    response = String(wifi.getResponse());
    int index1 = response.indexOf(" ",7);
    int index2 = response.indexOf(":",index1);
    int index3 = response.indexOf(":",index2);
    int index4 = response.indexOf("%");
    int index5 = response.indexOf("#");
    int index6 = response.indexOf("$");
    date = response.substring(7,index1);
    hrs = response.substring(18,index3).toInt();
    mins = response.substring(index3+1,index4).toInt();
    temp = response.substring(index5+1,index6).toFloat();
    skytext = response.substring(index4+1,index5);
    increment_timer = 0;
  }
}

void Primaries::incrTime(ESP8266 &wifi){
  mins = (mins + 1) % 60;
  if (mins == 0){hrs = (hrs+1) % 24;}
  if (hrs == 0 && mins == 0){initPrims(wifi);}
}

String Primaries::getTime(){
  if (mins < 10){
    return String(hrs) + ":" + "0" + String(mins);
  }
  return String(hrs) + ":" + String(mins);
}

String Primaries::getWeather(){
  return String(temp);
}

String Primaries::getPrettyDate(){
  int index7 = date.indexOf("-");
  String temp_year = date.substring(0, index7);
  int index8 = index7 + 3;
  String temp_month = date.substring(index7 + 1, index8);
  String temp_day = date.substring(index8 + 1, date.length());
  if (temp_month == "01") {temp_month = "Jan.";}
  else if (temp_month == "02") {temp_month = "Feb.";}
  else if (temp_month == "03") {temp_month = "March";}
  else if (temp_month == "04") {temp_month = "April";}
  else if (temp_month == "05") {temp_month = "May";}
  else if (temp_month == "06") {temp_month = "June";}
  else if (temp_month == "07") {temp_month = "July";}
  else if (temp_month == "08") {temp_month = "Aug.";}
  else if (temp_month == "09") {temp_month = "Sept.";}
  else if (temp_month == "10") {temp_month = "Oct.";}
  else if (temp_month == "11") {temp_month = "Nov.";}
  else if (temp_month == "12") {temp_month = "Dec.";}
  return temp_month + " " + temp_day + ", " + temp_year;
}

String Primaries::getSkytext(){
  return skytext;
}
