#ifndef WIFI_COMM_H
#define WIFI_COMM_H

#include <sensors.h>
// Serial Monitors
#define SerialMon Serial

// GSM and httpclient libraries
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>

// Website Credentials
extern const char* ssid;
extern const char* password;
const char server[] = "app.kloudtechsea.com";
extern String deviceSerial;
String resource = "/api/test/weather/insert-data?serial=" + deviceSerial;
const int port = 443;
WiFiClientSecure wifi;
HttpClient client = HttpClient(wifi, server, port);
const int maxRetriesWifi = 20;
int retryCountWifi = 0;
bool connectedWifi = false;

// Time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 28800, 60000);
char d[32], f[32];
String dateTime, fileName;
byte last_second, second_, minute_, hour_, day_, month_;
int year_;

void getTime() {
  timeClient.update();  // Update time from NTP server
  unsigned long unix_epoch = timeClient.getEpochTime();  // Get current epoch time
  // second_ = second(unix_epoch);  // Extract second from epoch time
  // if (last_second != second_)
  // {
  //   minute_ = minute(unix_epoch);  // Extract minute from epoch time
  //   hour_ = hour(unix_epoch);  // Extract hour from epoch time
  //   day_ = day(unix_epoch);  // Extract day from epoch time
  //   month_ = month(unix_epoch);  // Extract month from epoch time
  //   year_ = year(unix_epoch);  // Extract year from epoch time

  //   // Format and print NTP time on Serial monitor
  //   sprintf(d, "%02d-%02d-%02d %02d:%02d:%02d", year_, month_, day_, hour_, minute_, second_);
  //   dateTime = String(d);
  //   sprintf(f, "/%02d%02d%02d.csv", month_, day_, year_);
  //   fileName = String(f);
  // }

  // Convert Unix epoch time to readable format
  struct tm *timeinfo;
  time_t rawtime = unix_epoch;
  timeinfo = localtime(&rawtime);

  if (timeinfo->tm_year >= 70) {  // Avoid 1970 issue
    second_ = timeinfo->tm_sec;
    minute_ = timeinfo->tm_min;
    hour_ = timeinfo->tm_hour;
    day_ = timeinfo->tm_mday;
    month_ = timeinfo->tm_mon + 1;  // Month is 0-11 in struct tm
    year_ = timeinfo->tm_year + 1900; // Year starts from 1900

    sprintf(d, "%04d-%02d-%02d %02d:%02d:%02d", year_, month_, day_, hour_, minute_, second_);
    dateTime = String(d);
    sprintf(f, "/%02d%02d%04d.csv", month_, day_, year_);
    fileName = String(f);

    Serial.print("Updated Time: ");
    Serial.println(dateTime);
  }
}

// SD Card Definitions
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#define SCK 14
#define MISO 2
#define MOSI 15
#define CS 13
SPIClass spi = SPIClass(VSPI);
char data[256];

// SD Card Parameters
void appendFile(fs::FS &fs, String path, String message)
{
  SerialMon.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) { SerialMon.println("Failed to open file for appending"); return; }
  if (file.println(message)) {  SerialMon.println("Message appended"); }
  else { SerialMon.println("Append failed"); }
  file.close();
  SerialMon.println("File Closed");
}

void createHeader(fs::FS &fs, String path, String message)
{
  SerialMon.printf("Checking if %s exists...", path);
  File file = fs.open(path);
  if (!file)
  {
    SerialMon.print("\nFile does not exist creating header files now...");
    File file = fs.open(path, FILE_APPEND);
    if (file.println(message)) { SerialMon.println(" >OK"); }
    else { SerialMon.println(" >Failed"); }
    return;
  }
  SerialMon.println(" >OK");
  file.close();
  SerialMon.println("File Closed");
}

