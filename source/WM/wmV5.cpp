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
#define LENGTH 128

/*
idea:
- locate embedding block with first AC coefficients
- embed in DC with JND ?
Problem:
*/

// void wmV5(Mat input, int wm[], int wmLength, Mat output, double alpha) // consider WM length == 512
// {
//   // start the timer
//   chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
//
//   Mat yuvAll;
//   Mat yuv[3];
//   // convert the image from bgr to yuv in separated channels
//   cvtColor(input, yuvAll, COLOR_BGR2YCrCb);
//   split(yuvAll, yuv);
//
//   // convert the Y channel to be compatible with the opencv dct function input type
//   Mat yFloat; // , jnd;
//   yuv[0].convertTo(yFloat, CV_32FC1);
//   // jnd = Mat::zeros(Size(yFloat.cols/8, yFloat.rows/8), CV_32FC1);
//   // cout << yFloat.cols << " x " << yFloat.rows << endl;
//
//   const int nb_blk = (yFloat.cols/8)*(yFloat.rows/8);
//   double selected[nb_blk];
//   memset(selected, 0.0, nb_blk*sizeof(double));
//
//   int count = 0;
//
//   for (int i = 0; i < yFloat.cols - 7; i += 8){
//     for (int j = 0; j < yFloat.rows - 7; j += 8){
//       Mat dctTmp = yFloat(Rect(i, j, 8, 8));
//       dct(dctTmp, dctTmp);
//       double vec[8] = {0.0};
//       blk2vec(dctTmp, vec);
//       selected[count] = absSum(vec, 5);
//       ++count;
//     }
//   }
//
//   std::sort(selected, selected+nb_blk, std::greater<double>());
//   double minSelected = selected[LENGTH];
//
//   // cout << endl << endl;
//   // printArray(selected, 130);
//
//   count = 0;
//
//   for (int i = 0; i < yFloat.cols - 7; i += 8){
//     for (int j = 0; j < yFloat.rows - 7; j += 8){
//       // cout << i << " - " << j << "=>" << count << endl;
//       Mat macro_blk = yFloat(Rect(i, j, 8, 8));
//       double vec[8] = {0.0};
//       blk2vec(macro_blk, vec);
//       if(absSum(vec, 5) >= minSelected){
//         cout << i << " - " << j << " => " << absSum(vec, 5) << endl;
//         int mean = macro_blk.at<float>(0,0);
//         idct(macro_blk, macro_blk);
//
//         // macro_blk += 1.4*wm[count]*pow(mean, LUMASKEXPO)*alpha/8;
//         // macro_blk.at<float>(0,0) += 1.4*wm[count]*pow(macro_blk.at<float>(0,0), LUMASKEXPO)*alpha/8;
//         ++count;
//       }else{
//         macro_blk.at<float>(0,1) = 0;
//         macro_blk.at<float>(1,0) = 0;
//         idct(macro_blk, macro_blk);
//       }
//       // blk2vec(macro_blk, vec);
//       // selected[count] = absSum(vec, 5);
//     }
//   }
//
//   // std::sort(selected, selected+nb_blk, std::greater<double>());
//   // cout << endl;
//   // printArray(selected, 130);
//
//   yFloat.convertTo(yuv[0], CV_8UC1);
//   vector<Mat> channels;
//   channels.push_back(yuv[0]);
//   channels.push_back(yuv[1]);
//   channels.push_back(yuv[2]);
//   merge(channels, yuvAll);
//   cvtColor(yuvAll, output, COLOR_YCrCb2BGR);
//
//   // print the execution time
//   chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
//   chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
//   // cout << time_span.count() << ", PSNR: " << getPSNR(input, output) << endl;
//   cout << "----------------------------" << endl;
// }

void wmV5(Mat input, int wm[], int wmLength, Mat output, double alpha) // consider WM length == 512
{
    // Mat yuvAll;
    Mat bgr[3];
    // convert the image from bgr to yuv in separated channels
    split(input, bgr);

    // convert the Y channel to be compatible with the opencv dct function input type
    Mat bFloat; // , jnd;
    bgr[0].convertTo(bFloat, CV_32FC1);

    const int nb_blk = (bFloat.cols/8)*(bFloat.rows/8);
      double selected[nb_blk];
      memset(selected, 0.0, nb_blk*sizeof(double));

      int count = 0;
      // Mat tmppp = bgr[0](Rect(24, 80, 8, 8));
      // cout << tmppp << endl;

      for (int i = 0; i < bFloat.cols - 7; i += 8){
        for (int j = 0; j < bFloat.rows - 7; j += 8){
          Mat dctTmp = bFloat(Rect(i, j, 8, 8));
          dct(dctTmp, dctTmp);
          double vec[8] = {0.0};
          blk2vec(dctTmp, vec);
          // selected[count] = dctTmp.at<float>(0,1);
          selected[count] = absSum(vec, 2);
          ++count;
        }
      }

      std::sort(selected, selected+nb_blk, std::greater<double>());
      double minSelected = selected[LENGTH];
      printf("min: %f\n", minSelected);

      for (int i = 0; i < bFloat.cols - 7; i += 8){
        for (int j = 0; j < bFloat.rows - 7; j += 8){
          Mat dctTmp = bFloat(Rect(i, j, 8, 8));
          double vec[8] = {0.0};
          blk2vec(dctTmp, vec);

          // if(dctTmp.at<float>(0,1) >= minSelected)
          if(absSum(vec, 2) >= minSelected)
            printf("%d - %d\n", i, j);

          idct(dctTmp, dctTmp);

          ++count;
        }
      }



        bFloat.convertTo(bgr[0], CV_8UC1);
        // cout << tmppp << endl;

        vector<Mat> channels;
        channels.push_back(bgr[0]);
        channels.push_back(bgr[1]);
        channels.push_back(bgr[2]);
        merge(channels, output);

    // split(yuvAll, yuv);
    //
    // // convert the Y channel to be compatible with the opencv dct function input type
    // yuv[0].convertTo(yFloat, CV_32FC1);

    // cout << tmp1-cv::sum(yFloat)[0] << endl;

    // cout << tmp1 << endl;
    printf("%s\n", "--------------------------------------------");
}

