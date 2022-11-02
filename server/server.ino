#include <WiFi.h>
#include <WebServer.h>
#include "PMS.h"
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
 
PMS pms(Serial2);
PMS::DATA data;
 
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET    -1  // Reset pin # (or -1 if sharing reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
String val1;
String val2;
String val3;
 
int relayPin = 23;
int buzzerPin = 15;
 
const char* ssid = "Alexahome";
const char* password = "loranthus";
 
WebServer server(80);
 
void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
 
  pinMode(relayPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
 
  digitalWrite(relayPin, LOW);
  digitalWrite(buzzerPin, LOW);
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(100);
  display.clearDisplay();
 
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 20);
  display.print("Initializing....");
  display.display();
  delay(3000);
 
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
 
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());
 
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.print("WiFi connected..!");
  display.setTextSize(1);
  display.setCursor(20, 40);
  display.print(WiFi.localIP());
  display.display();
  delay(4000);
 
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
 
  server.begin();
  Serial.println("HTTP server started");
  delay(2000);
}
 
void loop()
{
  if (pms.read(data))
  {
    val1 = data.PM_AE_UG_1_0;
    val2 = data.PM_AE_UG_2_5;
    val3 = data.PM_AE_UG_10_0;
 
    Serial.println("Air Quality Monitor");
 
    Serial.println("PM1.0 :" + val1 + "(ug/m3)");
 
    Serial.println("PM2.5 :" + val2 + "(ug/m3)");
 
    Serial.println("PM10  :" + val3 + "(ug/m3)");
 
    Serial.println("");
 
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(10, 10);
    display.print("PM1.0: ");
    display.print(val1);
    display.print(" ug/m3");
 
    display.setCursor(10, 30);
    display.print("PM2.5: ");
    display.print(val2);
    display.print(" ug/m3");
 
    display.setCursor(10, 50);
    display.print("PM10 : ");
    display.print(val3);
    display.print(" ug/m3");
 
    display.display();
 
    delay(2000);
  }
  if (val2.toFloat() >= 150)
  {
    digitalWrite(relayPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
  }
  else
  {
    digitalWrite(relayPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }
  server.handleClient();
}
void handle_OnConnect()
{
  server.send(200, "text/html", SendHTML(val1.toFloat(), val2.toFloat(), val3.toFloat()));
}
 
 
void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}
 
String SendHTML(int val1, int val2, int val3)
{
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Wireless Weather Station</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "<script>\n";
  ptr += "setInterval(loadDoc,1000);\n";
  ptr += "function loadDoc() {\n";
  ptr += "var xhttp = new XMLHttpRequest();\n";
  ptr += "xhttp.onreadystatechange = function() {\n";
  ptr += "if (this.readyState == 4 && this.status == 200) {\n";
  ptr += "document.body.innerHTML =this.responseText}\n";
  ptr += "};\n";
  ptr += "xhttp.open(\"GET\", \"/\", true);\n";
  ptr += "xhttp.send();\n";
  ptr += "}\n";
  ptr += "</script>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Particulate Matter Monitoring</h1>\n";
 
  ptr += "<p>PM1.0: ";
  ptr += val1;
  ptr += " ug/m3</p>";
 
  ptr += "<p>PM2.5: ";
  ptr += val2;
  ptr += " ug/m3</p>";
 
  ptr += "<p>PM10: ";
  ptr += val3;
  ptr += " ug/m3</p>";
 
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}