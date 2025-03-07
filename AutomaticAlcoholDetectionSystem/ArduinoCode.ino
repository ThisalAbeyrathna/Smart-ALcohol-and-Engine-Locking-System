#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Your network credentials
const char* ssid = "Galaxy A04s 3B9C";
const char* password = "pnle1904";

//const char* ssid = "Galaxy A54 5G EB0D";
//const char* password = "AxioGameOf@369";

const char* nic = "587964257";  // Replace with appropriate value

// Your server's IP address or domain name
const char* serverName = "http://192.168.19.60/AutomaticAlcoholDetectionSys/location.php";
//const char* serverName = "http://192.168.173.14/AutomaticAlcoholDetectionSys/receive_data.php";

// Set up GPS module
static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

// Define the I2C address and display size (assuming a 16x2 LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows

// Define the I2C pin connections for ESP32
const int SDA_PIN = 21;  // GPIO pin for SDA
const int SCL_PIN = 22;  // GPIO pin for SCL

// Define the pin connections
const int alcoholPin = 34;  // Analog pin for alcohol sensor
const int ledPin = 23;      // GPIO pin for LED
const int motorPin = 32;    // GPIO pin for motor
const int buzzerPin = 27;   // GPIO pin for the Buzzer
const int threshold = 1450; // Adjust this threshold based on your sensor readings

// Flag to indicate if data has been sent
bool dataSent = false;
bool systemStopped = false;  // Flag to stop the system after sending data
bool isDrunk = false;        // Flag for drunk detection

// Setup WiFi
void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// Initialize hardware (LCD, I2C, pins)
void initializeHardware() {
  // Initialize serial communication for GPS
  Serial.begin(115200);
  ss.begin(GPSBaud);

  // Initialize I2C with custom SDA and SCL pins for ESP32
  Wire.begin(SDA_PIN, SCL_PIN);  // Set up I2C communication

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.backlight();  // Turn on the LCD backlight

  // Initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  //pinMode(alcoholPin, INPUT);  // Initialize Input Pin

  // Display a welcome message on the LCD
  lcd.setCursor(0, 0);
  lcd.print("Alcohol Detector");
  delay(2000);  // Wait for 2 seconds
  lcd.clear();
  lcd.print("Scanning...");
}

// Check alcohol level over a period of 1 minute
void checkAlcoholLevelForOneMinute() {
  unsigned long startTime = millis();
  int alcoholLevel;
  
  while (millis() - startTime < 30000) { // 1 minute = 60,000 milliseconds
    alcoholLevel = analogRead(alcoholPin);
    Serial.print("Alcohol Level: ");
    Serial.println(alcoholLevel);

    if (alcoholLevel > threshold) {
      isDrunk = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You are drunk!");
      activateAlert();
      break;  // Stop scanning if alcohol level exceeds
    }
    
    delay(1000);  // Check every second
  }

  if (!isDrunk) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You are Safe");
    deactivateAlert();
  }
}

// Activate LED, motor, and buzzer when alcohol level exceeds the threshold
void activateAlert() {
  digitalWrite(ledPin, HIGH);  // Turn on LED
  digitalWrite(motorPin, LOW); // Stop motor
  activateBuzzer(10);          // Activate buzzer for 5 seconds
}

// Deactivate LED, motor, and buzzer when alcohol level is below the threshold
void deactivateAlert() {
  digitalWrite(ledPin, LOW);   // Turn off LED
  digitalWrite(motorPin, HIGH);// Turn on motor
  digitalWrite(buzzerPin, LOW);// Turn off buzzer
}

// Activate buzzer for a specified number of seconds
void activateBuzzer(int durationInSeconds) {
  for (int i = 0; i < durationInSeconds; i++) {
    digitalWrite(buzzerPin, HIGH);  // Turn buzzer ON
    delay(1000);                    // Wait for 1 second
    digitalWrite(buzzerPin, LOW);   // Turn buzzer OFF
    delay(1000);                    // Wait for 1 second (beep effect)
  }
}

// Send GPS data to the server
void sendGPSData() {
  if (WiFi.status() == WL_CONNECTED && ss.available() > 0) {
    while (!dataSent) {
      gps.encode(ss.read());
      if (gps.location.isUpdated() && gps.location.isValid()) {
          // float latitude = gps.location.lat();
          // float longitude = gps.location.lng();
        // Test values
             float latitude = 6.527079;  // Example latitude
             float longitude = 79.851244; // Example longitude

        Serial.print("Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);

        String location = String(latitude, 6) + "," + String(longitude, 6);
        String serverPath = serverName + String("?location=") + location + "&nic=" + nic;

        //String serverPath = String(serverName) + "?latitude=" + String(latitude, 6) + "&longitude=" + String(longitude, 6);
        Serial.print("Sending to server: ");
        Serial.println(serverPath);

        HTTPClient http;
        http.begin(serverPath.c_str());

        int httpResponseCode = http.GET();
        if (httpResponseCode > 0) {
          String payload = http.getString();
          Serial.println(httpResponseCode);
          Serial.println(payload);
          dataSent = true;  // Mark that data has been sent
          systemStopped = true;  // Mark the system to stop after sending data
        } else {
          Serial.print("Error on sending GET: ");
          Serial.println(httpResponseCode);
        }
        http.end();
      } else {
        Serial.println("Waiting for valid GPS signal...");
      }
    }
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void setup() {
  initializeHardware(); // Initialize hardware
  setupWiFi();          // Connect to WiFi
}

void loop() {
  if (systemStopped) {
    // System has stopped, don't run the loop
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Stopped");
    while (1);  // Infinite loop to stop further execution
  }

  checkAlcoholLevelForOneMinute();  // Check alcohol level for one minute
  
  if (isDrunk) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please stay calm");
    delay(3000);  // Show for 3 seconds
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Till the driver arrives");
    lcd.setCursor(0, 1);
    lcd.print("arrives");
    delay(3000);  // Show for 3 seconds

    sendGPSData();  // Send GPS data to server
  } else { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Safe to drive");
    delay(3000);
    systemStopped = true;
  }

  delay(500);  // Small delay for stability
}
