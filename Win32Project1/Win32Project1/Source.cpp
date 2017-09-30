#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

// Declare structure to be used to pass data from C++ to Mono.
struct Circle
{
	Circle(int x, int y, int radius) : X(x), Y(y), Radius(radius) {}
	int X, Y, Radius;
};

Scalar red;
float highestRadius;
RNG rng(12345);

Mat imgTmp;
Mat imgLines;

CascadeClassifier _faceCascade;
String _windowName = "Unity OpenCV Interop Sample";
VideoCapture _capture;
int _scale = 1;

extern "C" int __declspec(dllexport) __stdcall  Init(int& outCameraWidth, int& outCameraHeight)
{
	// Load LBP face cascade.
	if (!_faceCascade.load("lbpcascade_frontalface.xml"))
		return -1;

	// Open the stream.
	_capture.open(0);
	if (!_capture.isOpened())
		return -2;

	outCameraWidth = _capture.get(CAP_PROP_FRAME_WIDTH);
	outCameraHeight = _capture.get(CAP_PROP_FRAME_HEIGHT);

	//============================================================================

	red = Scalar(0, 0, 255);

	if (!_capture.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	//namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	int iLowH = 22;
	int iHighH = 156;

	int iLowS = 30;
	int iHighS = 255;

	int iLowV = 203;
	int iHighV = 255;

	//Create trackbars in "Control" window
	/*createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);*/

	int iLastX = -1;
	int iLastY = -1;

	//Capture a temporary image from the camera
	imgTmp;
	_capture.read(imgTmp);

	//Create a black image with the size as the camera output
	imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall  Close()
{
	_capture.release();
}

extern "C" void __declspec(dllexport) __stdcall SetScale(int scale)
{
	_scale = scale;
}

extern "C" void __declspec(dllexport) __stdcall Detect(Circle* outFaces, int maxOutFacesCount, int& outDetectedFacesCount)
{
	Mat frame;
	_capture >> frame;
	if (frame.empty())
		return;

	outFaces[0] = Circle(99, 99, 99);

	// Display debug output.
	//imshow(_windowName, frame);
}