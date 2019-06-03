#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../helpers.cpp"
#include "../WM/wmV1.cpp"
#include "../WM/wmV2.cpp"
#include "../WM/wmV3.cpp"
#include "../WM/wmV4.cpp"
#include "../WM/studyDCT.cpp"

#define LENGTH 256 // 512 // 16384
using namespace cv;
using namespace std;

/**

g++ $(pkg-config --cflags --libs opencv) -std=c++11  VideoEmbedV4.cpp -o VideoEmbedV4

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

    VideoCapture cap("../../SampleVideo_1280x720_2mb.mp4");
    double fps = cap.get(CAP_PROP_FPS);
    VideoWriter video("../../figures/outcpp.avi", VideoWriter::fourcc('M','J','P','G'), fps, Size(1280,720));
    if(!cap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
    }

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    bool done = false;

    while(!done){
        // Capture frame-by-frame
        Mat frame0, frame1, frame2;
        for(int i = 0; i < 3; ++i){
          cap >> frame0;
          cap >> frame1;
          cap >> frame2;

          if (frame0.empty() || frame1.empty() || frame2.empty()){
            done = true;
            break;
          }
          frameCount += 3;

          Mat res0 = Mat::zeros(frame0.size(), CV_8UC3);
          Mat res1 = Mat::zeros(frame1.size(), CV_8UC3);
          wmV4(frame0, i == 0 ? wmInt : buffer, LENGTH, res0, alpha);
          wmV4(frame1, i == 0 ? wmInt : buffer, LENGTH, res1, alpha);

          imshow( "Frame", res0);
          video.write(res0);
          imshow( "Frame", res1);
          video.write(res1);
          imshow( "Frame", frame2);
          video.write(frame2);

          // Press  ESC on keyboard to exit
          char c = (char)waitKey(25);
          if(c == 27)
            break;
        }
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
}
