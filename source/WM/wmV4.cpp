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
  cout << endl << "new frame" <<endl;
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
      // cout << i << " - " << j << "=>" << count << endl;
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
        }
      }
      // if(j == 20*32 && i == 5*32){
      //   cout << endl << "here: " << count << endl;
      //   cout << (2*wm[count]-1) << endl;
      //   cout << macro_blk << endl;
      //   cout << jnd_tmp << endl;
      // }
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
  int count = 0;
  // cout << yFloat.cols - 31 << " - " << yFloat.rows - 31<< endl;

  // __TODO__
  for (int i = 0; i < yFloat.cols - 31; i += 32){
    for (int j = 0; j < yFloat.rows - 31 &&  count < len; j += 32){
      // Mat macro_blk = yFloat(Rect(i+8, j+8, 16, 16));
      Mat macro_blk = yFloat(Rect(i, j, 32, 32));
      // double tmp = cv::sum(blk)[0];
      // cout << endl << 20 <<endl;
      // cout << count << endl;
      wm[count] += cv::sum(macro_blk)[0]/(32*32);
      // if(j == 20*32 && i == 5*32){
      //   cout << endl << "here: " << count << endl;
      //   cout << macro_blk << endl;
      //   cout << cv::sum(macro_blk)[0]/(16*16) << endl;
      //
      // }
      ++count;

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
