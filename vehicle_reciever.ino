
#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"
#include <Servo.h>
#define CE_PIN 22
#define CSN_PIN 23

t
const int X_pin = 0; 
const int Y_pin = 1; 

//--------ultrasonic sensor pins --------
int trigPin = 10; 
int echoPin = 11;

const int engineA = 2;
const int engineB = 7;

const int right_engine_in1 = 3;
const int right_engine_in2 = 4;
const int right_engine_in3 = 5;
const int right_engine_in4 = 6;

const byte address[6] = "00001";

unsigned long previousMillis = 0;
const long time_interval = 3000;  // 2 second
long duration, inches;
int cm;
bool flag = false;

RF24 radio(CE_PIN, CSN_PIN);

Servo myservo;
int x;
int y;
int drive;
int pos;

unsigned long lastRadioTime = 0;
const unsigned long RADIO_TIMEOUT = 1000;

struct payload {
  int x;
  int y;
};

// does not work with automomous func implementation
// struct rpi_data {
//   int x_axes;
//   int y_axes;
// };

payload payload;

//rpi_data rpi_package;

void setup() {
  myservo.attach(8);
  
  //--------initialize a radio module -----------------
  radio.begin();
  
  //Append ACK packet from the receiving radio back to the transmitting radio
  radio.setAutoAck(false); //(true|false)
  radio.setChannel(0x6f);
  radio.setCRCLength(RF24_CRC_8);
  //Set the transmission datarate
  radio.setDataRate(RF24_1MBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  //Greater level = more consumption = longer distance
  radio.setPALevel(RF24_PA_MIN); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
  //Default value is the maximum 32 bytes
  radio.setPayloadSize(sizeof(payload));
  //Act as receiver
  radio.openReadingPipe(1, address);
  radio.startListening();

  //--------- engines pins initialisation ----------
  pinMode(engineA, OUTPUT);
  pinMode(right_engine_in1, OUTPUT);
  pinMode(right_engine_in2, OUTPUT);

  pinMode(engineB, OUTPUT);
  pinMode(right_engine_in3, OUTPUT);
  pinMode(right_engine_in4, OUTPUT);

  //digitalWrite(SW_pin, HIGH);
  digitalWrite(right_engine_in1, LOW);
  digitalWrite(right_engine_in2, LOW);
  digitalWrite(right_engine_in3, LOW);
  digitalWrite(right_engine_in4, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
}
void loop() {
  
  
  if (radio.available()) {
      radio.read(&payload, sizeof(payload));
      int *x_ptr = &x;
      int *y_ptr = &y;
      *x_ptr = payload.x;
      *y_ptr = payload.y;
      Serial.println("Received radio");
      Serial.println(radio.available());
      Serial.print("payload.x ");
      Serial.println(payload.x);
      Serial.print("payload.y ");
      Serial.println(payload.y);
      lastRadioTime = millis();
  } 
  // --- Fallback to RPi if radio is lost ---
  if (millis() - lastRadioTime > RADIO_TIMEOUT) {
    delay(50);
    data_from_rpi();
    Serial.println("Switched to RPi autonomy");
  }
  
  //--------convert data into dergees of wheels -----------
  float y_value = (float) y;
  float angle_value = -1*(503-y_value)/1023*180;
  int angle_pos = 90 - round(angle_value);
  //Serial.print(angle_pos);
  int distance = distance_sensor();
  if (distance < 10){
    x = 1023;
    y = 0;

  }

  /*------------ moving strait -----------------*/
  if (x > 522){
    drive = map(x, 522, 1023, 0, 255);
    analogWrite(engineA, drive);
    digitalWrite(right_engine_in1, LOW);
    digitalWrite(right_engine_in2, HIGH);

    analogWrite(engineB, drive);
    digitalWrite(right_engine_in3, HIGH);
    digitalWrite(right_engine_in4, LOW);

  }
  /*---------------moving back-------------------*/
  else if (x < 510){
    drive = map(x, 500, 0, 0, 255);
    analogWrite(engineA, drive);
    digitalWrite(right_engine_in1, HIGH);
    digitalWrite(right_engine_in2, LOW);

    analogWrite(engineB, drive);
    digitalWrite(right_engine_in3, LOW);
    digitalWrite(right_engine_in4, HIGH);
  }
  /*----------------neutral----------------------*/
  else{
    digitalWrite(right_engine_in1, LOW);
    digitalWrite(right_engine_in2, LOW);
    digitalWrite(right_engine_in3, LOW);
    digitalWrite(right_engine_in4, LOW);

  }
    // Serial.print("x:");
    // Serial.print(payload.x);
    // Serial.print("; ");
    // Serial.print("y:");
    // Serial.print(payload.y);
    // Serial.print("; ");
    // Serial.print("pos: ");
    // Serial.println(angle_pos);
    
  myservo.write(angle_pos);
  radio.flush_rx();

  Serial.print("distance from us sensor: ");
  Serial.println(distance);

  delay(50);

}

//-----------retrieve data from RPI ------------
// Python script on rpi4 process incoming webcam data with opencv 
// and pretrained object detection model on Edge Impulse
// sends data into arduino via serial1 

void data_from_rpi (){  
  // if frame data are not available, objects are not detected, 
  // car turns around

  bool *flag_ptr = &flag;
  unsigned long startTime = millis();
  while (!Serial1.available()) {
        
        if (millis() - startTime > 3000) {  // Timeout after 2 seconds
          
          Serial.println("autonomous");
          if (*flag_ptr==true){
                x = 100;
                y = 1024;
                *flag_ptr = false;  
          } else {
              x = 900;
              y = 0;
              *flag_ptr = true;
          }
        return;       
  }
  
  // if serial data are available
  if (Serial1.available()) {
      //Serial1.readBytes((char*)&rpi_package, sizeof(rpi_package));
      String data = Serial1.readStringUntil('\n');
      float horizontal_axes = data.substring(0, data.indexOf(',')).toFloat();
      float vertical_axes  = data.substring(data.indexOf(',') + 1).toFloat();
      x = 100;//rpi_package.x_axes;
      y = 1023.0 - horizontal_axes;
      
      Serial.print("rpi_package.y_axes: ");
      Serial.println(horizontal_axes);
      Serial.print("Serial1 data: ");
      Serial.print(data);
      Serial.println(Serial1.available());
      //Serial.print("Serial rpi is available cm: ");
      //Serial.println(cm);
  }

  
}
 // -----------ultrasound distance sensor presettings ----------
int distance_sensor(){

  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  //Serial.print("cm: ");
  //Serial.println(cm);
  return cm;
}


