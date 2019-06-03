#include <stdio.h>
#include <string>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <opencv2/opencv.hpp>
// #include "helpers.cpp"

using namespace cv;

#define OBSX1 0
#define OBSY1 16

/*
Problem: only embed the firsts bits => not optimal
*/
void embedV1(double orig[8], int wm[4], double res[8], double alpha);

void wmV1(Mat input, int wm[], int wmLength, Mat output, double alpha)
{
  // divide the bgr matrix of the image into the 3 channels blue, green and red
  Mat bgr[3];

  split(input,bgr);

  // start the timer
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
  // std::cout << bgr[0].size() << std::endl;
  // convert the blue channel to be compatible with the opencv dct function input type
  Mat blueFloat;
  bgr[0].convertTo(blueFloat,CV_32FC1);

  int count = 0;
  for (int i = 0; i < blueFloat.cols; i += 8){
    for (int j = 0; j < blueFloat.rows; j += 8){
      if(count == wmLength)
        break;
      Mat dctTmp = blueFloat(Rect(i, j, 8, 8));
      dct(dctTmp, dctTmp);
      double vec[8] = {0};
      blk2vec(dctTmp, vec);
      double vecWM[8] = {0};
      embedV1(vec, &wm[count], vecWM, alpha);
      vec2blk(vecWM, dctTmp);
      idct(dctTmp, dctTmp);
      // if(i == OBSX1 && j == OBSY1){
      //   std::cout << i << " - " << j << std::endl;
      //   printArray(&wm[count], 4);
      //   printArray(vecWM, 8);
      //   // imshow("Orig", bgr[0](Rect(i, j, 8, 8)));
      //   // waitKey(0);
      // }
      count += 4;
    }
  }
  // std::cout << blueFloat(Rect(0, 16, 8, 8)) << std::endl;

  // 0, 0, 1, 1,
  // 4.887169	0.246518	0.257000	8.190679	-10.690272	6.715621	-11.262077	2.520316
  // -6.433157	11.566843	-4.776161	13.223839	7.012674	-10.987326	4.629120	-13.370880

  blueFloat.convertTo(bgr[0],CV_8UC1);

  std::vector<Mat> channels;
  channels.push_back(bgr[0]);
  channels.push_back(bgr[1]);
  channels.push_back(bgr[2]);

  merge(channels, output);

  // print the execution time
  std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  std::cout << time_span.count() << ",";
}


void embedV1(double orig[8], int wm[4], double res[8], double alpha){
  for(int i = 0; i<4; ++i){
    res[2*i] = alpha*(2*wm[i]-1)+(orig[2*i]+orig[2*i+1])/2;
    res[2*i+1] = res[2*i] - alpha*(2*wm[i]-1)*2;
  }
}


void exV1(Mat input, int wm[], int len){
  // divide the bgr matrix of the image into the 3 channels blue, green and red

  Mat bgr[3];
  split(input,bgr);

  // start the timer
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
  // convert the blue channel to be compatible with the opencv dct function input type
  Mat blueFloat;
  bgr[0].convertTo(blueFloat,CV_32FC1);

  int count = 0;
  for (int i = 0; i < blueFloat.cols; i += 8){
    for (int j = 0; j < blueFloat.rows; j += 8){
      if(count >= len)
        break;
      Mat dctTmp = blueFloat(Rect(i, j, 8, 8));
      // if(i == OBSX1 && j == OBSY1){
      //   std::cout << i << " - "<< j << std::endl;
      //   std::cout << blueFloat(Rect(i, j, 8, 8)) << std::endl;
        // imshow("Extracted", bgr[0](Rect(i, j, 32, 32)));
        // waitKey(0);
      // }
      dct(dctTmp, dctTmp);
      double vec[8] = {0};
      blk2vec(dctTmp, vec);
      double vecWM[8] = {0};
      extract(vec, &wm[count]);
      // if(i == OBSX1 && j == OBSY1){
      //   printArray(vec, 8);
      //   printArray(&wm[count], 4);
      // }
      count += 4;
    }
  }

  // print the execution time
  std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  std::cout << time_span.count() << ",";
}
