#include <stdio.h>
#include <string>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <opencv2/opencv.hpp>
//#include "helpers.cpp"

using namespace cv;

#define EMB_WIDTH 8
#define EMB_HEIGHT 4
// 10x9 pour 1280x720
// 8x4 pour 512x512

// 1280 x 720 => 10x9

/*
nb_blocks = (width/8)*(height*8)                    => 4096 = (512/8)*(512/8)
nb_embed_diff = total_wm_length/wm_bits_per_block   => 128 = 512/4


nb de block par largeur:  √(len(WM)/4)*width/height == 20 // fait à taton


Problem: determine width en height of repetition
*/
void embedV3(double orig[8], int wm[4], double res[8], double alpha);
void reduceWM(int tmpWM[], int wm[]);

void wmV3(Mat input, int wm[], int wmLength, Mat output, double alpha)
{
  // divide the bgr matrix of the image into the 3 channels blue, green and red
  Mat bgr[3];
  split(input,bgr);

  // start the timer
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

  // convert the blue channel to be compatible with the opencv dct function input type
  Mat blueFloat;

  bgr[0].convertTo(blueFloat,CV_32FC1);
  int count = -4;
  for (int i = 0; i < blueFloat.cols; i += 8){
    for (int j = 0; j < blueFloat.rows; j += 8){
      if(j%(EMB_WIDTH*8) == 0){
        count += 4;
        if(i > 0 && j == 0 && i%(EMB_HEIGHT*8) != 0){
          count -= (4*blueFloat.cols/(EMB_WIDTH*8));
        }
      }
      Mat dctTmp = blueFloat(Rect(i, j, 8, 8));
      dct(dctTmp, dctTmp);
      double vec[8] = {0};
      blk2vec(dctTmp, vec);
      double vecWM[8] = {0};
      embedV3(vec, &wm[count], vecWM, alpha);
      vec2blk(vecWM, dctTmp);
      idct(dctTmp, dctTmp);
    }
  }

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


void embedV3(double orig[8], int wm[4], double res[8], double alpha){
  for(int i = 0; i < 4; ++i){
    res[2*i] = alpha*(2*wm[i]-1)+(orig[2*i]+orig[2*i+1])/2;
    res[2*i+1] = res[2*i] - alpha*(2*wm[i]-1)*2;
  }
}


void exV3(Mat input, int wm[], int len){
  // divide the bgr matrix of the image into the 3 channels blue, green and red
  Mat bgr[3];
  split(input,bgr);

  // start the timer
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
  // convert the blue channel to be compatible with the opencv dct function input type
  Mat blueFloat;
  bgr[0].convertTo(blueFloat,CV_32FC1);

  int count = 0;
  for (int i = 0; i < blueFloat.cols; i += 8*EMB_HEIGHT){
    for (int j = 0; j < blueFloat.rows; j += 8*EMB_WIDTH){
      int wm_tot[4] = {0};
      for(int k = 0; k < 8*EMB_HEIGHT; k += 8){
        for(int l = 0; l < 8*EMB_WIDTH; l += 8){
          int wm_tmp[4] = {0};
          Mat dctTmp = blueFloat(Rect(i+k, j+l, 8, 8));
          dct(dctTmp, dctTmp);
          double vec[8] = {0};
          blk2vec(dctTmp, vec);
          double vecWM[8] = {0};
          extract(vec, wm_tmp);
          for(int h = 0; h < 4; ++h)
            wm_tot[h] += wm_tmp[h];
        }
      }
      // printArray(wm_tot, 4);
      for(int m = 0; m < 4; ++m)
        wm[count+m] += wm_tot[m] >= 10 ? 1 : 0; // wm_tot in [0;EMB_WIDTH*EMB_HEIGHT] => 8*4/2 = 16
        // wm[count+m] += wm_tot[m] >= EMB_WIDTH*EMB_HEIGHT/2 ? 1 : 0; // wm_tot in [0;EMB_WIDTH*EMB_HEIGHT] => 8*4/2 = 16

      count += 4;
    }
  }

  // print the execution time
  std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  std::cout << time_span.count() << ",";
}

// void reduceWM(int tmpWM[], int wm[]){
//   for (int i = 0; i < 512; i += 8){
//     for (int j = 0; j < 512; j += 8){
//       if(j%(EMB_WIDTH*8) == 0){
//         count += 4;
//         if(i > 0 && j == 0 && i%(EMB_HEIGHT*8) != 0){
//           count -= (4*512/(EMB_WIDTH*8));
//         }
//       }
//
//     }
//   }
// }
