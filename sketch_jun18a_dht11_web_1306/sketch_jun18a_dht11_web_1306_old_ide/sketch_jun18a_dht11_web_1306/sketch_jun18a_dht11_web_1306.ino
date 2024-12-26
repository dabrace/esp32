#include <SPI.h>
#include "DHT.h"
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
//#include <Adafruit_SSD1306.h>
#include <SSD1306.h>
#include <string.h>

// FROG
// DAB - 09092021 4 OLED
// Board set to Heltec WiFi kit 32

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

#define DHTPIN 17     // Digital pin connected to the DHT sensor
#define DHT11TYPE DHT11   // DHT 11

#define DHT22PIN 16
#define DHT22TYPE DHT22

DHT dht(DHTPIN, DHT11TYPE);
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
//TwoWire TWOWire = TwoWire(0);


#define TCA9548A_RESET -1
#define TCAADDR 0x70
void TCA9548A(uint8_t bus)
{
  Wire.beginTransmission(TCAADDR);  // TCA9548A address is 0x70
	Wire.write(1 << bus);          // send byte to select bus
	Wire.endTransmission();
}

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

static SSD1306 display2(0x3c, 21, 22); // SDA, CLOCK
static SSD1306 display3(0x3c, 21, 22); // SDA, CLOCK
static SSD1306 display4(0x3c, 21, 22); // SDA, CLOCK
static SSD1306 display7(0x3c, 21, 22); // SDA, CLOCK

void initMyDisplay(SSD1306 *display, uint8_t displaynum)
{
  int rc = 0;
  
  // Set multiplexer to channel 1-7 and initialize OLED-0 with I2C addr 0x3C
  TCA9548A(displaynum);
 
  display->displayOn();
  rc = display->init();
  if (rc != 1) {
    Serial.print("Display initialization failed for:");
    Serial.print(rc);
    Serial.println(" ");
  } else {
    Serial.print("Display ");
    Serial.print(displaynum);
    Serial.println(" Initialized...");
  }
  //display->wakeup();
  display->normalDisplay();
  //display->setBrightness(255);
  
}

void clearDisplay(SSD1306 *display, uint8_t num)
{
  TCA9548A(num);
  display->clear();
}

void printToLED(SSD1306 *display, uint8_t displaynum, int x, int y, char *m)
{
  static int counter = 0;
 
  TCA9548A(displaynum);
  //delay(600);
  //display->setTextColor(WHITE);
  if (displaynum > 0)
      display->setFont(ArialMT_Plain_10);
  else
      display->setFont(ArialMT_Plain_16);
      
  //display->setCursor(x, y);
  display->normalDisplay();
  display->setColor(WHITE);
  display->drawString(x,y, m);
  //display->print(m);
  display->display();
  ++counter;
}

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

void setup()
{
	int i = 0;
	Serial.begin(115200);
	Serial.println("Hello, Don");

	dht.begin();
  delay(2000);
	dht22.begin();

	delay(1000); // Pause for 2 seconds
    
	Serial.println("Setting up WiFi");
  
	WiFi.begin(ssid, password);

	delay(8000);

	Serial.println("Waiting to connect");
  
	while (WiFi.status() != WL_CONNECTED) {
		delay(2000);
        	//Serial.print(".");
        	//if ((++i % 40) == 0)
            	//Serial.println();
        	Serial.print("Not Connected! Status = ");
        	Serial.print(WiFi.status());
		Serial.print(" ");
        	Serial.print(WL_NO_SSID_AVAIL);
		Serial.print(" ");
        	Serial.print(WL_SCAN_COMPLETED);
		Serial.print(" ");
        	Serial.print(WL_CONNECTED);
		Serial.print(" ");
        	Serial.print(WL_CONNECT_FAILED);
		Serial.print(" ");
        	Serial.print(WL_CONNECTION_LOST);
		Serial.print(" ");
        	Serial.println(WL_DISCONNECTED);
          
    WiFi.begin(ssid, password);
	}

	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	server.begin();

  Serial.println("\ndone\n");

  initMyDisplay(&display2, 0);
  initMyDisplay(&display2, 1);
	Serial.println("Initializing Display 2");
	initMyDisplay(&display2, 2);

	Serial.println("Initializing Display 3");
	initMyDisplay(&display3, 3);
  //delay(1000);

  Serial.println("Initializing Display 4");
  initMyDisplay(&display4, 4);

  Serial.println("Initializing Display 7");
  initMyDisplay(&display7, 7);
 // delay(1000);
  
	Serial.println("Setup completed");

  for (uint8_t t=0; t<8; t++) {
      TCA9548A(t);
      Serial.print("TCA Port #"); Serial.print(t);

      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) {
          Serial.println();                    
          continue;
        }          

        Wire.beginTransmission(addr);
        if (!Wire.endTransmission()) {
          Serial.print("\t\tFound I2C 0x");
          Serial.println(addr,HEX);
        }
      }
  }
  
	printToLED(&display2, 2, 0, 0, "HELLO");
  printToLED(&display2, 0, 0, 0, "HELLO");
  printToLED(&display2, 1, 0, 0, "HELLO");
	printToLED(&display3, 3, 0, 0, "HELLO");
  printToLED(&display4, 4, 0, 0, "HELLO");    
  printToLED(&display7, 7, 0, 0, "HELLO");  
  
  delay(8000);
} // setup

void printDHT11()
{
	char buf[128];
	String IP(WiFi.localIP().toString());
	//static const char *ip = IP.data();
	int n;

	memset(buf, 0, sizeof(buf));
  
	readDHT11();
	readDHT22();

	clearDisplay(&display2, 2);
	clearDisplay(&display3, 3);
  clearDisplay(&display4, 4);
  clearDisplay(&display7, 7);

	n = sprintf(buf, "IP:%s", IP.c_str());
	printToLED(&display2, 2, 0, 0, buf);
	printToLED(&display3, 3, 0, 0, buf);
  printToLED(&display4, 4, 0, 0, buf);    
  printToLED(&display7, 7, 0, 0, buf);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "Humidity:%s - %s", String(h), String(h22));
	printToLED(&display2, 2, 0, 15, buf);
	printToLED(&display3, 3, 0, 15, buf);
  printToLED(&display4, 4, 0, 15, buf);
  printToLED(&display7, 7, 0, 15, buf);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "HI:%s - %s", String(hif), String(hif22));
	printToLED(&display2, 2, 0, 25, buf);
	printToLED(&display3, 3, 0, 25, buf);
  printToLED(&display4, 4, 0, 25, buf);
  printToLED(&display7, 7, 0, 25, buf);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "T: %s - %s", String(f), String(f22));
	printToLED(&display2, 2, 0, 40, buf);
	printToLED(&display3, 3, 0, 40, buf);
  printToLED(&display4, 4, 0, 40, buf);
  printToLED(&display7, 7, 0, 40, buf);
  delay(500);
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
	//Serial.println("Client Disconnected.");
} // loop
