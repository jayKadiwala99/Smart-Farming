#include<ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>
#include<SimpleDHT.h>

HTTPClient http;  //Declare an object of class HTTPClient
 
const char* ssid = "i_m_jaykadi";
const char* password = "244466666";

int soil_pin = 0;//moisture sensor pin
int pinDHT11 = 2; //DHT pin 
int soil_value = 0;
int moisture_value = 0;
int LED_pin = 5;
String payload="";
int httpCode=0;

SimpleDHT11 dht11(pinDHT11); // making object
  byte temperature = 0;
  byte humidity = 0;
 
void setup () 
{
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print("Connecting..");
  }

  pinMode(LED_pin,OUTPUT);
}
 
void loop() {

  //interfacing os DHT11 sensor
  Serial.println("#############################");
  Serial.println("Start sampling");
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    //Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  Serial.println("Temp:"+String((int)temperature)+" *C"); 
  Serial.println("Humidity:"+String((int)humidity)+" H");
  
  //interfacing of soil moisture sensor
  soil_value = analogRead(soil_pin);
  moisture_value = map(soil_value,1023,0,0,100);
  Serial.println("Moisture : "+ String(moisture_value));
  Serial.println("#############################");

 
  if (WiFi.status() == WL_CONNECTED) 
  { 

  http.begin("http://paniculate-wholesal.000webhostapp.com/add_sensors_data.php?t="+String(temperature)+"&h="+String(humidity)+"&m="+String(moisture_value));  //Specify request destination
  httpCode = http.GET();                                                                  //Send the request   
  if (httpCode > 0) 
  {    
    payload = http.getString();   //Get the request response payload
    Serial.println("Recieved Payload"+payload);                     //Print the response payload
    if(payload== "Data Received")
    {
      Serial.println("Data Stored in database!!!!!");
    }
    else
    {
      Serial.println("Data Stored Fail!!!!!");
    }
  }

  http.begin("http://paniculate-wholesal.000webhostapp.com/motor_status.php");  //Specify request destination
  httpCode = http.GET();                                                                  //Send the request   
  if (httpCode > 0) 
  {    
    payload = http.getString();   //Get the request response payload
    Serial.println("Recieved Payload: "+payload);                     //Print the response payload
    if(payload.indexOf("ON")>0)
    {
      Serial.println("Motor Punp is ON!!!!");
      digitalWrite(5,HIGH);
    }
    else
    {
      Serial.println("Motor Punp is OFF!!!!");
      digitalWrite(5,LOW);
    }
  }
  

  
  http.end();   //Close connection
}
 
delay(3000);    //Send a request every 30 seconds
 
}
