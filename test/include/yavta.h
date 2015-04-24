#include "opencv2/opencv.hpp"

struct arg_struct {
  int argc;
  char **argv;
};

cv::Mat video_to_cv_mat();
void request_stop();
void *init(void *arg_struct);
