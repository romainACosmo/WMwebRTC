#include <stdio.h>
#include <string>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <opencv2/opencv.hpp>
// #include "helpers.cpp"

using namespace cv;

#define OBSX 0
#define OBSY 224
#define WM

/*
Problem: to choose the blocks to embed using rank of AC1 coefficient is not robust (even conversion to 8UC1 breaks it)
*/

void embedV2(double orig[8], int wm[4], double res[8], double alpha);

void wmV2(Mat input, int wm[], int wmLength, Mat output, double alpha)
{
  // divide the bgr matrix of the image into the 3 channels blue, green and red
  Mat bgr[3];
  split(input,bgr);

  // start the timer
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

  // convert the blue channel to be compatible with the opencv dct function input type
  Mat blueFloat;
  bgr[0].convertTo(blueFloat,CV_32FC1);

  int count = 0;
  double highAC[4096] = {0};

  for (int i = 0; i < blueFloat.rows; i += 8){
    for (int j = 0; j < blueFloat.cols; j += 8){
      Mat dctTmp = blueFloat(Rect(i, j, 8, 8));
      dct(dctTmp, dctTmp);
      // double vec[8] = {0};
      // blk2vecV2(dctTmp, vec);
      highAC[count] = abs(dctTmp.at<float>(0,1));
      // if(i == OBSX && j == OBSY){
        // std::cout << dctTmp << std::endl;
      //   printf("%f\n", highAC[count]);
      // }
      // double vecWM[8] = {0};
      // embedV2(vec, &wm[count], vecWM);
      // vec2blk(vecWM, dctTmp);
      // idct(dctTmp, dctTmp);
      ++count;
    }
  }

  std::sort(highAC, highAC+4096, std::greater<double>());
  double minSelected = highAC[128];
  //printArray(highAC, 128);
  // printf("min before embedding: %f\n", minSelected);

  count = 0;
  for (int i = 0; i < blueFloat.cols; i += 8){
    for (int j = 0; j < blueFloat.rows; j += 8){
      Mat dctTmp = blueFloat(Rect(i, j, 8, 8));
      double vec[8] = {0};
      blk2vecV2(dctTmp, vec);
      if(abs(dctTmp.at<float>(0,1)) >= minSelected && count < wmLength){
      //   printf("(%d,%d => %f) ", i, j, dctTmp.at<float>(0,1));
        double vecWM[8] = {0};
        embedV2(vec, &wm[count], vecWM, alpha);
        vec2blkV2(vecWM, dctTmp);
        // if(i == OBSX && j == OBSY){
        //   // printArray(vec, 8);
        //   // printArray(vecWM, 8);
        //   printf("%f\n", dctTmp.at<float>(0,1));
        // }
        count += 4;
      }
      idct(dctTmp, dctTmp);
    }
  }

  blueFloat.convertTo(bgr[0],CV_8UC1);
  // bgr[0].convertTo(blueFloat,CV_32FC1);
  // // printf("\n");
  // // std::cout << blueFloat(Rect(OBSX, OBSY, 8, 8)) << std::endl;
  // blueFloat.convertTo(bgr[0],CV_8UC1);
  //
  // Mat dctTmp = blueFloat(Rect(OBSX, OBSY, 8, 8));
  // dctTmp.convertTo(dctTmp,CV_32FC1);
  // dct(dctTmp, dctTmp);
  // // printf("\n\n%f\n\n", dctTmp.at<float>(0,1)); // already changed => due to convesion
  // idct(dctTmp, dctTmp);
  // dctTmp.convertTo(dctTmp,CV_8UC1);

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


void embedV2(double orig[8], int wm[4], double res[8], double alpha){
  for(int i = 0; i<4; ++i){
    res[2*i] = alpha*(2*wm[i]-1)+(orig[2*i]+orig[2*i+1])/2;
    res[2*i+1] = res[2*i] - alpha*(2*wm[i]-1)*2;
  }
}


void exV2(Mat input, int wm[], int len){
  // divide the bgr matrix of the image into the 3 channels blue, green and red
  Mat bgr[3];
  split(input,bgr);

  // start the timer
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
  // convert the blue channel to be compatible with the opencv dct function input type
  Mat blueFloat;
  bgr[0].convertTo(blueFloat,CV_32FC1);

  // std::cout << blueFloat(Rect(OBSX, OBSY, 8, 8)) << std::endl;

  // Mat dctTmp = blueFloat(Rect(OBSX, OBSY, 8, 8));
  // // dctTmp.convertTo(dctTmp,CV_32FC1);
  // dct(dctTmp, dctTmp);
  // // printf("\n\n%f\n\n", dctTmp.at<float>(0,1));
  // idct(dctTmp, dctTmp);
  // dctTmp.convertTo(dctTmp,CV_8UC1);


  double highAC[4096] = {0};
  int count = 0;

  for (int i = 0; i < blueFloat.rows; i += 8){
    for (int j = 0; j < blueFloat.cols; j += 8){
      Mat dctTmp = blueFloat(Rect(i, j, 8, 8));
      dct(dctTmp, dctTmp);
      double vec[8] = {0};
      blk2vecV2(dctTmp, vec);
      highAC[count] = abs(dctTmp.at<float>(0,1));
      // if(i == OBSX && j == OBSY){
      //   std::cout << dctTmp << std::endl;
      //   printf("%f\n", highAC[count]);
      // }
      // double vecWM[8] = {0};
      // extract(vec, &wm[count]);
      ++count;
    }
  }

  std::sort(highAC, highAC+4096, std::greater<double>());
  double minSelected = highAC[128];
  // printArray(highAC, 128);
  // printf("min in extraction: %f\n", minSelected);

  count = 0;
  for (int i = 0; i < blueFloat.rows; i += 8){
    for (int j = 0; j < blueFloat.cols; j += 8){
      if(count >= len)
        break;
      Mat dctTmp = blueFloat(Rect(i, j, 8, 8));
      double vec[8] = {0};
      blk2vecV2(dctTmp, vec);
      if(abs(dctTmp.at<float>(0,1)) >= minSelected){
        // printf("(%d,%d => %f) ", i, j, dctTmp.at<float>(0,1));
        extract(vec, &wm[count]);
        count += 4;
      }
    }
  }

  // print the execution time
  std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  std::cout << time_span.count() << ",";
}
