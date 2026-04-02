#include <Arduino.h>
#include "DHT.h"

// Pin Definitions
#define LED_PIN 2
#define BUTTON_PIN 4
#define DHT_PIN 21
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

TaskHandle_t ledTaskHandle;
TaskHandle_t sensorTaskHandle;
TaskHandle_t panicTaskHandle;
TaskHandle_t serialTaskHandle;

QueueHandle_t sensorQueue;

volatile bool panicTriggered = false;

typedef struct { 
  float temperature;
  float humidity;
}sensorData;

void ledTask(void *pvParameters) 
{
  while(true) 
  {
    Serial.println("LED Task Running");
    if(panicTriggered)
    {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Panic Mode: LED ON");
      vTaskSuspend(NULL);
    }

    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(250 / portTICK_PERIOD_MS);

    digitalWrite(LED_PIN, LOW);
    vTaskDelay(250 / portTICK_PERIOD_MS);
  }
}

void sensorTask(void *pvParameters)
{
  sensorData data_s ;
  while(true)
  {
    if(panicTriggered)
      vTaskSuspend(NULL);

    data_s.temperature = dht.readTemperature();
    data_s.humidity = dht.readHumidity();

    if(!isnan(data_s.temperature) && !isnan(data_s.humidity))
      xQueueSend(sensorQueue, &data_s, portMAX_DELAY);
    
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void serialTask(void *pvParameters)
{
  sensorData data_r;

  while (true)
  {
    if(xQueueReceive(sensorQueue, &data_r, portMAX_DELAY))
    {
      Serial.print("Temperature: ");
      Serial.print(data_r.temperature);
      Serial.print(" °C, Humidity: ");
      Serial.print(data_r.humidity);
      Serial.println(" %");
    }
  }
  
}

void panicTask(void *pvParameters)
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  while(true)
  {
    if(digitalRead(BUTTON_PIN)==LOW)
    {
      panicTriggered = true;
      vTaskSuspend(ledTaskHandle);
      vTaskSuspend(sensorTaskHandle);
      vTaskSuspend(serialTaskHandle);
      Serial.println("Panic Button Pressed! All tasks stopped.");
      vTaskSuspend(NULL);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() 
{
  Serial.begin(115200);
  dht.begin();

  sensorQueue = xQueueCreate(5, sizeof(sensorData));
  xTaskCreatePinnedToCore(ledTask, "LED Task", 1000, NULL, 1, &ledTaskHandle, 0);
  xTaskCreatePinnedToCore(sensorTask, "Sensor Task", 2000, NULL, 1, &sensorTaskHandle, 0);
  xTaskCreatePinnedToCore(serialTask, "Serial Task", 2000, NULL, 1, &serialTaskHandle, 1);
  xTaskCreatePinnedToCore(panicTask, "Panic Task", 1000, NULL, 2, &panicTaskHandle, 0);
}

void loop()
{

}