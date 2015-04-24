#!/usr/bin/python
from v4l2 import *
import fcntl
import mmap
import os
import ctypes
import cv2
import numpy as np
import sys

lib = ctypes.cdll.LoadLibrary('./convert.so')

def clamp(n):
    return max(0, min(255, n))

width = 320
height = 240

fd = os.open('/dev/video1', os.O_RDWR)
caps = v4l2_capability()
fcntl.ioctl(fd, VIDIOC_QUERYCAP, caps)

def set_control(fd, id, value):
    query = v4l2_queryctrl()
    query.id = id
    fcntl.ioctl(fd, VIDIOC_QUERYCTRL, query)

    ctrl = v4l2_ext_control()
    ctrl.id = query.id
    ctrl.value = value
    ctrl.value64 = value
    ctrls = v4l2_ext_controls()
    ctrls.ctrl_class = V4L2_CTRL_ID2CLASS(query.id)
    ctrls.count = 1
    ctrls.controls = ctypes.pointer(ctrl)
    print id
    try:
        fcntl.ioctl(fd, VIDIOC_S_CTRL, ctypes.pointer(ctrl))
    except:
        print 'except'
        fcntl.ioctl(fd, VIDIOC_S_EXT_CTRLS, ctypes.pointer(ctrls))
    #fcntl.ioctl(fd, VIDIOC_G_EXT_CTRLS, ctypes.pointer(ctrls))
    #print query.name, ctrls.controls.contents.value, ctrls.controls.contents.value64

set_control(fd, 0x009a0901, 0)
set_control(fd, 0x009a0902, 200)

# BUG here: the v4l2.py is outdated, not including the value.
# consider setting value directly.

#sys.exit(0)

fmt = v4l2_format()
fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE
fmt.fmt.pix.width = width
fmt.fmt.pix.height = height
fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV
fmt.fmt.pix.field = V4L2_FIELD_NONE
fcntl.ioctl(fd, VIDIOC_S_FMT, fmt)

req = v4l2_requestbuffers()
req.count = 1
req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE
req.memory = V4L2_MEMORY_MMAP
fcntl.ioctl(fd, VIDIOC_REQBUFS, req)

buf = v4l2_buffer()
buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE
buf.memory = V4L2_MEMORY_MMAP
buf.index = 0
fcntl.ioctl(fd, VIDIOC_QUERYBUF, buf)

mem = mmap.mmap(fd, buf.length, offset=buf.m.offset)

fcntl.ioctl(fd, VIDIOC_STREAMON, ctypes.c_int(buf.type))

while True:
    fcntl.ioctl(fd, VIDIOC_QBUF, buf)
    fcntl.ioctl(fd, VIDIOC_DQBUF, buf)

    yuyv = np.ndarray(shape=(height, width, 2), dtype=np.uint8, buffer=mem)
    bgr = np.ndarray(shape=(height, width, 3), dtype=np.uint8)
    lib.convert(ctypes.c_char_p(yuyv.ctypes.data),
                ctypes.c_char_p(bgr.ctypes.data),
                height * width * 3)

    #m = cv2.cv.fromarray(array)
    cv2.imshow('frame', bgr)
    if cv2.waitKey(1) == 113: # 'q' key
        break
