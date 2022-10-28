/*StAuth10222: I Michael Mena, 000817498 certify that this material is my original work.
No other person's work has been used without due acknowledgement. I have not made my work available to anyone else.*/
#include <Arduino.h>
#include <ESP8266WiFi.h>

// 1-Wire sensor communication libary
#include <OneWire.h>
// DS18B20 sensor library
#include <DallasTemperature.h>

// Pin that the  DS18B20 is connected to
const int oneWireBus = D3;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature DS18B20(&oneWire);

DeviceAddress address;

boolean sensorFound = false;
// access credentials for WiFi network.
const char *ssid = "Mohawk-IoT";
const char *password = "IoT@MohawK1";
// WiFI server. Listen on port 80, which is the default TCP port for HTTP
WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  Serial.println("\nWeb Server Demo");
  Serial.printf("\nConnecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n",
                WiFi.localIP().toString().c_str());

  if (DS18B20.getAddress(address, 0))
  {

    sensorFound = true;

    for (uint8_t i = 0; i < 8; i++)
    {
      Serial.print(address[i], HEX);
    }

    DS18B20.begin();
  }
  else
  {
    Serial.println("Device sensor not found");
  }
}

void loop()
{
  WiFiClient client = server.available();

  // wait for a client (web browser) to connect
  if (client)
  {
    Serial.println("\n>> Client connected");
    Serial.println(">> Client request:");
    client.println("<table border='10' style='background-color: aqua; border-color: red blue gold teal'><tr><th>Temperature</th><th>It feels</th></tr>");
    while (client.connected())
    {

      // read line by line what the client (web browser) is requesting
      if (client.available())
      {

        float answer;

        for (int i = 0; i < 10; i++)
        {
          // ask DS18B20 for the current temperature
          DS18B20.requestTemperatures();

          answer = DS18B20.getTempCByIndex(0);

          String feels = "";

          if (answer < 10)
          {
            feels += "Too cold!";
          }
          else if (answer < 15)
          {
            feels += "Cool";
          }
          else if (answer < 20)
          {
            feels += "Perfect";
          }
          else if (answer < 25)
          {
            feels += "Warm";
          }
          else if (answer < 30)
          {
            feels += "Hot";
          }
          else
          {
            feels += "Too Hot!";
          }
          // print information about the request (browser, OS, etc)
          String line = client.readStringUntil('\r');

          // wait for end of client's request
          client.println("<tr><td>" + (String)answer + "</td><td>" + feels + "</td></tr>");
          delay(500);
          // client.println("The temperature is " + (String)answer + " which is " + feels);
        }
        client.println("</table><footer><p>Author: Michael Mena student number 000817498<br><a href='www.github.com/pogoMena/WebServer'>Github repository</a></p></footer>");
        break;
      }
    }
    // keep read client request data (if any). After that, we can terminate
    // our client connection
    while (client.available())
    {
      client.read();
    }
    // close the connection:
    client.stop();
    Serial.println(">> Client disconnected");
  }
}
