#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../helpers.cpp"
#include "../WM/wmV1.cpp"
#include "../WM/wmV2.cpp"
#include "../WM/wmV3.cpp"
#include "../WM/wmV4.cpp"
#include "../WM/wmV6.cpp"
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

    VideoCapture cap("../../figures/captured.avi");
    // VideoCapture cap("../../figures/outcppV6.avi");
    if(!cap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
    }

    double fps = cap.get(CAP_PROP_FPS);
    double embedding_fps = 25;
    double fps_ratio = fps/embedding_fps;
    double width = cap.get(CAP_PROP_FRAME_WIDTH);
    double height = cap.get(CAP_PROP_FRAME_HEIGHT);

    cout << fps << endl;

    // initialization of syncronization sequences
    int nb_blk = (width/32)*(height/32);
    int nb_replicate = nb_blk/LENGTH;
    int synSeq0[nb_replicate*LENGTH];
    int synSeq1[nb_replicate*LENGTH];
    for (size_t i = 0; i < nb_replicate*LENGTH; i++){
      synSeq0[i] = 0;
      synSeq1[i] = 1;
    }

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    int frame_count_embed = 0, frame_count_received = 0;

    bool done = false, sync = false;


    while(!done){

      double wmResA[nb_blk];
      double wmResB[nb_blk];

      done = true;

      while (!sync){ // change frame rate
        Mat frame;
        for (size_t i = 0; i < 2; i++) {
          frame = cap.read();
          if (frame.empty())
            break;
          exV6(frame, wmResA, nb_blk);

        }
        frame = cap.read();
        if (frame.empty())
          break;

        for (size_t i = 0; i < 2; i++) {
          frame = cap.read();
          if (frame.empty())
            break;
          exV6(frame, wmResA, nb_blk);

        }
        frame = cap.read();
        if (frame.empty())
          break;



        cap.set(CAP_PROP_POS_FRAMES, 0);
      }

      }

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

Mat next_frame(VideoCapture vc, int frame_count_embed, int fps_ratio){
  Mat frame = vc.read();
  if (frame.empty())
    return NULL;
}
