//This codes is for remote monitoring of water Quality parameters like:
//Turbidity
//Power of Hydrogen
//Total Dissolved Solute
//The flow rate of the water
//All this parameters are remotely pushed to the blynk cloud for reading 

#define BLYNK_TEMPLATE_ID "TMPL2Rr8_ScEn"
#define BLYNK_TEMPLATE_NAME "Smart Meter"
#define BLYNK_AUTH_TOKEN "uHmqZang7WdJ7UDQPNi2im65AY0i-GUG"

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "WIFI  NAME";
char pass[] = "Password";

// Sensor pins
#define TURBIDITY_SENSOR_PIN 34
#define TdsSensorPin 35
#define PH_SENSOR_PIN 36

// Virtual pins for Blynk
#define VIRTUAL_PIN_TURBIDITY V1
#define VIRTUAL_PIN_TDS V2
#define VIRTUAL_PIN_PH V0

// Constants
#define VREF 3.3              // Analog reference voltage of the ESP32
#define SCOUNT 30             // Sum of sample points

LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

// Arrays for storing analog readings from the TDS sensor
int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;

float temperature = 25; // Set default temperature for compensation, adjust as needed

void setup() {
  Serial.begin(115200);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to TDS");
  delay(2000); // Display welcome message for 2 seconds
  lcd.clear();

  pinMode(TURBIDITY_SENSOR_PIN, INPUT);
  pinMode(TdsSensorPin, INPUT);
  pinMode(PH_SENSOR_PIN, INPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(1000L, sendTurbidityToBlynk);
  timer.setInterval(1000L, sendTDSToBlynk);
  timer.setInterval(1000L, sendPHToBlynk);
}

void sendTurbidityToBlynk() {
  int sensorValue = analogRead(TURBIDITY_SENSOR_PIN);
  int turbidity = map(sensorValue, 0, 4095, 100, 0); // Adjust the range based on your sensor
  int great = turbidity - 50;

  Serial.print("Turbidity: ");
  Serial.println(great);

  Blynk.virtualWrite(VIRTUAL_PIN_TURBIDITY, great);

  if (great < 20) {
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    Serial.println("Status: CLEAR");
  } else if ((great >= 20) && (great < 35)) {
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    Serial.println("Status: CLOUDY");
  } else {
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    Serial.println("Status: DIRTY");
  }
}

void sendTDSToBlynk() {
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) {  // Every 40 milliseconds, read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
    }
  }

  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();

    for (int copyIndex = 0; copyIndex < SCOUNT; copyIndex++) {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    }

    float averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * VREF / 4095.0;
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensationVoltage = averageVoltage / compensationCoefficient;
    float tdsValue = 133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TDS: ");
    lcd.print(tdsValue, 0);
    lcd.print(" ppm");

    lcd.setCursor(0, 1);
    if (tdsValue < 50) {
      lcd.print("Below Minimum");
    } else if (tdsValue >= 50 && tdsValue <= 150) {
      lcd.print("Excellent");
    } else if (tdsValue > 150 && tdsValue <= 250) {
      lcd.print("Good");
    } else if (tdsValue > 250 && tdsValue <= 300) {
      lcd.print("Fair");
    } else if (tdsValue > 300 && tdsValue <= 500) {
      lcd.print("Poor");
    } else {
      lcd.print("Not Suitable");
    }

    Blynk.virtualWrite(VIRTUAL_PIN_TDS, tdsValue); // Send TDS value to Blynk app through Virtual Pin 2
  }
}

void sendPHToBlynk() {
  int phValue = analogRead(PH_SENSOR_PIN);
  float voltage = phValue * (VREF / 4095.0); // 3.3V is the ESP32 operating voltage
  float pH = 7.0 + ((2.0 * voltage) - 1.0); // Adjust the formula based on your sensor

  Serial.print("pH value: ");
  Serial.println(pH);

  Blynk.virtualWrite(VIRTUAL_PIN_PH, pH);

  lcd.setCursor(0, 1);
  lcd.print("pH: ");
  lcd.print(pH, 2); // Display pH value with 2 decimal points
}

int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (int i = 0; i < iFilterLen; i++) {
    bTab[i] = bArray[i];
  }
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0) {
    bTemp = bTab[(iFilterLen - 1) / 2];
  } else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void loop() {
  Blynk.run();
  timer.run();
}
