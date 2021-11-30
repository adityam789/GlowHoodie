/*Simple example of multi loop
* By Evandro Luis Copercini
* Based on pcbreflux video
* Public domain license 2017
*/

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

volatile SemaphoreHandle_t binsem1;

//void loop1(void *pvParameters) {
//  while (1) {
//     Serial.println("loop1");
//     delay(1000);
//  }
//}
//
//void loop2(void *pvParameters) {
//  while (1) {
//     Serial.println("loop2");
//     delay(300);
//  }
//}
//
//void loop3(void *pvParameters) {
//  while (1) {
//     Serial.println("loop3");
//     delay(4000);
//  }
//}

volatile int variable = 0;

void vloop1(void *pvParameters) {
  while(1) {
//    Serial.println("we in loop1");
    if(xSemaphoreTake( binsem1, ( TickType_t ) 1000 ) == pdTRUE){
      Serial.print("vloop 1 decrease variable value: ");
      Serial.print(variable);
      variable = variable + 1;
      Serial.print(" to ");
      Serial.println(variable);
//      variable += 1;
//      Serial.println(variable);
      xSemaphoreGive(binsem1);
      delay(1);
    }
//    Serial.println("loop1 hi");
    delay(200);
  }
}

void vloop2(void *pvParameters) {
  while(1) {
//    Serial.println("we in loop2");
    if(xSemaphoreTake( binsem1, ( TickType_t ) 1000 ) == pdTRUE){
      Serial.print("vloop 2 decrease variable value: ");
      Serial.print(variable);
      variable = variable - 1;
      Serial.print(" to ");
      Serial.println(variable);
//      variable -= 1;
//      Serial.println(variable);
      xSemaphoreGive(binsem1);
      delay(1);
    }
//    Serial.println("loop2 hi");
    delay(300);
  }
}

void setup() {
  Serial.begin(9600);
//  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
//  xTaskCreatePinnedToCore(loop2, "loop2", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
//  xTaskCreatePinnedToCore(loop3, "loop3", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
//BaseType_t xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode, const char *constpcName, const uint32_t usStackDepth, void *constpvParameters,
//UBaseType_t uxPriority, TaskHandle_t *constpvCreatedTask, const BaseType_t xCoreID)
  binsem1 = xSemaphoreCreateBinary();
  if(binsem1 == NULL) {
    Serial.println("binsem1 create unsucessful");
  } else{
    Serial.println("binsem1 created sucessfully");
  }
  xSemaphoreGive(binsem1);
  xTaskCreatePinnedToCore(vloop1, "vloop1", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(vloop2, "vloop2", 4096, NULL, 1, NULL, ARDUINO_RUNNING_CORE);
}

void loop() {
   Serial.print("loop0 running: ");
   Serial.println(variable);
   delay(500);
}
