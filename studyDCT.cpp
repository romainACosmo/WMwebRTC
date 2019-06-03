#include <stdio.h>
#include <string>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*
Problem: to choose the blocks to embed using rank of AC1 coefficient is not robust (even conversion to 8UC1 breaks it)
*/

double collect(Mat input, int mode);
void analyse(double input[], int length, bool output[], int nb_sel);
void toMax(Mat input);

void studyDCT(Mat input, Mat output, int mode, int nb_sel)
{
  // divide the bgr matrix of the image into the 3 channels blue, green and red
  Mat bgr[3];
  split(input,bgr);
  // convert the blue channel to be compatible with the opencv dct function input type
  Mat blueFloat;
  bgr[0].convertTo(blueFloat,CV_32FC1);

  int count = 0;
  double values[4096] = {0.0};
  bool selected[4096] = {false};

  for (int i = 0; i < blueFloat.cols; i += 8){
    for (int j = 0; j < blueFloat.rows; j += 8){
      Mat tmp = blueFloat(Rect(i, j, 8, 8));
      dct(tmp, tmp);
      values[count] = collect(tmp, mode);
      idct(tmp, tmp);
      ++count;
    }
  }

  analyse(values, 4096, selected, nb_sel);

  count = 0;
  for(int i = 0; i < 4096; ++i){
    Mat tmp = blueFloat(Rect((i/64)*8, (i%64)*8, 8, 8));
    if(selected[i]){
      // printf("%s %d, index: (%d,%d)\n", "Selected", i, 8*(i/64), 8*(i%64));
      toMax(tmp);
    }
  }

  blueFloat.convertTo(bgr[0],CV_8UC1);
  std::vector<Mat> channels;
  channels.push_back(bgr[0]);
  channels.push_back(bgr[1]);
  channels.push_back(bgr[2]);
  merge(channels, output);
}

double collect(Mat input, int mode){
  switch (mode) {
    case 1:
      return abs(input.at<float>(0,1));
    case 2:
      double vec[8];
      blk2vec(input, vec);
      return absSum(vec, 8);
    case 3:
      return countNZZ(input); // depends a lot of the picture
    default:
      return abs(input.at<float>(0,1));
  }
  // cout << input << endl;
}

void analyse(double input[], const int length, bool output[], int nb_sel){
  double copy[length];
  for(int i = 0; i < length; ++i)
    copy[i] = input[i];
  std::sort(copy, copy+length, std::greater<double>());
  double minSelected = copy[nb_sel];
  printf("%f\n", minSelected);
  for(int i = 0; i < length; ++i)
    output[i] = input[i] >= minSelected;
}

void toMax(Mat input){
  for(int i = 0; i < input.cols; ++i){
    for(int j = 0; j < input.rows; ++j){
      input.at<float>(i,j) = 255;
    }
  }
}
