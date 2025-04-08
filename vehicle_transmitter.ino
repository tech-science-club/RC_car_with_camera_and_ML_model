// Arduino pin numbers

#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"

#define CE_PIN 9
#define CSN_PIN 10
#define INTERVAL_MS_TRANSMISSION 250

RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "00001";

const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 3; // analog pin connected to X output
const int Y_pin = 4; // analog pin connected to Y output

int x;
int y;

struct payload {
  int x;
  int y;
};

payload payload;

void setup() {
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  Serial.begin(9600);
  
  radio.begin();
  
  //Append ACK packet from the receiving radio back to the transmitting radio
  radio.setAutoAck(false); //(true|false)
  radio.setChannel(0x6f);
  radio.setCRCLength(RF24_CRC_8);
  //Set the transmission datarate
  radio.setDataRate(RF24_1MBPS); //(RF24_250KBPS|RF24_1MBPS|RF24_2MBPS)
  //Greater level = more consumption = longer distance
  radio.setPALevel(RF24_PA_MAX); //(RF24_PA_MIN|RF24_PA_LOW|RF24_PA_HIGH|RF24_PA_MAX)
  //Default value is the maximum 32 bytes
  radio.setPayloadSize(sizeof(payload));
  //Act as transmitter
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  payload.x = analogRead(X_pin);
  payload.y = analogRead(Y_pin);

  radio.write(&payload, sizeof(payload));

  Serial.print("x:");
  Serial.println(payload.x);

  Serial.print("y:");
  Serial.println(payload.y);

  Serial.println("*---------------------*");

  

  /*Serial.print("Switch:  ");
  Serial.print(digitalRead(SW_pin));
  Serial.print(" | ");
  Serial.print("X-axis: ");
  Serial.print(analogRead(X_pin));
  Serial.print(" | ");
  Serial.print("Y-axis: ");
  Serial.print(analogRead(Y_pin));
  Serial.println(" | ");*/
  delay(50);
}



//NRF24L01 buffer limit is 32 bytes (max struct size)