void exV5(Mat input, double wm[], int len){
  Mat bgr[3];
  // convert the image from bgr to yuv in separated channels
  split(input, bgr);

  // convert the Y channel to be compatible with the opencv dct function input type
  Mat bFloat; // , jnd;
  bgr[0].convertTo(bFloat, CV_32FC1);

  const int nb_blk = (bFloat.cols/8)*(bFloat.rows/8);
    double selected[nb_blk];
    memset(selected, 0.0, nb_blk*sizeof(double));

    int count = 0;
    // Mat tmppp = bgr[0](Rect(24, 80, 8, 8));
    // cout << tmppp << endl;

    for (int i = 0; i < bFloat.cols - 7; i += 8){
      for (int j = 0; j < bFloat.rows - 7; j += 8){
        Mat dctTmp = bFloat(Rect(i, j, 8, 8));
        dct(dctTmp, dctTmp);
        double vec[8] = {0.0};
        blk2vec(dctTmp, vec);
        selected[count] = absSum(vec, 2);
        ++count;
      }
    }

    std::sort(selected, selected+nb_blk, std::greater<double>());
    double minSelected = selected[LENGTH];
    printf("min: %f\n", minSelected);


    for (int i = 0; i < bFloat.cols - 7; i += 8){
      for (int j = 0; j < bFloat.rows - 7; j += 8){
        Mat dctTmp = bFloat(Rect(i, j, 8, 8));
        double vec[8] = {0.0};
        blk2vec(dctTmp, vec);

        // if(dctTmp.at<float>(0,1) >= minSelected)
        if(absSum(vec, 2) >= minSelected)
          printf("%d - %d\n", i, j);

        ++count;
      }
    }
    printf("%s\n", "--------------------------------------------");
}

/*
[ 24,  29,  28,  38,  42,  59,  76,  93;
[ 20,  24,  24,  33,  38,  54,  72,  89;


  19,  25,  33,  49,  90, 108, 126, 128;
  15,  21,  29,  45,  86, 103, 122, 124;




  42,  61,  98, 114, 128, 131, 133, 132;
  38,  57,  94, 110, 124, 126, 129, 128;
 106, 117, 129, 125, 118, 115, 115, 110;
 121, 113, 110,  98, 103,  99,  95,  91;
 106,  92,  81,  74,  78,  75,  71,  66;
  72,  63,  54,  54,  56,  54,  48,  43;
  49,  45,  42,  41,  44,  41,  35,  30]


 110, 122, 133, 129, 122, 119, 119, 115;



 124, 116, 113, 101, 106, 103,  98,  94;
 109,  95,  84,  77,  82,  78,  74,  69;
  75,  66,  57,  57,  60,  57,  49,  44;
  52,  48,  46,  44,  47,  44,  36,  32]
*/


// void exV5(Mat input, double wm[], int len){
//
//   // start the timer
//   chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
//
//   Mat yuvAll;
//   Mat yuv[3];
//   // convert the image from bgr to yuv in separated channels
//   cvtColor(input, yuvAll, COLOR_BGR2YCrCb);
//   split(yuvAll, yuv);
//
//   // convert the Y channel to be compatible with the opencv dct function input type
//   Mat yFloat;
//   yuv[0].convertTo(yFloat, CV_32FC1);
//   int count = 0;
//
//   const int nb_blk = (yFloat.cols/8)*(yFloat.rows/8);
//   double selected[nb_blk];
//   memset(selected, 0.0, nb_blk*sizeof(double));
//
//   for (int i = 0; i < yFloat.cols - 7; i += 8){
//     for (int j = 0; j < yFloat.rows - 7; j += 8){
//       Mat dctTmp = yFloat(Rect(i, j, 8, 8));
//       dct(dctTmp, dctTmp);
//       double vec[8] = {0.0};
//       blk2vec(dctTmp, vec);
//       selected[count] = absSum(vec, 5);
//       ++count;
//     }
//   }
//
//   std::sort(selected, selected+nb_blk, std::greater<double>());
//   double minSelected = selected[LENGTH];
//
//   // cout << endl;
//   // printArray(selected, 130);
//
//   count = 0;
//
//   // __TODO__
//   for (int i = 0; i < yFloat.cols - 7; i += 8){
//     for (int j = 0; j < yFloat.rows - 7 &&  count < len; j += 8){
//       Mat macro_blk = yFloat(Rect(i, j, 8, 8));
//       double vec[8] = {0.0};
//       blk2vec(macro_blk, vec);
//       if(absSum(vec, 5) >= minSelected){
//         // if(absSum(vec, 5) >= 840)
//         cout << i << " - " << j << " => " << absSum(vec, 5) << endl;
//         idct(macro_blk, macro_blk);
//
//         // wm[count] += macro_blk.at<float>(0,0); // mauvaise dét&éction pour ancienne technique aussi
//         wm[count] += cv::sum(macro_blk)[0]/(8*8); // mauvaise dét&éction pour ancienne technique aussi
//         ++count;
//       }
//     }
//   } // check les index selectionné
//
//   cout << "----------------------------" << endl;
//
//   // print the execution time
//   chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
//   chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
//   // cout << "Time:" << time_span.count() << endl;
// }
