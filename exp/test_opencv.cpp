#include <cstdio>
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int argc, char** argv)
{
  int width = 320, height = 240; // dimension of a single camera

  char gstPipeline[512]; // GStreamer pipeline
  sprintf(gstPipeline,
      "videomixer name=mix ! ffmpegcolorspace ! appsink sync=false " // mixer
      "v4l2src device=/dev/video2 ! " // first device
      "video/x-raw-yuv, framerate=30/1, width=%d, height=%d ! "
      "videobox border-alpha=0 top=0 left=0 ! mix. "
      "v4l2src device=/dev/video1 ! " // second device
      "video/x-raw-yuv, framerate=30/1, width=%d, height=%d ! "
      "videobox border-alpha=0 top=0 left=-%d ! mix.",
      width, height, width, height, width);
  VideoCapture cap(gstPipeline); // open webcam

  if (!cap.isOpened())  // check if we succeeded
    return -1;

  namedWindow("frame", 1);
  while (true) {
    Mat frame;
    cap >> frame; // get a new frame from camera
    imshow("frame", frame);
    if (waitKey(30) >= 0) {
      break;
    }
  }
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
