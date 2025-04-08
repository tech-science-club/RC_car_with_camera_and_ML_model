#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cstring>
#include <fstream>
#include "SimpleSerial.cpp"

using namespace std;

double frameWidth;
double frameHeight;

int x_axes=0;
int y_axes=0;
int value [2];

std::string send_data;
char com_port[] = "\\\\.\\COM3";
std::string str = "here is a string\n";

DWORD COM_BAUD_RATE = CBR_9600;

SimpleSerial Serial(com_port, COM_BAUD_RATE);

int main() {
    // Load the Haar cascade classifier for face detection
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(cv::samples::findFile("rsc/haarcascade_frontalface_default.xml"))) {
        std::cerr << "Error loading face cascade\n";
        return -1;
    }

    // Open the default camera (camera index 0)
    cv::VideoCapture cap(1);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video capture\n";
        return -1;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame;  // Capture a new frame
        if (frame.empty()) {
            std::cerr << "No captured frame -- Break!\n";
            break;
        }
        frameWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        frameHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        // Convert frame to grayscale for better detection performance
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        // Detect faces
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(30, 30));

        // Draw rectangles around detected faces
        for (size_t i = 0; i < faces.size(); ++i) {
            cv::rectangle(frame, faces[i], cv::Scalar(0, 255, 0), 2);
            //std::cout << faces[i].x + faces[i].width / 2 - 320 << " " << faces[i].y + faces[i].height / 2 - 240 << std::endl;//
            x_axes = faces[i].x + faces[i].width / 2 - 320;
            y_axes = faces[i].y + faces[i].height / 2 - 240;
        }
        //std::cout << frameWidth << "x" << frameHeight << std::endl;        
        // Show the result
        cv::imshow("Face Detection - Webcam", frame);

        // Break the loop if 'q' is pressed
        if (cv::waitKey(30) == 'q') {
            break;
        }
        //value[0] = x_axes;
        //value[1] = y_axes;

        //send_data = std::to_string(x_axes)+ std::string(" ") + std::to_string(y_axes) + "\n"; // +" " + std::to_string(y_axes) + "\n";
        //std::cout << send_data << "\n";
        string send_data = std::to_string(x_axes) + std::string(" ") + std::to_string(y_axes) + "\n"; // +" " + std::to_string(y_axes) + "\n";
        Serial.WriteSerialPort(send_data);

//if (is_sent) {
        cout << " send data" << send_data << '\n';//do whatever

       // }
    }

    cap.release();
    cv::destroyAllWindows();
    //return x_axes, y_axes;







    
    return 0;
}