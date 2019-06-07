#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../helpers.cpp"
#include "../WM/wmV1.cpp"
#include "../WM/wmV2.cpp"
#include "../WM/wmV3.cpp"
#include "../WM/wmV4.cpp"
#include "../WM/wmV5.cpp"
#include "../WM/studyDCT.cpp"

#define LENGTH 128
using namespace cv;
using namespace std;

/**
38 gop
g++ $(pkg-config --cflags --libs opencv) -std=c++11  VideoExtractV4.cpp -o VideoExtractV5

**/

int main(int argc, char** argv )
{
  // length = 512
  const char wmStr2[] = "00110010001111000000100000011111100011111011000000110011111100010001101000101110000100000101110101010001000000010101100101111100100111100001001100100101111111100000000101001001111100101000000110001000000101010010001101101001110111000011110111101011011011111000111110010011100011110001011000001101011101110110100101101101011010011000110010001110110110001111001101011010000101101111011101100010011011110101010101001111111111011010101101011100111100000110011110000001101101101110010100100001010001000000001010001110";


  int wmInt[LENGTH] = {0};
  int buffer[LENGTH] = {0};
  str2Array(wmStr2, LENGTH, wmInt);
  for (size_t i = 0; i < LENGTH; i++)
    buffer[i] = 1-wmInt[i];

    VideoCapture cap("../../figures/outcppV5.mkv");
    if(!cap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
    }

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    bool done = false;
    int wmRes[LENGTH] = {0};

    while(!done){
        double wmResA[LENGTH];
        double wmResB[LENGTH];
        memset(wmResA, 0.0, LENGTH*sizeof(double));
        memset(wmResB, 0.0, LENGTH*sizeof(double));

        // Capture frame-by-frame
        Mat frame0, frame1, frame2;
        for(int i = 0; i < 2; ++i){
          cap >> frame0;
          cap >> frame1;
          cap >> frame2;

          if (frame0.empty() || frame1.empty() || frame2.empty()){
            done = true;
            break;
          }
          frameCount += 3;

          exV5(frame0, i == 0 ? wmResA : wmResB, LENGTH);
          exV5(frame1, i == 0 ? wmResA : wmResB, LENGTH);
        }
        // cout << "printing wm res for frames " << frameCount << endl;

        for(int i = 0; i < LENGTH; ++i){
          wmRes[i] += wmResA[i] > wmResB[i] ? 1 : 0;
        }

        int resXor[LENGTH] = {0};
        // cout << endl;
        // printArray(wmInt, LENGTH);
        // printArray(wmRes, LENGTH);
        myXor(wmInt, wmRes, LENGTH, resXor);

        // std::cout << endl << count1(resXor,LENGTH) << endl;
      }
      cout << endl;
      // printArray(wmRes, LENGTH);

      for(int i = 0; i < LENGTH; ++i)
        wmRes[i] = wmRes[i] > 19 ? 1 : 0;
      int resXorFinal[LENGTH] = {0};
      myXor(wmInt, wmRes, LENGTH, resXorFinal);
      // printArray(wmRes, LENGTH);

      for (size_t i = 0; i < LENGTH; i++) {
        if(resXorFinal[i] == 1){
          cout << resXorFinal[i] << endl;
          cout << i << ": " << (i*32%1280)/32 << " - " << i*32/1280 << endl;
          cout << wmRes[i] << endl;
          cout << wmInt[i] << endl << endl;
        }
      }

      cout << "Hamming distance = " << count1(resXorFinal,LENGTH) <<endl;

      // printArray(wmRes, LENGTH);
      // cout << endl;
      // printArray(resXorFinal, LENGTH);

    // while(1){
    //     Mat frame;
    //     // Capture frame-by-frame
    //     cap >> frame;
    //     // If the frame is empty, break immediately
    //     if (frame.empty())
    //       break;
    //     ++frameCount;
    //
    //     int wmRes[LENGTH] = {0};
    //
    //     exV1(frame, wmRes, LENGTH);
    //     int resXor[LENGTH] = {0};
    //     myXor(wmInt, wmRes, LENGTH, resXor);
    //     std::cout << "Hamming distance: "<< count1(resXor,LENGTH) << std::endl;
    //
    //   }
      // print the execution time
      std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
      std::cout << "Total time: " << time_span.count() << " seconds" << std::endl;
      std::cout << "Number of frames: " << frameCount << std::endl;

      // When everything done, release the video capture object
      cap.release();
      // Closes all the frames
      destroyAllWindows();

      return 0;
}
