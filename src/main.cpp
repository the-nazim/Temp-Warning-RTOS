#include <Arduino.h>
#include "DHT.h"

// Pin Definitions
#define LED_PIN 2
#define BUTTON_PIN 4
#define DHT_PIN 21
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}