// Saving to SD Card
void logDataToSDCard() {
  SD.end();
  if (!SD.begin(CS, spi)) { SerialMon.println(" >Failed. Skipping SD Storage"); }
  else {
    SerialMon.println("SD Card Initialization Complete");
    getTime();
    SerialMon.println("Datetime: " + dateTime);
    SerialMon.println("Filename:" + fileName);
    sprintf(data, ", %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s", 
            t1Str, h1Str, p1Str, t2Str, h2Str, p2Str, t3Str, h3Str, p3Str, 
            windDirStr, lightStr, uvIntensityStr, rainStr, windSpeedStr, communication);    
    SerialMon.println("Data: " + String(data));
    String log = dateTime + data;

    createHeader(SD, fileName, "Date, Temperature 1, Humidity 1, Pressure 1, Temperature 2, Humidity 2, Pressure 2, Temperature 3, Humidity 3, Pressure 3, Wind Direction, Light Intensity, UV Intensity, Precipitation, Wind Speed, Communication");
    appendFile(SD, fileName, log);

    SerialMon.println("Data logged successfully.");
    SerialMon.println();
  }
}

void startSDCard() {
  SerialMon.println("\n=================================== SD Card Initializing ===================================");
  SerialMon.println("Connecting to SD Card...");
  spi.begin(SCK, MISO, MOSI, CS);
  getTime();
  logDataToSDCard();
}

void printResults() {
  SerialMon.println("T1 = " + t1Str);
  SerialMon.println("T2 = " + t2Str);
  SerialMon.println("T3 = " + t3Str);
  SerialMon.println("H1 = " + h1Str);
  SerialMon.println("H2 = " + h2Str);
  SerialMon.println("H3 = " + h3Str);
  SerialMon.println("P1 = " + p1Str);
  SerialMon.println("P2 = " + p2Str);
  SerialMon.println("P3 = " + p3Str);
  SerialMon.println("Light Intensity = " + lightStr);
  SerialMon.println("UV Intensity = " + uvIntensityStr);
  SerialMon.println("Wind Direction = " + windDirStr);
  SerialMon.println("Wind Speed = " + windSpeedStr);
  SerialMon.println("Rain = " + rainStr);
  SerialMon.println("Gust = " + gustStr);
  SerialMon.println("Battery Voltage = " + batteryStr);
  SerialMon.println("Time: " + dateTime);
  SerialMon.println("Communication Status: " + communication);
}

void connectWiFi() {
  SerialMon.println("\n=================================== Connecting to WiFi ===================================");
  SerialMon.print("Connecting to "); SerialMon.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED && retryCountWifi < maxRetriesWifi) {
    delay(500);
    SerialMon.print(".");
    retryCountWifi++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    communication = "Success";
    connectedWifi = true;
    SerialMon.println("\nWiFi connected!");
    SerialMon.print("IP Address: "); SerialMon.println(WiFi.localIP());
  }
  else {
    communication = "Failed";
    SerialMon.println("\nWiFi not connected!");
  }
}

void sendDataToServer() {
  wifi.setInsecure();
  SerialMon.println("\n=========================================Making POST request============================================");

  String postData = "{\"recordedAt\":\"" + dateTime + "\", \"light\":\"" + lightStr + "\", \"uvIntensity\":\"" + uvIntensityStr + "\", \"windDirection\":\"" + windDirStr + "\", \"windSpeed\":\"" + windSpeedStr + "\", \"precipitation\":\"" + rainStr + "\", \"gust\":\"" + gustStr + "\", \"T1\":\"" + t1Str + "\", \"T2\":\"" + t2Str + "\", \"T3\":\"" + t3Str + "\", \"H1\":\"" + h1Str + "\", \"H2\":\"" + h2Str + "\", \"H3\":\"" + h3Str + "\", \"P1\":\"" + p1Str + "\", \"P2\":\"" + p2Str + "\", \"P3\":\"" + p3Str + "\", \"batteryVoltage\":\"" + batteryStr + "\"}";
  SerialMon.println(postData);

  client.beginRequest();
  client.post(resource);
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", postData.length());
  client.print(postData);
  client.endRequest();

  int status_code = client.responseStatusCode();
  String response = client.responseBody();
  SerialMon.printf("Status code: %d\n", status_code);
  SerialMon.println("Response: " + response);
}

#endif