#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libuvc/libuvc.h"

int cb_count = 0;

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void cb(uvc_frame_t *frame, void *ptr) {
  uvc_frame_t *bgr;
  uvc_error_t ret;
  FILE *fp = (FILE *)ptr;

  printf("callback count: %d\n", cb_count++);

  /* We'll convert the image from YUV/JPEG to BGR, so allocate space */
  bgr = uvc_allocate_frame(frame->width * frame->height * 3);
  if (!bgr) {
    printf("unable to allocate bgr frame!");
    return;
  }

  /* Do the BGR conversion */
  ret = uvc_any2bgr(frame, bgr);
  if (ret) {
    uvc_perror(ret, "uvc_any2bgr");
    uvc_free_frame(bgr);
    return;
  }

  /* Save to file */
//  for (int i = 0; i < bgr->width * bgr->height * 3; ++i) {
//    fprintf(fp, "%d", bgr->data[i]);
//  }

  /* Call a user function:
   *
   * my_type *my_obj = (*my_type) ptr;
   * my_user_function(ptr, bgr);
   * my_other_function(ptr, bgr->data, bgr->width, bgr->height);
   */

  /* Call a C++ method:
   *
   * my_type *my_obj = (*my_type) ptr;
   * my_obj->my_func(bgr);
   */

  /* Use opencv.highgui to display the image:
   * 
   * cvImg = cvCreateImageHeader(
   *     cvSize(bgr->width, bgr->height),
   *     IPL_DEPTH_8U,
   *     3);
   *
   * cvSetData(cvImg, bgr->data, bgr->width * 3); 
   *
   * cvNamedWindow("Test", CV_WINDOW_AUTOSIZE);
   * cvShowImage("Test", cvImg);
   * cvWaitKey(10);
   *
   * cvReleaseImageHeader(&cvImg);
   */

  uvc_free_frame(bgr);

  printf("callback ends\n");
}

void cb0(uvc_frame_t *frame, void *ptr) {
}

void cb1(uvc_frame_t *frame, void *ptr) {
}

/* List all devices */
void list_devices(uvc_context_t *ctx) {
  uvc_error_t ret;
  uvc_device_t **list;
  ret = uvc_get_device_list(ctx, &list);
  if (ret != UVC_SUCCESS) {
    uvc_perror(ret, "uvc_get_device_list");
    return;
  }
  puts("Listing devices");
  uvc_device_t *test_dev;
  int dev_idx = 0;
  while ((test_dev = list[dev_idx++]) != NULL) {
    uvc_device_descriptor_t *desc;
    if (uvc_get_device_descriptor(test_dev, &desc) != UVC_SUCCESS)
      continue;
    printf("(found) vid: %d, pid: %d, serial: %s\n",
           desc->idVendor, desc->idProduct, desc->serialNumber);
    uvc_free_device_descriptor(desc);
  }
  uvc_free_device_list(list, 1);
}

