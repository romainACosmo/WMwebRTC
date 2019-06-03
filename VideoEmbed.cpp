#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "helpers.cpp"
#include "wmV1.cpp"
#include "wmV2.cpp"
#include "wmV3.cpp"

#define LENGTH 512 // 16384
using namespace cv;
using namespace std;

/**

g++ $(pkg-config --cflags --libs opencv) -std=c++11  main0.cpp -o main0

**/

int main(int argc, char** argv )
{
  if ( argc < 2 )
  {
      printf("Pass alpha as parameter \n");
      return -1;
  }
  double alpha = atof(argv[1]);

  // length = 512
  const char wmStr2[] = "00110010001111000000100000011111100011111011000000110011111100010001101000101110000100000101110101010001000000010101100101111100100111100001001100100101111111100000000101001001111100101000000110001000000101010010001101101001110111000011110111101011011011111000111110010011100011110001011000001101011101110110100101101101011010011000110010001110110110001111001101011010000101101111011101100010011011110101010101001111111111011010101101011100111100000110011110000001101101101110010100100001010001000000001010001110";

  int wmInt[LENGTH] = {0};
  int buffer[LENGTH] = {0};
  str2Array(wmStr2, LENGTH, wmInt);
  for (size_t i = 0; i < LENGTH; i++)
    buffer[i] = 1-wmInt[i];

    VideoCapture cap("SampleVideo_1280x720_2mb.mp4");
    double fps = cap.get(CAP_PROP_FPS);
    VideoWriter video("outcpp.mp4",VideoWriter::fourcc('B','A','8','1'),fps, Size(1280,720));
    if(!cap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
    }

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    int frameCount = 0;


    while(1){
        Mat frame;
        // Capture frame-by-frame
        cap >> frame;
        // If the frame is empty, break immediately
        if (frame.empty())
          break;
        ++frameCount;

        Mat res = Mat::zeros(frame.size(), CV_8UC3);

        wmV1(frame, frameCount%2 == 0 ? wmInt : buffer, LENGTH, res, alpha); // 0.00356083086s latency
        // wmV3(frame, wmInt, LENGTH, res); //


        // Display the resulting frame
        imshow( "Frame", res);
        video.write(res);
        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if(c == 27)
          break;
      }
      // print the execution time
      std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
      std::cout << "Total time: " << time_span.count() << " seconds" << std::endl;
      std::cout << "Number of frames: " << frameCount << std::endl;

      // When everything done, release the video capture object
      cap.release();
      video.release();

      // Closes all the frames
      destroyAllWindows();

      return 0;
    //
    //
    // Mat src = imread("landscape.jpeg", 1), res;
    // if ( !src.data )
    // {
    //     printf("No image data \n");
    //     return -1;
    // }
    //
    // res = Mat::zeros(src.size(), CV_8UC3);
    //
    // // length = 512
    //
    // int wmInt[LENGTH] = {0};
    // str2Array(wmStr2, LENGTH, wmInt);
    // // wmV1(src, wmInt, LENGTH, res);
    // wmV3(src, wmInt, LENGTH, res);
    //
    // std::cout << "PSNR: " << getPSNR(src, res) << std::endl;
    // int wmRes[LENGTH] = {0};
    //
    // imshow("full watermarked image", res);
    // waitKey(0);

    // exV1(res, wmRes, LENGTH);
    // exV2(res, wmRes, LENGTH);
    // int resXor[LENGTH] = {0};
    // myXor(wmInt, wmRes, LENGTH, resXor);
    // std::cout << "Hamming distance: "<< count1(resXor,LENGTH) << std::endl;
    // return 0;
}
