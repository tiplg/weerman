#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <DHT.h>
#include <string>
#include "../include/credentials.h"

using namespace std;

const char *ssid = STASSID;
const char *password = STAPSK;

const char *softAPName = "De Weerman";

const char *OTAauth = "6bd5e09202c765c4f0cee992644082d9";

//fucntion prototypes
void handleRoot();
void handleStyles();
void startWifiAP();
void startDNSServer();
void startWebsockets();
void startHTTPServer();
void startOTAServer();
void startWifiStation();
const char *get_filename_ext(const char *filename);

const byte DNS_PORT = 53;
const IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer httpServer(80);
WebSocketsServer webSocketServer(81);

unsigned long currentMillis = 0; // used for timing
unsigned long sendSomethingMillis = 0;

DHT dht;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(500000);
  Serial.println("Booting");

  SPIFFS.begin();

  pinMode(2, OUTPUT);

  WiFi.mode(WIFI_AP_STA);

  startWifiAP();
  startDNSServer();
  startWebsockets();
  startHTTPServer();
  startOTAServer();
  startWifiStation();

  dht.setup(13);

  currentMillis = millis();
  sendSomethingMillis = currentMillis;
}

int windrichting = 0;
int windsnelheid = 0;

void loop()
{
  currentMillis = millis();

  if (currentMillis - sendSomethingMillis > 1 * 1000) //set delay , DHT minimum = 2000ms
  {
    if (windrichting++ >= 360)
      windrichting = 0;
    if (windsnelheid++ >= 100)
      windsnelheid = 0;

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%i,%i,%.1f,%.1f", windsnelheid, windrichting, dht.getTemperature(), dht.getHumidity());

    webSocketServer.broadcastTXT(buffer);

    sendSomethingMillis = currentMillis;
  }

  ArduinoOTA.handle();
  dnsServer.processNextRequest();
  httpServer.handleClient();
  webSocketServer.loop();
}

void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r");
  httpServer.streamFile(file, "text/html");
  file.close();
}

void handleScripts()
{
  File file = SPIFFS.open("/scripts.js", "r");
  httpServer.streamFile(file, "text/javascript");
  file.close();
}

void handleStyles()
{
  File file = SPIFFS.open("/style.css", "r");
  httpServer.streamFile(file, "text/css");
  file.close();
}

void handleIcon()
{
  File file = SPIFFS.open("/favicon.ico", "r");
  httpServer.streamFile(file, "image/x-icon");
  file.close();
}

void handleIcons()
{
  File file = SPIFFS.open("/icons.woff", "r");
  httpServer.streamFile(file, "font/woff");
  file.close();
}

void startHTTPServer()
{
  httpServer.on("/", handleRoot);
  httpServer.on("/scripts.js", handleScripts);
  httpServer.on("/style.css", handleStyles);
  httpServer.on("/icons.woff", handleIcons);
  httpServer.on("/favicon.ico", handleIcon);
  httpServer.onNotFound(handleRoot);
  httpServer.begin();
  Serial.println("HTTP server started");
}

void startWifiAP()
{
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  bool result = WiFi.softAP(softAPName);
  Serial.print("SoftAP ");
  Serial.print(result ? "Connected" : "Failed!");
  Serial.print("   IP:");
  Serial.println(WiFi.softAPIP());
}

void startDNSServer()
{
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  bool result = dnsServer.start(DNS_PORT, "*", apIP);
  Serial.print("DNS server ");
  Serial.println(result ? "Connected" : "Failed!");
}

void startWebsockets()
{
  webSocketServer.begin();
  Serial.println("Websocket server started");
}

void startOTAServer()
{
  ArduinoOTA.setPasswordHash(OTAauth);

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
  });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void startWifiStation()
{
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Wifi connection Failed! (station)");
  }
  Serial.print("Station IP: ");
  Serial.println(WiFi.localIP());
}

const char *get_filename_ext(const char *filename)
{
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}