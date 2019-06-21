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

void wmV6(Mat input, int wm[], int wmLength, Mat output, double alpha)
{
  // start the timer
  chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();


  Mat bgr[3];
  // convert the image from bgr to bgr in separated channels
  split(input, bgr);

  // convert the Y channel to be compatible with the opencv dct function input type
  Mat bFloat;
  bgr[0].convertTo(bFloat, CV_32FC1);

  int count = 0;

  for (int i = 0; i < bFloat.cols - 31; i += 32){
    for (int j = 0; j < bFloat.rows - 31 && count < wmLength; j += 32){
      // cout << i << " - " << j << "=>" << count << endl;
      Mat macro_blk = bFloat(Rect(i, j, 32, 32));
      Mat copyDCT = macro_blk.clone();
      double c = 0;

      for (int x = 0; x < copyDCT.cols; x += 8){
        for (int y = 0; y < copyDCT.rows; y += 8){
          Mat blk = copyDCT(Rect(x, y, 8, 8));
          dct(blk, blk);
          c += blk.at<float>(0,0);
        }
      }
      c /= 16;

      for (int x = 0; x < copyDCT.cols; x+=8){
        for (int y = 0; y < copyDCT.rows; y+=8){
          Mat blk = macro_blk(Rect(x, y, 8, 8)); // 8 as 1 coef computed for each 8x8 blk => only the DC coef
          double tmp = 1.4*pow(copyDCT.at<float>(x,y)/c, LUMASKEXPO);
          blk += (2*wm[count]-1)*alpha/8;
          // blk += tmp*(2*wm[count]-1)*alpha/8;
        }
      }
      ++count;
    }
  }

  bFloat.convertTo(bgr[0], CV_8UC1);
  vector<Mat> channels;
  channels.push_back(bgr[0]);
  channels.push_back(bgr[1]);
  channels.push_back(bgr[2]);
  merge(channels, output);


  // print the execution time
  chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
  chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
  cout << time_span.count() << ", PSNR: " << getPSNR(input, output) << endl;

  // Mat s1;
  // absdiff(input, output, s1);
  // cout << s1 << endl;
}


void exV6(Mat input, double wm[], int len){

  // start the timer
  chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

  Mat bgr[3];

  // convert the image from bgr to bgr in separated channels
  split(input, bgr);

  // convert the Y channel to be compatible with the opencv dct function input type
  Mat bFloat;
  bgr[0].convertTo(bFloat, CV_32FC1);
  int count = 0;

  for (int i = 0; i < bFloat.cols - 31; i += 32){
    for (int j = 0; j < bFloat.rows - 31 &&  count < len; j += 32){
      // Mat macro_blk = bFloat(Rect(i+8, j+8, 16, 16));
      Mat macro_blk = bFloat(Rect(i+8, j+8, 16, 16));
      // double tmp = cv::sum(blk)[0];

      wm[count] += cv::sum(macro_blk)[0]/(16*16);
      ++count;
    }
  }

  // print the execution time
  chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
  chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
  // cout << time_span.count() << ",";
}

void extractFull(Mat input_frames[6], double last_neutral[], int size_wm, double output[]){ // !!! if first give last == -1

  double wmResA[size_wm];
  double wmResB[size_wm];
  bool first = true;

  // substract the sum of neutral frame from wmResA
  for (size_t i = 0; i < size_wm; i++){
    if(last_neutral[i] > 0){
      first = false;
      wmResA[i] = -1*last_neutral[i];
    }
    last_neutral[i] = 0;
  }

  // extract values of the 2 first frames of the 1st part of the GOP
  exV6(input_frames[0], wmResA, size_wm);
  exV6(input_frames[1], wmResA, size_wm);

  // read neutral frame of the first GOP
  exV6(input_frames[2], last_neutral, size_wm);

  // substract the sum of neutral frame from wmResA and wmResB
  for (size_t i = 0; i < size_wm; i++){
    wmResA[i] -= (first ? 2 : 1)*last_neutral[i]; // works well ? use boolean as int (0 and 1)?
    wmResB[i] = -1*last_neutral[i];
    last_neutral[i] = 0;
  }

  // extract values of the 2 first frames of the 2nd part of the GOP
  exV6(input_frames[3], wmResB, size_wm);
  exV6(input_frames[4], wmResB, size_wm);

  // read neutral frame of the second GOP
  exV6(input_frames[5], last_neutral, size_wm);

  for(int i = 0; i < size_wm; ++i){
    // substract the sum of neutral frame from wmResB
    wmResB[i] -= last_neutral[i];

    // if the tendency is to decrease, 1 has been embedded, otherwise 0
    output[i] = wmResA[i] > wmResB[i] ? 1 : 0;
  }
}
