#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include "opencv2/opencv.hpp"

static inline unsigned char sat(int i) {
  return (unsigned char)( i >= 255 ? 255 : (i < 0 ? 0 : i));
}

#define IYUYV2RGB_2(pyuv, prgb) { \
    int r = (22987 * ((pyuv)[3] - 128)) >> 14; \
    int g = (-5636 * ((pyuv)[1] - 128) - 11698 * ((pyuv)[3] - 128)) >> 14; \
    int b = (29049 * ((pyuv)[1] - 128)) >> 14; \
    (prgb)[0] = sat(*(pyuv) + r); \
    (prgb)[1] = sat(*(pyuv) + g); \
    (prgb)[2] = sat(*(pyuv) + b); \
    (prgb)[3] = sat((pyuv)[2] + r); \
    (prgb)[4] = sat((pyuv)[2] + g); \
    (prgb)[5] = sat((pyuv)[2] + b); \
    }
#define IYUYV2RGB_16(pyuv, prgb) IYUYV2RGB_8(pyuv, prgb); IYUYV2RGB_8(pyuv + 16, prgb + 24);
#define IYUYV2RGB_8(pyuv, prgb) IYUYV2RGB_4(pyuv, prgb); IYUYV2RGB_4(pyuv + 8, prgb + 12);
#define IYUYV2RGB_4(pyuv, prgb) IYUYV2RGB_2(pyuv, prgb); IYUYV2RGB_2(pyuv + 4, prgb + 6);

void convert(uint8_t *pyuv, uint8_t *prgb, int length) {
  uint8_t *prgb_end = prgb + length;

  while (prgb < prgb_end) {
    IYUYV2RGB_8(pyuv, prgb);

    prgb += 3 * 8;
    pyuv += 2 * 8;
  }
}

using namespace cv;

int main(int argc, char** argv)
{
  int width = 1920;
  int height = 1080;
  int length = width * height * 2;
  FILE *fd = fopen("yavta/frame-000020.bin", "rb");
  void *buf422 = (void *)malloc(length);
  fread(buf422, length, 1, fd);


  uint8_t *rgb = (uint8_t *)malloc(width * height * 3);
  convert((uint8_t *)buf422, (uint8_t *)rgb, width * height * 3);
  Mat rgb_mat(height, width, CV_8UC3, rgb);



//  char *buf444 = (char *)malloc(width * height * 3);
//  for (int i = 0; i < length; i += 4) {
//    char y1 = ((char *)buf422)[i];
//    char u = ((char *)buf422)[i + 1];
//    char y2 = ((char *)buf422)[i + 2];
//    char v = ((char *)buf422)[i + 3];
//    int pos = i / 2 * 3;
//    buf444[pos] = y1;
//    buf444[pos + 1] = u;
//    buf444[pos + 2] = v;
//    buf444[pos + 3] = y2;
//    buf444[pos + 4] = u;
//    buf444[pos + 5] = v;
//  }

//  Mat yuv(height, width, CV_8UC3);
//  for (int i = 0; i < height; ++i) {
//    for (int j = 0; j < width; ++j) {
//      int pos = i * width + j;
//      yuv.at<Vec3b>(i, j).val[0] = buf444[pos];
//      yuv.at<Vec3b>(i, j).val[1] = buf444[pos + 1];
//      yuv.at<Vec3b>(i, j).val[2] = buf444[pos + 2];
//      printf("%d %d %d\n", 
//          yuv.at<uchar>(i, j, 0),
//          yuv.at<uchar>(i, j, 1),
//          yuv.at<uchar>(i, j, 2));
//    }
//  }
//  Mat bgr;
//  cvtColor(yuv, bgr, CV_YCrCb2BGR);

//  Mat gray(height, width, CV_8UC1);
//  for (int i = 0; i < height; ++i) {
//    for (int j = 0; j < width; ++j) {
//      int pos = i * width + j;
//      gray.at<uchar>(i, j) = buf444[pos];
//    }
//  }

//  Mat bgr_new(width, height, CV_8UC3);
//  char *pData = (char *)buf422;
//  for (int i = 0; i < width; ++i) {
//    for (int j = 0; j < height; j += 2) {
//      int pos = 2 * (i * height + j);
//      bgr_new.at<uchar>(i, j, 0) = pData[pos] + pData[pos+3]*((1 - 0.299)/0.615);
//      bgr_new.at<uchar>(i, j, 1) = pData[pos] - pData[pos+1]*((0.114*(1-0.114))/(0.436*0.587)) - pData[pos+3]*((0.299*(1 - 0.299))/(0.615*0.587));
//      bgr_new.at<uchar>(i, j, 2) = pData[pos] + pData[pos+1]*((1 - 0.114)/0.436);
//      bgr_new.at<uchar>(i, j + 1, 0) = pData[pos+2] + pData[pos+3]*((1 - 0.299)/0.615);
//      bgr_new.at<uchar>(i, j + 1, 1) = pData[pos+2] - pData[pos+1]*((0.114*(1-0.114))/(0.436*0.587)) - pData[pos+3]*((0.299*(1 - 0.299))/(0.615*0.587));
//      bgr_new.at<uchar>(i, j + 1, 2) = pData[pos+2] + pData[pos+1]*((1 - 0.114)/0.436);
//    }
//  }

//  Mat bgr_wiki(height, width, CV_8UC3);
//  for (int i = 0; i < height; ++i) {
//    for (int j = 0; j < width; ++j) {
//      printf("(%d, %d)\n", i, j);
//      printf("%d\n", yuv.at<Vec3b>(i, j).val[0]);
//      printf("%d\n", yuv.at<Vec3b>(i, j).val[1]);
//      printf("%d\n", yuv.at<Vec3b>(i, j).val[2]);
//      uchar r = 1 *       yuv.at<Vec3b>(i, j).val[0] +
//                0 *       yuv.at<Vec3b>(i, j).val[1] +
//                1.13983 * yuv.at<Vec3b>(i, j).val[2];
//      uchar g = 1 *        yuv.at<Vec3b>(i, j).val[0] +
//                -0.39465 * yuv.at<Vec3b>(i, j).val[1] +
//                -0.58060 * yuv.at<Vec3b>(i, j).val[2];
//      uchar b = 1 *       yuv.at<Vec3b>(i, j).val[0] +
//                2.03211 * yuv.at<Vec3b>(i, j).val[1] +
//                0 *       yuv.at<Vec3b>(i, j).val[2];
//      bgr_wiki.at<Vec3b>(i, j)[0] = b;
//      bgr_wiki.at<Vec3b>(i, j)[1] = g;
//      bgr_wiki.at<Vec3b>(i, j)[2] = r;
//    }
//  }

  namedWindow("frame");
  imshow("frame", rgb_mat);
  waitKey(0);
//  while (true) {
//    Mat frame;
//    cap >> frame; // get a new frame from camera
//    imshow("frame", frame);
//    if (waitKey(30) >= 0) {
//      break;
//    }
//  }
  free(buf422);
//  free(buf444);
  fclose(fd);
  // the camera will be deinitialized automatically in VideoCapture destructor
  return 0;
}
