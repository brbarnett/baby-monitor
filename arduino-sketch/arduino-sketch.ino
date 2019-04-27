#include <SPI.h>
#include <WiFiNINA.h>

// wifi
char ssid[] = "barnett";
char pass[] = "--secret--";
int wifiStatus = WL_IDLE_STATUS;

WiFiClient client;

char server[] = "192.168.1.48";

void setup()
{
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0")
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (wifiStatus != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);
    Serial.println(wifiStatus);

    // wait 10 seconds for connection:
    delay(10000);
  }

  printWifiStatus();
}

void loop()
{
  int del = 200;       // 5 times per second
  int interval = 5000; // send data every 2 seconds
  int readings = interval / del;

  int averageLight = 0;
  int averageSound = 0;
  for (int i = 0; i < readings; i++)
  {
    averageLight = averageLight + analogRead(A0);
    averageSound = averageSound + square(analogRead(A1)); // square sound to make outliers more effective

    delay(del);
  }
  averageLight = averageLight / readings;
  averageSound = averageSound / readings;

  String data = "{ \"light\": " + String(averageLight) + ", \"sound\": " + String(averageSound) + " }";
  httpRequest(data);
}

int square(int x)
{
  return x * x;
}

void httpRequest(String data)
{
  // close any connection before send a new request.
  // This will free the socket on the Nina module
  client.stop();

  if (client.connect(server, 9090))
  {
    Serial.println("connected to server");
    // Make an HTTP request:
    client.println("POST /api/v1/A1_TEST_TOKEN/telemetry HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Content-Type: application/json");
    client.print("content-length: ");
    client.println(data.length());
    client.println();
    client.println(data);
    client.println();
    Serial.print("Sent: ");
    Serial.println(data);
  }
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