/* Run both streaming (FAIL to start the second streaming) */
int both() {
  uvc_context_t *ctx0, *ctx1;
  uvc_error_t ret0, ret1;

  /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
  ret0 = uvc_init(&ctx0, NULL);
  ret1 = uvc_init(&ctx1, NULL);

  if (ret0 < 0 || ret1 < 0) {
    uvc_perror(ret0, "uvc_init");
    uvc_perror(ret1, "uvc_init");
    return -1;
  }

  puts("UVC initialized");

  /* List all devices. */
  list_devices(ctx0);

  /* Locates both UVC devices, stores in dev.
   *   (Logitech) vid: 1133, pid: 2092, serial: 0C188C90
   *   (Logitech) vid: 1133, pid: 2092, serial: 22206950
   */
  uvc_device_t *dev0, *dev1;
  uvc_device_handle_t *devh0, *devh1;
  uvc_stream_ctrl_t ctrl0, ctrl1;
  ret0 = uvc_find_device(ctx0, &dev0, 1133, 2092, "0C188C90");
  ret1 = uvc_find_device(ctx1, &dev1, 1133, 2092, "22206950");

  if (ret0 < 0 || ret1 < 0) {
    /* no devices found */
    uvc_perror(ret0, "uvc_find_device");
    uvc_perror(ret1, "uvc_find_device");
  } else {
    puts("Device found");

    /* Try to open the device: requires exclusive access */
    ret0 = uvc_open(dev0, &devh0);
    ret1 = uvc_open(dev1, &devh1);

    if (ret0 < 0 || ret1 < 0) {
      /* unable to open device */
      uvc_perror(ret0, "uvc_open");
      uvc_perror(ret1, "uvc_open");
    } else {
      puts("Device opened");

      /* Print out a message containing all the information that libuvc
       * knows about the device
       */
//      uvc_print_diag(devh0, stderr);
//      uvc_print_diag(devh1, stderr);

      /* Try to negotiate a 640x480 30 fps YUYV stream profile */
      ret0 = uvc_get_stream_ctrl_format_size(
          devh0, &ctrl0, /* result stored in ctrl */
          UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
          640, 480, 30 /* width, height, fps */
      );
      ret1 = uvc_get_stream_ctrl_format_size(
          devh1, &ctrl1, /* result stored in ctrl */
          UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
          640, 480, 30 /* width, height, fps */
      );

      /* Print out the result */
//      uvc_print_stream_ctrl(&ctrl0, stderr);
//      uvc_print_stream_ctrl(&ctrl1, stderr);

      if (ret0 < 0 || ret1 < 0) {
        /* no matching stream provided by device */
        uvc_perror(ret0, "get_mode");
        uvc_perror(ret1, "get_mode");
      } else {
        /* Start the video stream.
         * The library will call user function as cb(frame, (void*) 12345)
         */
        ret0 = uvc_start_streaming(devh0, &ctrl0, cb0, (void*) 12345, 0);
//        ret1 = uvc_start_streaming(devh1, &ctrl1, cb1, (void*) 12345, 0);
        printf("Start streaming ret0: %d, ret1: %d.", ret0, ret1);

        if (ret0 < 0 || ret1 < 0) {
          /* unable to start stream */
          uvc_perror(ret0, "start_streaming");
          uvc_perror(ret1, "start_streaming");
        } else {
          puts("Streaming for 10 seconds...");
//          uvc_error_t resAEMODE0 = uvc_set_ae_mode(devh0, 1);
//          uvc_perror(resAEMODE0, "set_ae_mode");
//          uvc_error_t resAEMODE1 = uvc_set_ae_mode(devh1, 1);
//          uvc_perror(resAEMODE1, "set_ae_mode");
//          int i;
//          for (i = 1; i <= 10; i++) {
//            uvc_error_t resEXP = uvc_set_exposure_abs(devh, 20 + i);
//            uvc_perror(resEXP, "set_exp_abs");
//            sleep(1);
//          }
          sleep(1);

          /* End the stream. Blocks until last callback is serviced */
          puts("Stop streaming.");
          uvc_stop_streaming(devh0);
//          uvc_stop_streaming(devh1);
          puts("Done streaming.");
        }
      }

      /* Release our handle on the device */
      uvc_close(devh0);
      uvc_close(devh1);
      puts("Device closed");
    }

    /* Release the device descriptor */
    uvc_unref_device(dev0);
    uvc_unref_device(dev1);
  }

  /* Close the UVC context.
   * This closes and cleans up any existing device handles,
   * and it closes the libusb context if one was not provided. */
  uvc_exit(ctx0);
  uvc_exit(ctx1);
  puts("UVC exited");

  return 0;
}

