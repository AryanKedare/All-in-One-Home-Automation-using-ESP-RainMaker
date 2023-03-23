/*
 * 
 * This is the code for the project,
 * 
 * Home Automation project using ESP RainMaker made using ESP32
 * 
 * This code was written by Arun Suthar for techiesms YouTube channel
 * 
 * Changes for OLD PCB done by AryanKedare
 * 
 * This code is provided free for project purpose and fair use only.
 * Do mail us if you want to use it commercially 
 * 
 * techiesms@gmail.com
 * Copyrighted © by techiesms
 * 
 * 
 * Watch out it's complete tutorial on our YouTube channel 
 * https://www.YouTube.com/techiesms
 */


//This example demonstrates the ESP RainMaker with a standard Switch device.
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include <AceButton.h>
#include <wifi_provisioning/manager.h>
#include <esp_heap_caps.h>
#include <Preferences.h>
Preferences pref;

//for BLE
const char *service_name = "Name"; //name of node in BLE
const char *pop = "Password"; //password

using namespace ace_button;

#define DEBUG_SW 1 // for ON/OFF serial moniter
#define DEFAULT_RELAY_STATE true

// GPIO for push button
static uint8_t gpio_reset = 0;

// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relay 1
bool toggleState_2 = LOW; //Define integer to remember the toggle state for relay 2
bool toggleState_3 = LOW; //Define integer to remember the toggle state for relay 3
bool toggleState_4 = LOW; //Define integer to remember the toggle state for relay 4

// GPIO for switch
static uint8_t switch1 = 32;
static uint8_t switch2 = 35;
static uint8_t switch3 = 34;
static uint8_t switch4 = 39;

// GPIO for Relay (Appliance Control)
static uint8_t relay1 = 15;
static uint8_t relay2 = 2;
static uint8_t relay3 = 4;
static uint8_t relay4 = 22;

//LEDs Status
static uint8_t LED1 = 26;
static uint8_t LED2 = 25;
static uint8_t LED3 = 27;

/* Variable for reading pin status*/
bool switch_state_ch1 = LOW;
bool switch_state_ch2 = LOW;
bool switch_state_ch3 = LOW;
bool switch_state_ch4 = LOW;

bool wifi_scanning  = 0;


static Switch my_switch1("Switch 1", &relay1);
static Switch my_switch2("Switch 2", &relay2);
static Switch my_switch3("Switch 3", &relay3);
static Switch my_switch4("Switch 4", &relay4);

ButtonConfig config1;
AceButton button1(&config1);
ButtonConfig config2;
AceButton button2(&config2);
ButtonConfig config3;
AceButton button3(&config3);
ButtonConfig config4;
AceButton button4(&config4);


void handleEvent1(AceButton*, uint8_t, uint8_t);
void handleEvent2(AceButton*, uint8_t, uint8_t);
void handleEvent3(AceButton*, uint8_t, uint8_t);
void handleEvent4(AceButton*, uint8_t, uint8_t);

void sysProvEvent(arduino_event_t *sys_event)
{
  switch (sys_event->event_id)
  {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      printQR(service_name, pop, "ble");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("\nConnected IP address : ");
      Serial.println(IPAddress(sys_event->event_info.got_ip.ip_info.ip.addr));
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("\nDisconnected. Connecting to the AP again... ");
      break;
    case ARDUINO_EVENT_PROV_CRED_RECV:
      Serial.println("\nReceived Wi-Fi credentials");
      Serial.print("\tSSID : ");
      Serial.println((const char *) sys_event->event_info.prov_cred_recv.ssid);
      Serial.print("\tPassword : ");
      Serial.println((char const *) sys_event->event_info.prov_cred_recv.password);
      break;
    case ARDUINO_EVENT_PROV_INIT:
      wifi_prov_mgr_disable_auto_stop(10000);
      break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      Serial.println("Stopping Provisioning!!!");
      wifi_prov_mgr_stop_provisioning();
      break;
  }
}

