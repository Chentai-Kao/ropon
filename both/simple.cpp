#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "opencv2/opencv.hpp"

static inline unsigned char sat(int i) {
  return (unsigned char)(i >= 255 ? 255 : (i < 0 ? 0 : i));
}

#define IYUYV2BGR_2(pyuv, pbgr) { \
  int b = (29049 * ((pyuv)[1] - 128)) >> 14; \
  int g = (-5636 * ((pyuv)[1] - 128) - 11698 * ((pyuv)[3] - 128)) >> 14; \
  int r = (22987 * ((pyuv)[3] - 128)) >> 14; \
  (pbgr)[0] = sat(*(pyuv) + b); \
  (pbgr)[1] = sat(*(pyuv) + g); \
  (pbgr)[2] = sat(*(pyuv) + r); \
  (pbgr)[3] = sat((pyuv)[2] + b); \
  (pbgr)[4] = sat((pyuv)[2] + g); \
  (pbgr)[5] = sat((pyuv)[2] + r); \
}
#define IYUYV2BGR_4(pyuv, pbgr) { \
  IYUYV2BGR_2(pyuv, pbgr); \
  IYUYV2BGR_2(pyuv + 4, pbgr + 6); \
}
#define IYUYV2BGR_8(pyuv, pbgr) { \
  IYUYV2BGR_4(pyuv, pbgr); \
  IYUYV2BGR_4(pyuv + 8, pbgr + 12); \
}
#define IYUYV2BGR_16(pyuv, pbgr) { \
  IYUYV2BGR_8(pyuv, pbgr); \
  IYUYV2BGR_8(pyuv + 16, pbgr + 24); \
}

#define NBUF 8

void convert(uint8_t *pyuv, uint8_t *pbgr, int length)
{
  uint8_t *pbgr_end = pbgr + length;
  while (pbgr < pbgr_end) {
    IYUYV2BGR_8(pyuv, pbgr);
    pbgr += 3 * 8;
    pyuv += 2 * 8;
  }
}

cv::Mat video_to_cv_mat(void *memory, const int height, const int width) {
  cv::Mat m(height, width, CV_8UC3);
  convert((uint8_t *)memory, (uint8_t *)m.data, height * width * 3);
  return m;
}

void control(int fd, int request, void *arg, const char *message) {
  if (-1 == ioctl(fd, request, arg)) {
    fprintf(stderr, "Error ioctl %s\n", message);
    exit(1);
  }
}

int main() {
  const int width = 320, height = 240;

  // Open the capture device.
  int fd = open("/dev/video1", O_RDWR);
  if (-1 == fd) {
    fprintf(stderr, "Error opening video device\n");
    return 1;
  }

  // Query the capture.
  struct v4l2_capability caps;
  control(fd, VIDIOC_QUERYCAP, &caps, "querying capabilities");

  // Image format.
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof fmt);
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = width;
  fmt.fmt.pix.height = height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  control(fd, VIDIOC_S_FMT, &fmt, "setting pixel format");

  // Request buffers.
  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof req);
  req.count = NBUF;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  control(fd, VIDIOC_REQBUFS, &req, "requesting buffer");

  // Query buffer.
  void* memory[NBUF] = { NULL };
  struct v4l2_buffer buf;
  for (int i = 0; i < NBUF; ++i) {
    memset(&buf, 0, sizeof buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    control(fd, VIDIOC_QUERYBUF, &buf, "querying buffer");
    memory[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
        buf.m.offset);
    if ((void *)-1 == memory) {
      fprintf(stderr, "Error on mmap\n");
      return 1;
    }
    control(fd, VIDIOC_QBUF, &buf, "queuing frame");
  }

  // Print framerate.
  struct v4l2_streamparm parm;
  memset(&parm, 0, sizeof parm);
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  control(fd, VIDIOC_G_PARM, &parm, "getting frame rate");
  printf("Current frame rate: %u/%u\n",
      parm.parm.capture.timeperframe.numerator,
      parm.parm.capture.timeperframe.denominator);

  // Capture image.
  control(fd, VIDIOC_STREAMON, &buf.type, "starting capture");
  for (int i = 0; i < 100; ++i) {
    control(fd, VIDIOC_DQBUF, &buf, "retrieving frame");
    cv::Mat m = video_to_cv_mat(memory[buf.index], height, width);
    cv::imshow("frame", m);
    if (cv::waitKey(1) == 'q') {
      break;
    }
    control(fd, VIDIOC_QBUF, &buf, "queuing frame");
  }

  // Stop capture.
  control(fd, VIDIOC_STREAMOFF, &buf.type, "ending capture");
  for (int i = 0; i < NBUF; ++i) {
    if (-1 == munmap(memory[i], buf.length)) {
      fprintf(stderr, "Error on munmap\n");
      return 1;
    }
  }
  if (-1 == close(fd)) {
    fprintf(stderr, "Error closing device\n");
    return 1;
  }
}