void single(void *arg) {
  uvc_context_t *ctx;
  uvc_device_t *dev;
  uvc_device_handle_t *devh;
  uvc_stream_ctrl_t ctrl;
  uvc_error_t res;
  int webcam = (int)arg;

  /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
  res = uvc_init(&ctx, NULL);

  if (res < 0) {
    uvc_perror(res, "uvc_init");
    return;
  }

  puts("UVC initialized");

  /* Locates the first attached UVC device, stores in dev */
  //res = uvc_find_device(ctx, &dev, 1133, 2092, webcamSerial);
  if (webcam == 0)
    res = uvc_find_device(ctx, &dev, 1133, 2092, "0C188C90");
  else
    res = uvc_find_device(ctx, &dev, 1133, 2092, "22206950");

  if (res < 0) {
    uvc_perror(res, "uvc_find_device"); /* no devices found */
  } else {
    puts("Device found");

    /* Try to open the device: requires exclusive access */
    res = uvc_open(dev, &devh);

    if (res < 0) {
      uvc_perror(res, "uvc_open"); /* unable to open device */
    } else {
      puts("Device opened");

      /* Print out a message containing all the information that libuvc
       * knows about the device */
//      uvc_print_diag(devh, stderr);

      /* Try to negotiate a 640x480 30 fps YUYV stream profile */
      res = uvc_get_stream_ctrl_format_size(
          devh, &ctrl, /* result stored in ctrl */
          UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
          640, 480, 30 /* width, height, fps */
      );

      /* Print out the result */
//      uvc_print_stream_ctrl(&ctrl, stderr);

      if (res < 0) {
        uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
      } else {
        /* Start the video stream. The library will call user function cb:
         *   cb(frame, (void*) 12345)
         */
        if (webcam == 0)
          res = uvc_start_streaming(devh, &ctrl, cb0, (void *)12345, 0);
        else
          res = uvc_start_streaming(devh, &ctrl, cb1, (void *)12345, 0);

        if (res < 0) {
          uvc_perror(res, "start_streaming"); /* unable to start stream */
        } else {
          puts("Streaming...");

          uvc_set_ae_mode(devh, 1); /* e.g., turn on auto exposure */

          sleep(1); /* stream for 10 seconds */

          /* End the stream. Blocks until last callback is serviced */
          uvc_stop_streaming(devh);
          puts("Done streaming.");
        }
      }

      /* Release our handle on the device */
      uvc_close(devh);
      puts("Device closed");
    }

    /* Release the device descriptor */
    uvc_unref_device(dev);
  }

  /* Close the UVC context. This closes and cleans up any existing device handles,
   * and it closes the libusb context if one was not provided. */
  uvc_exit(ctx);
  puts("UVC exited");
}

void single_fork() {
  uvc_context_t *ctx;
  uvc_device_t *dev;
  uvc_device_handle_t *devh;
  uvc_stream_ctrl_t ctrl;
  uvc_error_t res;

  pid_t pId = fork();
  int webcam = (pId == 0)? 0 : 1;

  /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
  res = uvc_init(&ctx, NULL);

  if (res < 0) {
    uvc_perror(res, "uvc_init");
    return;
  }

  puts("UVC initialized");

  /* Locates the first attached UVC device, stores in dev */
  //res = uvc_find_device(ctx, &dev, 1133, 2092, webcamSerial);
  if (webcam == 0)
    res = uvc_find_device(ctx, &dev, 1133, 2092, "0C188C90");
  else
    res = uvc_find_device(ctx, &dev, 1133, 2092, "22206950");

  if (res < 0) {
    uvc_perror(res, "uvc_find_device"); /* no devices found */
  } else {
    puts("Device found");

    /* Try to open the device: requires exclusive access */
    res = uvc_open(dev, &devh);

    if (res < 0) {
      uvc_perror(res, "uvc_open"); /* unable to open device */
    } else {
      puts("Device opened");

      /* Print out a message containing all the information that libuvc
       * knows about the device */
//      uvc_print_diag(devh, stderr);

      /* Try to negotiate a 640x480 30 fps YUYV stream profile */
      res = uvc_get_stream_ctrl_format_size(
          devh, &ctrl, /* result stored in ctrl */
          UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
          640, 480, 30 /* width, height, fps */
      );

      /* Print out the result */
//      uvc_print_stream_ctrl(&ctrl, stderr);

      if (res < 0) {
        uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
      } else {
        /* Start the video stream. The library will call user function cb:
         *   cb(frame, (void*) 12345)
         */
        if (webcam == 0)
          res = uvc_start_streaming(devh, &ctrl, cb0, (void *)12345, 0);
        else
          res = uvc_start_streaming(devh, &ctrl, cb1, (void *)12345, 0);

        if (res < 0) {
          uvc_perror(res, "start_streaming"); /* unable to start stream */
        } else {
          puts("Streaming...");

          uvc_set_ae_mode(devh, 1); /* e.g., turn on auto exposure */

          sleep(1); /* stream for 10 seconds */

          /* End the stream. Blocks until last callback is serviced */
          uvc_stop_streaming(devh);
          puts("Done streaming.");
        }
      }

      /* Release our handle on the device */
      uvc_close(devh);
      puts("Device closed");
    }

    /* Release the device descriptor */
    uvc_unref_device(dev);
  }

  /* Close the UVC context. This closes and cleans up any existing device handles,
   * and it closes the libusb context if one was not provided. */
  uvc_exit(ctx);
  puts("UVC exited");
}

