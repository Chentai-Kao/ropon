import time
from ctypes import *

lib = cdll.LoadLibrary('./grab.so')

#arguments = './yavta -l /dev/video1'
arguments = './yavta -l /dev/video1 -s 320x240 --capture=100'
args = arguments.split()
argv = (c_char_p * len(args))()
for i in xrange(len(args)):
    argv[i] = c_char_p(args[i])
lib.main(c_int(len(argv)), pointer(argv))

time.sleep(1)
lib.request_stop()
