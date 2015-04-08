#!/bin/bash

##### Gstreamer 0.10 #####

gst-launch -e videomixer name=mix ! ffmpegcolorspace ! xvimagesink sync=false \
  v4l2src device=/dev/video2 ! video/x-raw-yuv, framerate=30/1, width=320, height=240 ! \
  videobox border-alpha=0 top=0 left=0 ! mix. \
  v4l2src device=/dev/video1 ! video/x-raw-yuv, framerate=30/1, width=320, height=240 ! \
  videobox border-alpha=0 top=0 left=-320 ! mix.

##### Gstreamer 1.0 #####

#gst-launch-1.0 -v -e videomixer name=mix ! videoconvert ! autovideosink sync=false \
#  v4l2src device=/dev/video2 ! video/x-raw, format=RGB, framerate=30/1, width=320, height=240 ! \
#  videobox border-alpha=0 top=0 left=0 ! mix. \
#  v4l2src device=/dev/video1 ! video/x-raw, format=RGB, framerate=30/1, width=320, height=240 ! \
#  videobox border-alpha=0 top=0 left=-320 ! mix.