void single_write(int webcam) {
  uvc_context_t *ctx;
  uvc_device_t *dev;
  uvc_device_handle_t *devh;
  uvc_stream_ctrl_t ctrl;
  uvc_error_t res;

  /* Initialize a UVC service context. Libuvc will set up its own libusb
   * context. Replace NULL with a libusb_context pointer to run libuvc
   * from an existing libusb context. */
  res = uvc_init(&ctx, NULL);

  if (res < 0) {
    uvc_perror(res, "uvc_init");
    return;
  }

  puts("UVC initialized");

  /* Locates the first attached UVC device, stores in dev */
  //res = uvc_find_device(ctx, &dev, 1133, 2092, webcamSerial);
  res = uvc_find_device(ctx, &dev, 1133, 2092,
                        (webcam == 0)? "0C188C90" : "22206950");

  if (res < 0) {
    uvc_perror(res, "uvc_find_device"); /* no devices found */
  } else {
    puts("Device found");

    /* Try to open the device: requires exclusive access */
    res = uvc_open(dev, &devh);

    if (res < 0) {
      uvc_perror(res, "uvc_open"); /* unable to open device */
    } else {
      puts("Device opened");

      /* Print out a message containing all the information that libuvc
       * knows about the device */
//      uvc_print_diag(devh, stderr);

      /* Try to negotiate a 640x480 30 fps YUYV stream profile */
      res = uvc_get_stream_ctrl_format_size(
          devh, &ctrl, /* result stored in ctrl */
          UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
          640, 480, 30 /* width, height, fps */
      );

      /* Print out the result */
//      uvc_print_stream_ctrl(&ctrl, stderr);

      if (res < 0) {
        uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
      } else {
        /* Start the video stream. The library will call user function cb:
         *   cb(frame, (void*) 12345)
         */
        res = uvc_start_streaming(devh, &ctrl, cb, (void *)&webcam, 0);

        if (res < 0) {
          uvc_perror(res, "start_streaming"); /* unable to start stream */
        } else {
          puts("Streaming...");

          uvc_set_ae_mode(devh, 1); /* e.g., turn on auto exposure */

          sleep(10); /* stream for 10 seconds */

          /* End the stream. Blocks until last callback is serviced */
          uvc_stop_streaming(devh);
          puts("Done streaming.");
        }
      }

      /* Release our handle on the device */
      uvc_close(devh);
      puts("Device closed");
    }

    /* Release the device descriptor */
    uvc_unref_device(dev);
  }

  /* Close the UVC context. This closes and cleans up any existing device handles,
   * and it closes the libusb context if one was not provided. */
  uvc_exit(ctx);
  puts("UVC exited");
}

int main(int argc, char **argv) {
  //return both();

  //pthread_t t0, t1;
  //pthread_create(&t0, NULL, (void *)&single, (void *)"0C188C90");
  //pthread_create(&t1, NULL, (void *)&single, (void *)"22206950");
  //pthread_create(&t0, NULL, (void *)&single, (void *)0);
  //pthread_create(&t1, NULL, (void *)&single, (void *)1);
  //pthread_join(t0, NULL);
  //pthread_join(t1, NULL);

  //single_fork();

  //single_write(0);
  single_write(1);
}
