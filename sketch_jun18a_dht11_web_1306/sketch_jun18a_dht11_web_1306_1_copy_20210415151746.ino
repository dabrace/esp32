#include <SPI.h>
#include "DHT.h"
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <SSD1306.h>
#include "SSD1306Wire.h" 
#include <string.h>

const char* ssid     = "home-367D-2.4";
//const char *ssid    = "thermo";
const char* password = "copied4235brick";

WiFiServer server(80);

#if 0
typedef enum {
        WL_NO_SHIELD = 255,
        WL_IDLE_STATUS = 0,
        WL_NO_SSID_AVAIL,
        WL_SCAN_COMPLETED,
        WL_CONNECTED,
        WL_CONNECT_FAILED,
        WL_CONNECTION_LOST,
        WL_DISCONNECTED
} wl_status_t;
#endif

/*
    IPv4 Address
    10.0.0.127
    MAC Address
    24:62:AB:DE:A0:B8
    Comments

*/

#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

#define DHT22PIN 2
#define DHT22TYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
DHT dht22(DHT22PIN, DHT22TYPE);

float h;
float t;
float f;
float hif;
float hic;

float h22;
float t22;
float f22;
float hif22;
float hic22;

unsigned long loop_count = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels



#define TCAADDR 0x70
void TCA9548A(uint8_t bus)
{
	Wire.beginTransmission(TCAADDR);  // TCA9548A address is 0x70
	Wire.write(1 << bus);          // send byte to select bus
	Wire.endTransmission();
}

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
//Adafruit_SSD1306 display1(OLED_RESET);
//Adafruit_SSD1306 display2(OLED_RESET);
SSD1306 display1(0x3c, 21, 22);
//SSD1306 display2(0x3c, 21, 22);

#define TCA9548A_RESET 33

void readDHT11()
{
	static bool did_warn = false;
  
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	h = dht.readHumidity();
	// Read temperature as Celsius (the default)
	t = dht.readTemperature();
	// Read temperature as Fahrenheit (isFahrenheit = true)
	f = dht.readTemperature(true);

	// Check if any reads failed and exit early (to try again).
	if (isnan(h) || isnan(t) || isnan(f)) {
		if (!did_warn) {
			Serial.println(F("Failed to read from DHT 11 sensor!"));
			did_warn = true;
		}
		return;
	}

	// Compute heat index in Fahrenheit (the default)
	hif = dht.computeHeatIndex(f, h);
	// Compute heat index in Celsius (isFahreheit = false)
	hic = dht.computeHeatIndex(t, h, false);

	Serial.print(F("DHT11 Humidity: "));
	Serial.print(h);
	Serial.print(F("%  Temperature: "));
	Serial.print(t);
	Serial.print(F("°C "));
	Serial.print(f);
	Serial.print(F("°F  Heat index: "));
	Serial.print(hic);
	Serial.print(F("°C "));
	Serial.print(hif);
	Serial.print(F("°F"));
	Serial.print(" ");
	Serial.print(WiFi.localIP());
	Serial.println();
	delay(2000);
}

void readDHT22()
{
	static bool did_warn = false;
  
	// Reading temperature or humidity takes about 250 milliseconds!
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	h22 = dht22.readHumidity();
	// Read temperature as Celsius (the default)
	t22 = dht22.readTemperature();
	// Read temperature as Fahrenheit (isFahrenheit = true)
	f22 = dht22.readTemperature(true);

	// Check if any reads failed and exit early (to try again).
	if (isnan(h22) || isnan(t22) || isnan(f22)) {
		if (!did_warn) {
			Serial.println(F("Failed to read from DHT 22 sensor!"));
			did_warn = true;
		}
		return;
	}

	// Compute heat index in Fahrenheit (the default)
	hif22 = dht.computeHeatIndex(f, h);
	// Compute heat index in Celsius (isFahreheit = false)
	hic22 = dht.computeHeatIndex(t, h, false);

	Serial.print(F("DHT22 Humidity: "));
	Serial.print(h22);
	Serial.print(F("%  Temperature: "));
	Serial.print(t22);
	Serial.print(F("°C "));
	Serial.print(f22);
	Serial.print(F("°F  Heat index: "));
	Serial.print(hic22);
	Serial.print(F("°C "));
	Serial.print(hif22);
	Serial.print(F("°F"));
	Serial.print(" ");
	Serial.print(WiFi.localIP());
	Serial.println();
	delay(2000);
}

void initMyDisplay(SSD1306 *display, unsigned char displaynum)
{
	// Set multiplexer to channel 1 and initialize OLED-0 with I2C addr 0x3C
	TCA9548A(displaynum);
	//display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	//display->begin(SSD1306_EXTERNALVCC, 0x3C);
	display->init();
  display->connect();
}

