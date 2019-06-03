#include <stdio.h>
#include <string>
#include <math.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define LUMASKEXPO 0.649
#define BETA 2
#define OBSX1 0
#define OBSY1 16

// const double sensitivity_table [8][8] = {
//     {1.4 , 1.01, 1.16, 1.65, 2.4  , 3.43 , 4.79 , 6.55 },
//     {1.01, 1.45, 1.32, 1.52, 2    , 2.71 , 3.67 , 4.93 },
//     {1.16, 1.32, 2.24, 2.59, 2.98 , 3.64 , 4.6  , 5.88 },
//     {1.66, 1.52, 2.59, 3.77, 4.55 , 5.3  , 6.28 , 7.6  },
//     {2.4 , 2   , 2.98, 4.55, 6.15 , 7.46 , 8.71 , 10.17},
//     {3.43, 2.71, 3.64, 5.3 , 7.46 , 9.62 , 11.58, 13.51},
//     {4.79, 3.67, 4.6 , 6.28, 8.71 , 11.58, 14.5 , 17.29},
//     {6.56, 4.93, 5.88, 7.6 , 10.17, 13.51, 17.29, 21.15},
// };

/*
From:
- AN H.264/AVC HDTV watermarking algorithm robust to camcorder
diff: embed the all 32x32 block with 1 bit => 16^2 bits => 256 bit WM
sum 2 first frames - 2 last frames > 0 ur < 0

- Blind Image Steganalysis Based on Local Information an HVS
Problem:
*/

void wmV4(Mat input, int wm[], int wmLength, Mat output, double alpha) // consider WM length == 512
{
  // start the timer
  chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

  Mat yuvAll;
  Mat yuv[3];

  // convert the image from bgr to yuv in separated channels
  cvtColor(input, yuvAll, COLOR_BGR2YCrCb);
  split(yuvAll, yuv);

  // convert the Y channel to be compatible with the opencv dct function input type
  Mat yFloat, jnd;
  yuv[0].convertTo(yFloat, CV_32FC1);
  jnd = Mat::zeros(Size(yFloat.cols/8, yFloat.rows/8), CV_32FC1);
  // cout << yFloat.cols << " x " << yFloat.rows << endl;

  int count = 0;
  for (int i = 0; i < yFloat.cols - 31; i += 32){
    for (int j = 0; j < yFloat.rows - 31 && count < wmLength; j += 32){
      Mat macro_blk = yFloat(Rect(i, j, 32, 32));
      Mat jnd_tmp = jnd(Rect(i/8, j/8, 32/8, 32/8)); // 8 as 1 coef computed for each 8x8 blk => only the DC coef
      double c = 0;

      for (int x = 0; x < macro_blk.cols; x += 8){
        for (int y = 0; y < macro_blk.rows; y += 8){
          Mat blk = macro_blk(Rect(x, y, 8, 8));
          dct(blk, blk);
          c += blk.at<float>(0,0);
        }
      }
      c /= 16;

      for (int x = 0; x < jnd_tmp.cols; ++x){
        for (int y = 0; y < jnd_tmp.rows; ++y){
          Mat blk = macro_blk(Rect(x*8, y*8, 8, 8)); // 8 as 1 coef computed for each 8x8 blk => only the DC coef

          jnd_tmp.at<float>(x,y) = 1.4*pow(blk.at<float>(0,0)/c, LUMASKEXPO);
          idct(blk, blk);

          blk += jnd_tmp.at<float>(x,y)*(2*wm[count]-1)*alpha/8;

          // int maxBrightX = 0;
          // int maxBrightY = 0;
          // double maxBright = 0.0;
          // for (int m = 0; m < 4; m += 4) {
          //   for (int n = 0; n < 4; n += 4) {
          //     double tmp = cv::sum(blk(Rect(m, n, 4, 4)))[0];
          //     if(tmp >= maxBright){
          //       maxBright = tmp;
          //       maxBrightX = m;
          //       maxBrightY = n;
          //     }
          //   }
          // }
          // Mat mini_blk = blk(Rect(maxBrightX, maxBrightY, 4, 4));
          // mini_blk += jnd_tmp.at<float>(x,y)*(2*wm[count]-1)*alpha/8;
        }
      }
      ++count;
    }
  }

  yFloat.convertTo(yuv[0], CV_8UC1);
  vector<Mat> channels;
  channels.push_back(yuv[0]);
  channels.push_back(yuv[1]);
  channels.push_back(yuv[2]);
  merge(channels, yuvAll);
  cvtColor(yuvAll, output, COLOR_YCrCb2BGR);

  // print the execution time
  chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
  chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
  cout << time_span.count() << ",";
}


void exV4(Mat input, double wm[], int len){

  // start the timer
  chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

  Mat yuvAll;
  Mat yuv[3];

  // convert the image from bgr to yuv in separated channels
  cvtColor(input, yuvAll, COLOR_BGR2YCrCb);
  split(yuvAll, yuv);

  // convert the Y channel to be compatible with the opencv dct function input type
  Mat yFloat;
  yuv[0].convertTo(yFloat, CV_32FC1);

  // __TODO__
  for (int i = 0; i < yFloat.cols; i += 32){
    for (int j = 0; j < yFloat.rows; j += 32){
      Mat macro_blk = yFloat(Rect(i+8, j+8, 16, 16));
      // double tmp = cv::sum(blk)[0];
      // cout << i/32*yFloat.cols/32+j/32 << endl;

      wm[i/32*yFloat.cols/32+j/32] += cv::sum(macro_blk)[0];

      // for (int x = 0; x < yFloat.cols; x+=8){
      //   for (int y = 0; y < yFloat.rows; y+=8){
      //     Mat blk = macro_blk(Rect(x, y, 8, 8));
      //     double tmp = cv::sum(blk)[0];
      //     wm[i/32*yFloat.cols+j/32] += cv::sum(blk)[0];



          // int maxBrightX = 0;
          // int maxBrightY = 0;
          // double maxBright = 0.0;
          // for (int m = 0; m < 4; m+=4){
          //   for (int n = 0; n < 4; n+=4){
          //     double tmp = cv::sum(blk(Rect(m, n, 4, 4)))[0];
          //     if(tmp >= maxBright){
          //       maxBright = tmp;
          //       maxBrightX = m;
          //       maxBrightY = n;
          //     }
          //     // wm[i/32*yFloat+j/32] += blk.at<float>(0,0);
          //   }
          // }
          // Mat mini_blk = blk(Rect(maxBrightX, maxBrightY, 4, 4));
          // wm[i/32*yFloat.cols+j/32] += jnd_tmp.at<float>(x,y)*(2*wm[count]-1)*alpha/8;
      //   }
      // }


    }
  }

  // print the execution time
  chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
  chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
  cout << time_span.count() << ",";
}

// void exV4Wrapper(Mat frame0, Mat frame1, Mat frame3, Mat frame4, double wm[], int len){
//   double wm_tmp01[];
//   double wm_tmp34[];
//   exV4(frame0, wm_tmp, len);
//   exV4(frame1, wm_tmp, len);
//   exV4(frame3, wm_tmp, len);
//   exV4(frame4, wm_tmp, len);
// }
