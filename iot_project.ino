#include <ESP8266HTTPClient.h>

#include <ESP8266WiFi.h>
// #include <HTTPClient.h>

const char* ssid = "admin";
const char* password = "12345678";
int gasPin = A0; // analog input pin to read gas sensor
int gasValue;    // variable to store gas sensor reading
float temperature; // variable to store temperature reading
int lm35Pin = A0;  // analog input pin to read LM35 temperature sensor
const int CURRENT_SENSOR_PIN = A0;


int value1;
int value2;
int value3;



String server = "http://maker.ifttt.com";
String eventName = "grid_data";
String IFTTT_Key = "fkMGeEIQHm_nNcVcYcaAGUTDGHnOfhm9QliRg6EIPRe";
String IFTTTUrl="http://maker.ifttt.com/trigger/temp_data/with/key/e272MXJrh4_et5KUm56LmYHjJrNRtj9BjxUT5u6Njr7";





void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

}




void sendDataToSheet(void)
{
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key + "?value1=" + String((int)value1) + "&value2="+String((int)value2) +"&value3=" + String((int)value3);  
  Serial.println(url);
  //Start to send data to IFTTT
  HTTPClient http;
  WiFiClient wifiClient;
  Serial.print("[HTTP] begin...\n");
  http.begin(wifiClient, url); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

}















void loop() {

  // Gas Sensor
  gasValue = analogRead(gasPin); // read gas sensor value
  // Serial.print("Gas Sensor Value: "); // print gas sensor value
  // Serial.println(gasValue);
  // delay(1000); // wait for 1 second

  // Current sensor
    int currentSensorValue = analogRead(CURRENT_SENSOR_PIN);

  // Convert the analog value to voltage (assuming 5V supply voltage)
  float voltage = currentSensorValue * 5.0 / 1024.0;

  // Convert the voltage to current (assuming 30A current sensor)
  float current = voltage / 0.066;

  // // Print the current value to the serial monitor
  // Serial.print("Current: ");
  // Serial.print(current);
  // Serial.println(" A");

  // // Wait for 1 second before reading the sensor value again
  // delay(1000);

  // Temp sensor
    int sensorValue = analogRead(lm35Pin); // read sensor value
  temperature = (sensorValue * 3.3 / 1024) * 100; // convert sensor value to temperature in degree Celsius
  // Serial.print("Temperature: "); // print temperature value
  // Serial.print(temperature);
  // Serial.println(" C");
  // delay(1000); // wait for 1 second


  value1 = temperature;
  value2 = current;
  value3 = gasValue;
  Serial.print("Values are ");
  Serial.print(value1);
  Serial.print(' ');
  Serial.print(value2);
  Serial.print(' ');
  Serial.println(value3);
  Serial.print(' '); 

  sendDataToSheet();
  delay(10000);
}
 