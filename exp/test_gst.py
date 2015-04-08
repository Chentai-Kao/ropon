import gst

pipeline = gst.parse_launch("""
        v4l2src ! decodebin ! ffmpegcolorspace ! pngenc ! filesink location="%s"
        """ % sys.argv[-1])

pipeline.set_state(gst.STATE_PLAYING)
