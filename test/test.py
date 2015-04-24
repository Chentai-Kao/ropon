import ctypes as c
import cv2
import numpy as np
import sys
import time

lib = c.cdll.LoadLibrary('./grab.so')
nplib = np.ctypeslib.load_library('grab.so', '.')
nplib.get_bgr_mem.restype = c.POINTER(c.c_ubyte)
nplib.set_bgr_mem.argtypes = [c.POINTER(c.POINTER(c.c_ubyte))]
lib.copy_bgr_mem.argtypes = [np.ctypeslib.ndpointer(c.c_ubyte, flags="C_CONTIGUOUS"),
                             c.c_size_t]

argvs = [] # keep argv variables and its memory when running main(argc, argv)

def run_camera(device, width, height):
    command = './yavta -l %s -s %dx%d --capture=100' % (device, width, height)
    args = command.split()
    # construct argv
    argc = len(args)
    argv = (c.c_char_p * argc)()
    for i in xrange(argc):
        argv[i] = c.c_char_p(args[i])
    argvs.append(argv)
    # call main(argc, argv)
    lib.main(c.c_int(argc), c.pointer(argv))

width = 320
height = 240

run_camera('/dev/video1', width, height)

time.sleep(1)




############### one approach
bgr = np.ndarray(shape=(height, width, 3), dtype=np.uint8)

for i in xrange(1000):
    print i
    lib.query_frame()
    lib.copy_bgr_mem(bgr, bgr.size)
    cv2.imshow("test", bgr)
    if cv2.waitKey(1) == 113: # 'q' key
        break



############### another approach
bgr = np.ndarray(shape=(height, width, 3), dtype=np.uint8)
mem = bgr.ctypes.data_as(c.POINTER(c.c_ubyte))
nplib.set_bgr_mem(c.byref(mem))

for i in xrange(1000):
    print i
    lib.query_frame()
    a = np.asarray(mem[:width * height * 3]).reshape((height, width, 3)).astype(np.uint8)
    cv2.imshow("test", a)
    if cv2.waitKey(1) == 113: # 'q' key
        break

lib.request_stop()