void setup()
{
	int i = 0;
	Serial.begin(115200);
	Serial.println("Hello, Don");

	dht.begin();
	dht22.begin();

	delay(2000); // Pause for 2 seconds

  pinMode(TCA9548A_RESET, OUTPUT);

  digitalWrite(TCA9548A_RESET, HIGH);
  delay(2000);
  digitalWrite(TCA9548A_RESET, LOW);
  delay(2000);
  
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		      delay(1000);
        	//Serial.print(".");
        	//if ((++i % 40) == 0)
            	//Serial.println();
        	Serial.println("Not Connected!");
        	Serial.println(WiFi.status());
        	Serial.println(WL_NO_SSID_AVAIL);
        	Serial.println(WL_SCAN_COMPLETED);
        	Serial.println(WL_CONNECTED);
        	Serial.println(WL_CONNECT_FAILED);
        	Serial.println(WL_CONNECTION_LOST);
        	Serial.println(WL_DISCONNECTED);
	}

	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	server.begin();

	Serial.println("Initializing Display 0");
	initMyDisplay(&display1, 0);

	Serial.println("Initializing Display 1");
	initMyDisplay(&display1, 1);

	Serial.println("Setup completed");
} // setup

void printToLED(SSD1306 *display, unsigned char displaynum, int x, int y, char *m)
{
	static int counter = 0;

	TCA9548A(displaynum);
	//display->setTextColor(WHITE);
	//display->setTextSize(2);
	//display->setCursor(x, y);
	display->drawString(x,y, m);
	//display->print(m);
	display->display();
	++counter;
}

void clearDisplay(SSD1306 *display, unsigned char num)
{
	TCA9548A(num);
	display->clear();
}

void printDHT11()
{
	char buf[128];
	String IP(WiFi.localIP().toString());
	//static const char *ip = IP.data();
	int n;

	memset(buf, 0, sizeof(buf));
  
	readDHT11();
	readDHT22();

	clearDisplay(&display1, 0);
	clearDisplay(&display1, 1);

	n = sprintf(buf, "IP:%s", IP.c_str());
	printToLED(&display1, 0, 0, 0, buf);
	printToLED(&display1, 1, 0, 0, buf);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "Humidity:%s - %s", String(h), String(h22));
	printToLED(&display1, 0, 0, 15, buf);
	printToLED(&display1, 1, 0, 15, buf);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "HI:%s - %s", String(hif), String(hif22));
	printToLED(&display1, 0, 1, 25, buf);
	printToLED(&display1, 1, 2, 25, buf);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "T: %s - %s\0", String(f), String(f22));
	printToLED(&display1, 0, 0, 40, buf);
	printToLED(&display1, 1, 0, 40, buf);
}

void loop()
{

	readDHT11();
	readDHT22();

	printDHT11();
  
	WiFiClient client = server.available();   // listen for incoming clients

	if (client) {                             // if you get a client,
		Serial.println("New Client.");           // print a message out the serial port
		String currentLine = "";                // make a String to hold incoming data from the client
		while (client.connected()) {            // loop while the client's connected
			if (client.available()) {             // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then
				Serial.write(c);                    // print it out the serial monitor
				if (c == '\n') {                    // if the byte is a newline character
					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();

            					// the content of the HTTP response follows the header:
						client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
						client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");
						// the content of the HTTP response follows the header:
						client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
						client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

						client.print("<br>");
						client.print("<br>");
						client.print(F("Humidity: "));
						client.print(h);
						client.print(F("%  Temperature: "));
						client.print(t);
						client.print(F("C "));
						client.print(f);
						client.print(F("F  Heat index: "));
						client.print(hic);
						client.print(F("C "));
						client.print(hif);
						client.print(F("F"));
						client.print("  loop_count:");
						client.print(loop_count);
						client.print(" ");
						client.print(WiFi.localIP());
						client.print("<br>");

						++loop_count;
						// The HTTP response ends with another blank line:
						client.println();
						// break out of the while loop:
						break;
					} else {    // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				} else if (c != '\r') {  // if you got anything else but a carriage return character,
					currentLine += c;      // add it to the end of the currentLine
				}

        			// Check to see if the client request was "GET /H" or "GET /L":
				if (currentLine.endsWith("GET /H")) {
					//digitalWrite(2, HIGH);               // GET /H turns the LED on
				}
				if (currentLine.endsWith("GET /L")) {
					//digitalWrite(2, LOW);                // GET /L turns the LED off
				}
			} // if 
		} // while
	}
	// close the connection:
	client.stop();
	Serial.println("Client Disconnected.");
} // loop
