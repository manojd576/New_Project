/*
  WiFi Router Fan Controller - FINAL CORRECTED CODE (Active HIGH Logic)
  --------------------------------------------------------------------
  මෙම කේතය C1815 වැනි NPN ට්‍රාන්සිස්ටරයකින් සාදන ලද
  "Active HIGH" driver පරිපථයක් සඳහා නිවැරදි කර ඇත.
*/

#include <math.h> 

// --- Pin සම්බන්ධතා ---
#define THERMISTOR_PIN 34 
#define RELAY_PIN 23

// --- උෂ්ණත්ව සීමාවන් ---
const float TEMP_THRESHOLD_ON = 34.50; 
const float TEMP_THRESHOLD_OFF = 30.50; 

// --- Thermistor නියත අගයන් ---
#define SERIES_RESISTOR 100000    
#define NOMINAL_RESISTANCE 100000  
#define B_COEFFICIENT 3450 
#define NOMINAL_TEMPERATURE_KELVIN 298.15

// --- පද්ධතියේ තත්වය මතක තබා ගැනීමට විචල්‍යයක් ---
bool isFanOn = false;


void setup() {
  Serial.begin(115200);
  Serial.println("\n\nWiFi Router Fan Controller - FINAL CODE Initializing...");

  pinMode(RELAY_PIN, OUTPUT);


  // Active HIGH නිසා, ආරම්භයේදී Fan OFF කිරීමට LOW signal එකක් යැවිය යුතුය.
  digitalWrite(RELAY_PIN, LOW); 
  isFanOn = false;
  Serial.println("System Initialized. Fan should be OFF.");
}

void loop() {
  // 1. උෂ්ණත්වය ගණනය කිරීම
  int adcValue = analogRead(THERMISTOR_PIN);
  if (adcValue == 0) {
    Serial.println("Error reading thermistor!");
    delay(5000);
    return;
  }
  float resistance = SERIES_RESISTOR * ( (4095.0 / adcValue) - 1.0 );
  float steinhart = log(resistance / NOMINAL_RESISTANCE) / B_COEFFICIENT + (1.0 / NOMINAL_TEMPERATURE_KELVIN);
  float tempC = (1.0 / steinhart) - 273.15;

  // 2. වත්මන් උෂ්ණත්වය පෙන්වීම
  Serial.print("Current Temperature: ");
  Serial.print(tempC, 2);
  Serial.print(" °C  |  ");

  // 3. **** Hysteresis තර්කනය ****
  if (isFanOn == false) {
    Serial.print("Fan Status: OFF. ");
    if (tempC >= TEMP_THRESHOLD_ON) {
      // Fan ON කිරීමට HIGH signal එක යැවීම
      digitalWrite(RELAY_PIN, HIGH); 
      isFanOn = true;
      Serial.println("-> Turning ON!");
    } else {
      Serial.println("(Waiting to reach ON threshold)");
    }
  }
  else { // isFanOn == true
    Serial.print("Fan Status: ON.  ");
    if (tempC <= TEMP_THRESHOLD_OFF) {
      // Fan OFF කිරීමට LOW signal එක යැවීම
      digitalWrite(RELAY_PIN, LOW); 
      isFanOn = false;
      Serial.println("-> Turning OFF!");
    } else {
      Serial.println("(Waiting to reach OFF threshold)");
    }
  }

  delay(90000); 
}