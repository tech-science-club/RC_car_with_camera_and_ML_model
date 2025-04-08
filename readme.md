# RC car with ML model on board

Arduino hardware-driven car with remote control with nRF24L01+LNA modules.

Aim:  
Develop pet project to study new things and open new horizons 
in robotics and machine learning.

List of materials:
 - Arduino Mega 2560
 - L298N motor driver
 - servos sg90
 - radio module nRF24L01+LNA, 2.4 GHz
 - Arduino Nano
 - Arduino joystick
 - Raspberry Pi4, 2 Gb RAM, Debian 12/64

## Project flow:
- test radio module compatibility
- design transmitter
- design receiver part
- attempt to train machine learning model with TensorFlow, Yolo, Edge Impulse
- test OpenCV lib on a PC with C++
- move OpenCV to raspberry Pi4
- connect raspberry Pi4 with servo via gpio 
- connect raspberry Pi4 with atmega 2560
- train and deploy Edge impulse model on raspberry Pi4
- test the car for bugs

### nRF24L01+LNA

Here is the proofread version of your next text:

Might be a pain in the ass due to its features. But its price and speed change everything.
To avoid problems with connectivity, it is highly recommended to read the official documentation, specifically the 
troubleshooting paragraph.
It is recommended to solder a capacitor to the 5V and GND pins on the radio module to reduce noise.
It is recommended to check the radio space for radio noise sources (microwaves, mobile devices, Bluetooth, Wi-Fi devices)  
and set the right channel for broadcasting.
It is recommended to use an adapter to avoid misconnections with 5V instead of 3.3V.

![nrf2401l](nRF01L_pics/nrf2401l.jpg)  

### Transmitter

Straitforward design with Arduino nano board, 2 joysticks for x and y representation and radio module to  
transmit data.

<img src="transmitter_pics/transmitter.jpg" alt="Alt text" style="width: 70%;"/>


<img src="/transmitter_pics/vehicle_transmitter.jpg" alt="Alt text" style="width: 70%;"/>

### Receiver

More complicated due to the variety of tasks. The Atmega 2560 has to receive data from the transmitter, process it,  
and pass values to the servo (steering control, accessing data from the Y-axis of the transmitter joystick)  
and to the wheels (X-axis data from the transmitter).
Moreover, a UART connection was established between the Raspberry Pi 4 and the Atmega 2560; Serial1 is assigned.
Also, an ultrasonic distance sensor was embedded to stop the car in case of obstacles.

<img src="reciever_pics/car.jpg" alt="Alt text" style="width: 70%;"/>

<img src="reciever_pics/rpi4_atmega_on_wheels.png" alt="Alt text" style="width: 70%;"/>

### Train ML learning model with TensorFlow, Yolo, Edge Impulse

The best performance results were achieved with Edge Impulse. It had minimum latency and approximately 15 FPS.  
In the same timeframe, the YOLOv5.tflite model achieved only around 1 FPS.
Training a model with TensorFlow was unsuccessful due to its dependency incompatibilities  
and confusing documentation regarding edge devices.

### Test opencv lib on pc with C++
To make a test with OpenCV lib and C++ was written a straightforward script which reads off video from  
laptop webcam, processes it with opencv and Haardcascade face detection set and pass data via serial communication  
to arduino board to move servoes. 

### Move opencv to raspberry Pi4

It is possible to use python based OpenCV lib and C++ lib. 
Easier way to do it is with Python. First of all, OpenCV lib originally is written on C++, python lib is just a bridge,  
way to use it in python projects. Secondly, The speed of all process is being determinated with image processing,
that related of hardware performance.

### Connect raspberry Pi4 with peripheries
It is possible with enabling of hardware GPIO pins and UART communication in raspi-config

### Train and deploy Edge impulse model on raspberry Pi4
There is well organized documentation and guidance on Edge Impulse web page. 
Just follow their instruction how to do everything step by step. There are possibilities to collect data,  
train model directly on their server and check it performance directly with web browser on mobile, laptop  
via internet having not downloaded it.
But it is paid service, is free for small size projects
A link to the Edge Impulse guidance is [here](https://docs.edgeimpulse.com/docs/tutorials/end-to-end-tutorials/computer-vision/object-detection/detect-objects-using-fomo)
.
How to deploy ML model on the Raspberry Pi4 board is well described [here](https://docs.edgeimpulse.com/docs/edge-ai-hardware/cpu/raspberry-pi-4)

### Test the car for bugs
Deploying on raspberry Pi4 board occurs as it is described on their documentation. Script downloads SDK for  
python together with ML model.
Among generated py files there is file classify.py which is responsible for object recognition and classification,
and it is ready to use, but we can modify it in desirable way according to our needs. 
There was noticed, usage of RPI4 demands a separate power source or some type of amplifier.  
It can not take a voltage fluctuation and needs at least 2A of current. That's why was decided to  
use 2 power supplies.
There was an issue with serial communication between RPI4 and atmega 2560. 
Initially, there was an attempt to use the struct library and pass data as struct packages, but it did not work that way.  
Instead, transmitting data as a string was used, and it worked quite well.
There was an issue with servo jitter when connected to the RPi 4. This was solved by using the gpiozero library.

### Result is here on video:

[![video](https://img.youtube.com/vi/I8UdSLM6D9M/0.jpg)](https://www.youtube.com/watch?v=I8UdSLM6D9M)

View from raspberry Pi4
[![video](https://img.youtube.com/vi/vrmn5sfN3jk/0.jpg)](https://www.youtube.com/watch?v=vrmn5sfN3jk)





