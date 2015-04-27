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

cv::Mat getMat(void *memory, const int height, const int width) {
  cv::Mat m(height, width, CV_8UC3);
  convert((uint8_t *)memory, (uint8_t *)m.data, height * width * 3);
  return m;
}

void setControl(int fd, int request, void *arg, const char *message) {
  if (-1 == ioctl(fd, request, arg)) {
    fprintf(stderr, "Error ioctl %s\n", message);
    exit(1);
  }
}

void setParameter(int fd, int id, const unsigned int value,
    const char *message) {
  // Set up the control
  struct v4l2_ext_control ctrl;
  memset(&ctrl, 0, sizeof(ctrl));
  ctrl.id = id;
  ctrl.value64 = value;
  // Wrap up and send to ioctl
  struct v4l2_ext_controls ctrls;
  memset(&ctrls, 0, sizeof(ctrls));
  ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(id);
  ctrls.count = 1;
  ctrls.controls = &ctrl;
  setControl(fd, VIDIOC_S_EXT_CTRLS, &ctrls, message);
}

int main() {
  const int width = 800, height = 448; // widest frame with 30 fps

  // Open the capture device.
  int fd = open("/dev/video1", O_RDWR);
  if (-1 == fd) {
    fprintf(stderr, "Error opening video device\n");
    return 1;
  }

  // Query the capture.
  struct v4l2_capability caps;
  setControl(fd, VIDIOC_QUERYCAP, &caps, "querying capabilities");

  // Image format.
  struct v4l2_format fmt;
  memset(&fmt, 0, sizeof fmt);
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  fmt.fmt.pix.width = width;
  fmt.fmt.pix.height = height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
  fmt.fmt.pix.field = V4L2_FIELD_NONE;
  setControl(fd, VIDIOC_S_FMT, &fmt, "setting pixel format");

  // Request buffers.
  struct v4l2_requestbuffers req;
  memset(&req, 0, sizeof req);
  req.count = NBUF;
  req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory = V4L2_MEMORY_MMAP;
  setControl(fd, VIDIOC_REQBUFS, &req, "requesting buffer");

  // Query buffer.
  void* memory[NBUF] = { NULL };
  struct v4l2_buffer buf;
  for (int i = 0; i < NBUF; ++i) {
    memset(&buf, 0, sizeof buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;
    setControl(fd, VIDIOC_QUERYBUF, &buf, "querying buffer");
    memory[i] = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
        buf.m.offset);
    if ((void *)-1 == memory) {
      fprintf(stderr, "Error on mmap\n");
      return 1;
    }
    setControl(fd, VIDIOC_QBUF, &buf, "queuing frame");
  }

  // Print framerate.
  struct v4l2_streamparm parm;
  memset(&parm, 0, sizeof parm);
  parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  parm.parm.capture.timeperframe.numerator = 1;
  parm.parm.capture.timeperframe.denominator = 30;
  setControl(fd, VIDIOC_S_PARM, &parm, "getting frame rate");

  // Set parameters.
  setParameter(fd, 0x00980900, 100, "brightness"); // 0 ~ 255
  setParameter(fd, 0x009a0901, 1, "exposure auto"); // 1. manual 3. auto
  setParameter(fd, 0x009a0902, 150, "exposure (absolute)"); // 3 ~ 2047
  setParameter(fd, 0x0098090c, 0, "white balance auto"); // 0 or 1
  setParameter(fd, 0x0098091a, 4500, "white balance (absolute)"); // 2800 ~ 6500
  setParameter(fd, 0x0098091c, 0, "backlight compensation"); // 0 or 1
  setParameter(fd, 0x009a090c, 0, "focus auto"); // 0 or 1
  setParameter(fd, 0x009a090a, 0, "focus (absolute)"); // 0 ~ 255
  setParameter(fd, 0x0098091b, 100, "sharpness"); // 0 ~ 255
  setParameter(fd, 0x009a090d, 1, "zoom (absolute)"); // (wide) 1 ~ 5 (near)

  // Capture image.
  setControl(fd, VIDIOC_STREAMON, &buf.type, "starting capture");
  while (true) {
    setControl(fd, VIDIOC_DQBUF, &buf, "retrieving frame");
    cv::Mat m = getMat(memory[buf.index], height, width);
    cv::imshow("frame", m);
    if (cv::waitKey(1) == 'q') {
      break;
    }
    setControl(fd, VIDIOC_QBUF, &buf, "queuing frame");
  }

  // Stop capture.
  setControl(fd, VIDIOC_STREAMOFF, &buf.type, "ending capture");
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
