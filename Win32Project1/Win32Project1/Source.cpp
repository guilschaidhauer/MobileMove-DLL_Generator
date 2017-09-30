#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <thread>

using namespace std;
using namespace cv;

Scalar red;
float highestRadius;
RNG rng(12345);

int iLowH;
int iHighH;

int iLowS;
int iHighS;

int iLowV;
int iHighV;

int iLastX;
int iLastY;

Mat imgTmp;
Mat imgLines;
Mat imgOriginal;

// Declare structure to be used to pass data from C++ to Mono.
struct Circle
{
	Circle(int x, int y, int radius) : X(x), Y(y), Radius(radius) {}
	int X, Y, Radius;
};

CascadeClassifier _faceCascade;
String _windowName = "Unity OpenCV Interop Sample";
VideoCapture _capture;
int _scale = 1;

Circle *outFaces2;

extern "C" void __declspec(dllexport) __stdcall UpdateFaces()
{
	int x = 100;
}

extern "C" void __declspec(dllexport) __stdcall StartThread()
{
	//outFaces2[0] = Circle((int)(rand() % 10 + 1), (int)(rand() % 10 + 1), (int)(rand() % 10 + 1));
	thread first(UpdateFaces);
}

extern "C" int __declspec(dllexport) __stdcall  Init(int& outCameraWidth, int& outCameraHeight)
{
	srand(time(NULL));

	// Load LBP face cascade.
	if (!_faceCascade.load("lbpcascade_frontalface.xml"))
		return -1;

	// Open the stream.
	_capture.open(0);
	if (!_capture.isOpened())
		return -2;

	red = Scalar(0, 0, 255);

	outCameraWidth = _capture.get(CAP_PROP_FRAME_WIDTH);
	outCameraHeight = _capture.get(CAP_PROP_FRAME_HEIGHT);

	//==============================================================
	namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

	iLowH = 23;
	iHighH = 62;

	iLowS = 95;
	iHighS = 255;

	iLowV = 114;
	iHighV = 255;

	//Create trackbars in "Control" window
	createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 179);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);

	iLastX = -1;
	iLastY = -1;
	//==============================================================

	//Capture a temporary image from the camera
	//Mat imgTmp;
	//cap.read(imgTmp);
	_capture.read(imgTmp);

	//Create a black image with the size as the camera output
	imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);

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
	/*Mat frame;
	_capture >> frame;
	if (frame.empty())
		return;*/

	/*//bool bSuccess = _capture.read(imgOriginal); // read a new frame from video

	if (!bSuccess) //if not success, break loop
	{
		cout << "Cannot read a frame from video stream" << endl;
	}

	Mat imgHSV;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

	Mat imgThresholded;
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image //morphological opening (removes small objects from the foreground)

	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//morphological closing (removes small holes from the foreground)
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//Calculate the moments of the thresholded image
	Moments oMoments = moments(imgThresholded);

	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	//imshow("Thresholded Image", imgThresholded); //show the thresholded image

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(imgThresholded, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > 100)
		{
			approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
			boundRect[i] = boundingRect(Mat(contours_poly[i]));
			minEnclosingCircle((Mat)contours_poly[i], center[i], radius[i]);
		}
	}

	//highestRadius = getHighestFloat(&radius);

	/// Draw polygonal contour + bonding rects + circles
	Mat drawing = Mat::zeros(imgThresholded.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		if (contours[i].size() > 100)
		{
			circle(imgOriginal, center[i], (int)radius[i], red, 4, 8, 0);
			circle(imgOriginal, center[i], 5, red, -1);
			outFaces[i] = Circle((int)center[i].x, (int)center[i].y, (int)radius[i]);
			outDetectedFacesCount++;

			if (outDetectedFacesCount == maxOutFacesCount)
				break;
		}
	}

	//cv::flip(imgOriginal, imgOriginal, 1);
	//imshow("Original", imgOriginal);

	/*std::vector<Rect> faces;
	// Convert the frame to grayscale for cascade detection.
	Mat grayscaleFrame;
	cvtColor(frame, grayscaleFrame, COLOR_BGR2GRAY);
	Mat resizedGray;
	// Scale down for better performance.
	resize(grayscaleFrame, resizedGray, Size(frame.cols / _scale, frame.rows / _scale));
	equalizeHist(resizedGray, resizedGray);

	// Detect faces.
	_faceCascade.detectMultiScale(resizedGray, faces);

	// Draw faces.
	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(_scale * (faces[i].x + faces[i].width / 2), _scale * (faces[i].y + faces[i].height / 2));
		ellipse(frame, center, Size(_scale * faces[i].width / 2, _scale * faces[i].height / 2), 0, 0, 360, Scalar(0, 0, 255), 4, 8, 0);

		// Send to application.
		outFaces[i] = Circle(faces[i].x, faces[i].y, faces[i].width / 2);
		outDetectedFacesCount++;

		if (outDetectedFacesCount == maxOutFacesCount)
			break;
	}*/

	// Display debug output.
	//imshow(_windowName, frame);
	//imshow("Original", imgOriginal);

	outFaces[0] = Circle((int)(rand() % 10 + 1), (int)(rand() % 10 + 1), (int)(rand() % 10 + 1));
	//outFaces[0] = outFaces2[0];
}