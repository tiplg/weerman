#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <DHT.h>
#include <Anemo.h>
#include "credentials.h"

using namespace std;

int internalLed = 2;
int externalLed = 15;

const char *ssid = STASSID;
const char *password = STAPSK;

const char *softAPName = "De Weerman";

const char *OTAauth = OTAAUTH;

//fucntion prototypes
void startSoftAP();
void startDNSServer();
void startWebsockets();
void startHTTPServer();
void startOTAServer();
void startWifiStation();
void startLog();

const byte DNS_PORT = 53;
const IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
AsyncWebServer httpServer(80);
WebSocketsServer webSocketServer(81);

WiFiEventHandler stationConnectedHandler;
WiFiEventHandler stationDisconnectedHandler;

unsigned long currentMillis = 0; // used for timing
unsigned long sendSomethingMillis = 0;

DHT dht;
Windvaan windvaan;
Anemometer anemometer;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(500000);
  Serial.println("Booting...");

  pinMode(externalLed, OUTPUT);
  pinMode(internalLed, OUTPUT);

  digitalWrite(externalLed, HIGH);
  digitalWrite(internalLed, LOW);

  SPIFFS.begin();

  WiFi.mode(WIFI_AP_STA);

  startSoftAP();
  startDNSServer();
  startWebsockets();
  startHTTPServer();
  startOTAServer();
  startLog();

  //TEST remove in release maybe
  startWifiStation();

  dht.setup(12);
  windvaan.Setup(A0, 807, 0, 100);
  anemometer.Setup(13, 60000000);

  currentMillis = millis();
  sendSomethingMillis = currentMillis;

  digitalWrite(internalLed, HIGH);
  Serial.println("Running...");
}

void loop()
{
  currentMillis = millis();

  if (currentMillis - sendSomethingMillis > 1 * 500) //set delay , DHT minimum = 2000ms
  {
    digitalWrite(externalLed, !digitalRead(externalLed));
    // digitalWrite(internalLed, !digitalRead(internalLed));

    //windvaan.getRichting(); // TODO debug

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%.0f,%.0f,%.1f,%.1f", anemometer.getSnelheid(), windvaan.getRichting(), isnan(dht.getTemperature()) ? (float)0 : dht.getTemperature(), isnan(dht.getHumidity()) ? 0 : dht.getHumidity());

    webSocketServer.broadcastTXT(buffer);

    sendSomethingMillis = currentMillis;
  }

  anemometer.Handle();

  ArduinoOTA.handle();
  dnsServer.processNextRequest();
  webSocketServer.loop();
}

String processor(const String &var)
{
  if (var == "DATA_TEMPLATE")
  {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%.3f,%.0f,%.1f,%.1f", anemometer.getSnelheid(), windvaan.getRichting(), isnan(dht.getTemperature()) ? (float)0 : dht.getTemperature(), isnan(dht.getHumidity()) ? 0 : dht.getHumidity());
    return buffer;
  }
  return String();
}

void handleNotFound(AsyncWebServerRequest *request)
{
  request->redirect("/");
}

void startHTTPServer()
{
  httpServer.serveStatic("/admin", SPIFFS, "/admin/log.txt").setAuthentication(ADMINUSER, ADMINPSK);
  httpServer.serveStatic("/scripts.js", SPIFFS, "/www/scripts.js").setTemplateProcessor(processor);
  httpServer.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html").setCacheControl("max-age=600"); //.setCacheControl("max-age=600");
  httpServer.onNotFound(handleNotFound);
  httpServer.begin();
  Serial.println("HTTP server started");
}

void logSoftAPConnect(WiFiEventSoftAPModeStationConnected event)
{

  File file = SPIFFS.open("/admin/log.txt", "a+");
  if (!file)
  {
    Serial.println("failed to open log.txt");
    return;
  }

  Serial.printf("CONNECTED\t%02x:%02x:%02x:%02x:%02x:%02x %i\n", event.mac[0], event.mac[1], event.mac[2], event.mac[3], event.mac[4], event.mac[5], event.aid);
  file.printf("CONNECTED\t%02x:%02x:%02x:%02x:%02x:%02x %i\n", event.mac[0], event.mac[1], event.mac[2], event.mac[3], event.mac[4], event.mac[5], event.aid);

  file.close();
}

void logSoftAPDisconnect(WiFiEventSoftAPModeStationDisconnected event)
{
  File file = SPIFFS.open("/admin/log.txt", "a+");
  if (!file)
  {
    Serial.println("failed to open log.txt");
    return;
  }

  Serial.printf("DISCONNECTED\t%02x:%02x:%02x:%02x:%02x:%02x %i\n", event.mac[0], event.mac[1], event.mac[2], event.mac[3], event.mac[4], event.mac[5], event.aid);
  file.printf("DISCONNECTED\t%02x:%02x:%02x:%02x:%02x:%02x %i\n", event.mac[0], event.mac[1], event.mac[2], event.mac[3], event.mac[4], event.mac[5], event.aid);

  file.close();
}

void startSoftAP()
{
  stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&logSoftAPConnect);
  stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&logSoftAPDisconnect);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  bool result = WiFi.softAP(softAPName, "", 1, 0, 8);
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
  //ArduinoOTA.setPasswordHash(OTAauth);

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
  });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

void startWifiStation()
{
  WiFi.begin(ssid, password);

  //FIXME continue if connection failed
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Wifi connection Failed! (station)");

    WiFi.mode(WIFI_AP);
  }
  else
  {
    Serial.printf("Station IP: %s\n", WiFi.localIP().toString().c_str());

    File file = SPIFFS.open("/admin/log.txt", "a+");
    if (!file)
    {
      Serial.println("failed to open log.txt");
      return;
    }

    file.printf("Station IP: %s\n", WiFi.localIP().toString().c_str());

    file.close();
  }
}

void startLog()
{
  File file = SPIFFS.open("/admin/log.txt", "a+");
  if (!file)
  {
    Serial.println("failed to open log.txt");
    return;
  }

  file.printf("---------------------REBOOT---------------------\n");

  file.close();
}