void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
{
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  if (strcmp(device_name, "Switch 1") == 0)
  {
    Serial.printf("Switch value_1 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch1 = val.val.b;
      (switch_state_ch1 == false) ? digitalWrite(relay1, LOW) : digitalWrite(relay1, HIGH);
      param->updateAndReport(val);
    }

  }
  if (strcmp(device_name, "Switch 2") == 0) {

    Serial.printf("Switch value_2 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch2 = val.val.b;
      (switch_state_ch2 == false) ? digitalWrite(relay2, LOW) : digitalWrite(relay2, HIGH);
      param->updateAndReport(val);
    }
  }
  if (strcmp(device_name, "Switch 3") == 0) {

    Serial.printf("Switch value_3 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch3 = val.val.b;
      (switch_state_ch3 == false) ? digitalWrite(relay3, LOW) : digitalWrite(relay3, HIGH);
      param->updateAndReport(val);
    }
  }
  if (strcmp(device_name, "Switch 4") == 0) {

    Serial.printf("Switch value_4 = %s\n", val.val.b ? "true" : "false");

    if (strcmp(param_name, "Power") == 0) {
      Serial.printf("Received value = %s for %s - %s\n", val.val.b ? "true" : "false", device_name, param_name);
      switch_state_ch4 = val.val.b;
      (switch_state_ch4 == false) ? digitalWrite(relay4, LOW) : digitalWrite(relay4, HIGH);
      param->updateAndReport(val);
    }
  }
}

void getRelayState()
{
  toggleState_1 = pref.getBool("Relay1", 0);
  Serial.print("Last State Relay1 - "); Serial.println(toggleState_1);
  digitalWrite(relay1, toggleState_1);
  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_1);
  delay(200);
  toggleState_2 = pref.getBool("Relay2", 0);
  Serial.print("Last State Relay2- "); Serial.println(toggleState_2);
  digitalWrite(relay2, toggleState_2);
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_2);
  delay(200);
  toggleState_3 = pref.getBool("Relay3", 0);
  Serial.print("Last State Relay3- "); Serial.println(toggleState_3);
  digitalWrite(relay3, toggleState_3);
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_3);
  delay(200);
  toggleState_4 = pref.getBool("Relay4", 0);
  Serial.print("Last State Relay4- "); Serial.println(toggleState_4);
  digitalWrite(relay4, toggleState_4);
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, toggleState_4);
  delay(200);
}

void All_Lights_Off()
{
  switch_state_ch1 = 0;
  digitalWrite(relay1, LOW);
  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch1);
  pref.putBool("Relay1", switch_state_ch1);

  switch_state_ch2 = 0;
  digitalWrite(relay2, LOW);
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch2);
  pref.putBool("Relay2", switch_state_ch2);

  switch_state_ch3 = 0;
  digitalWrite(relay3, LOW);
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch3);
  pref.putBool("Relay3", switch_state_ch3);

  switch_state_ch4 = 0;
  digitalWrite(relay4, LOW);
  my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch4);
  pref.putBool("Relay4", switch_state_ch4);

}

void All_Lights_On()
{
  switch_state_ch1 = 1;
  digitalWrite(relay1, HIGH);
  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch1);
  pref.putBool("Relay1", switch_state_ch1);

  switch_state_ch2 = 1;
  digitalWrite(relay2, HIGH);
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch2);
  pref.putBool("Relay2", switch_state_ch2);

  switch_state_ch3 = 1;
  digitalWrite(relay3, HIGH);
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch3);
  pref.putBool("Relay3", switch_state_ch3);

  switch_state_ch4 = 1;
  digitalWrite(relay4, HIGH);
  my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch4);
  pref.putBool("Relay4", switch_state_ch4);

}

void setup()
{

  Serial.begin(115200);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED3, HIGH);
  delay(200);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);

  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED3, HIGH);
  delay(200);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  // Configure the input GPIOs
  pinMode(gpio_reset, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(switch1, INPUT_PULLUP);
  pinMode(switch2, INPUT_PULLUP);
  pinMode(switch3, INPUT_PULLUP);
  pinMode(switch4, INPUT_PULLUP);

  // Write to the GPIOs the default state on booting
  digitalWrite(relay1, DEFAULT_RELAY_STATE);
  digitalWrite(relay2, DEFAULT_RELAY_STATE);
  digitalWrite(relay3, DEFAULT_RELAY_STATE);
  digitalWrite(relay4, DEFAULT_RELAY_STATE);

  config1.setEventHandler(button1Handler);
  config2.setEventHandler(button2Handler);
  config3.setEventHandler(button3Handler);
  config4.setEventHandler(button4Handler);

  button1.init(switch1);
  button2.init(switch2);
  button3.init(switch3);
  button4.init(switch4);


  Node my_node;
  my_node = RMaker.initNode("ESP32-RainMaker");

  my_switch1.addCb(write_callback);
  my_node.addDevice(my_switch1);
  delay(500);

  my_switch2.addCb(write_callback);
  my_node.addDevice(my_switch2);
  delay(500);

  my_switch3.addCb(write_callback);
  my_node.addDevice(my_switch3);
  delay(500);

  my_switch4.addCb(write_callback);
  my_node.addDevice(my_switch4);
  delay(500);

  //This is optional
  RMaker.enableOTA(OTA_USING_PARAMS);
  //If you want to enable scheduling, set time zone for your region using setTimeZone().
  //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
  // RMaker.setTimeZone("Asia/Shanghai");
  // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
  RMaker.enableTZService();
  RMaker.enableSchedule();

  Serial.printf("\nStarting ESP-RainMaker\n");
  RMaker.start();

  WiFi.onEvent(sysProvEvent);
#if CONFIG_IDF_TARGET_ESP32
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
#else
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
#endif

  getRelayState(); // Get the last state of Relays

  my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
  my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
  my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
  my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
}


void loop()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    delay(500);
  }
  else
  {
   digitalWrite(LED1, HIGH);
   digitalWrite(LED2, HIGH);
   digitalWrite(LED3, HIGH);
  }

  button1.check();
  button2.check();
  button3.check();
  button4.check();


  // Read GPIO0 (external button to gpio_reset device
  if (digitalRead(gpio_reset) == LOW) {
    //Push button pressed
    Serial.printf("reset Button Pressed!\n");
    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_reset) == LOW) delay(50);
    int endTime = millis();

    if ((endTime - startTime) > 5000) {
      // If key pressed for more than 5secs, reset all
      Serial.printf("reset to factory.\n");
      RMakerFactoryReset(2);
    }
  }
  delay(100);
}


//functions for defineing manual switch

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  //EspalexaDevice* d1 = espalexa.getDevice(0);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch1 = true;
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch1);
      digitalWrite(relay1, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch1 = false;
      my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch1);
      digitalWrite(relay1, LOW);
      break;
  }
}
void button2Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT2");
  //EspalexaDevice* d2 = espalexa.getDevice(1);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch2 = true;
      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch2);
      digitalWrite(relay2, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch2 = false;
      my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch2);
      digitalWrite(relay2, LOW);
      break;
  }
}
void button3Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT3");
  //EspalexaDevice* d3 = espalexa.getDevice(2);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch3 = true;
      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch3);
      digitalWrite(relay3, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch3 = false;
      my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch3);
      digitalWrite(relay3, LOW);
      break;
  }
}
void button4Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT4");
  //EspalexaDevice* d4 = espalexa.getDevice(3);
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("kEventPressed");
      switch_state_ch4 = true;
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch4);
      digitalWrite(relay4, HIGH);
      break;
    case AceButton::kEventReleased:
      Serial.println("kEventReleased");
      switch_state_ch4 = false;
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, switch_state_ch4);
      digitalWrite(relay4, LOW);
      break;
  }
}
