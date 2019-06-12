#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../helpers.cpp"
#include "../WM/wmV6.cpp"

#define LENGTH 128 // 512 // 16384
#define GOP 6
using namespace cv;
using namespace std;

/**

g++ $(pkg-config --cflags --libs opencv) -std=c++11  VideoEmbedV6.cpp -o VideoEmbedV6

Compatibility:
avi - mjpg => best quality
mkv - x264
mp4 - avc1

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

// convert wm to int array add syncronization sequence
  int wmInt[LENGTH] = {0};
  str2Array(wmStr2, LENGTH, wmInt);

  int nb_blk = (1280.0/32)*(720.0/32);
  int nb_replicate = nb_blk/LENGTH;
  int wm_rep[nb_replicate*LENGTH];
  int buffer[nb_replicate*LENGTH];
  for(int i = 0; i < nb_replicate; ++i){
    for (int j = 0; j < LENGTH; j++){
      wm_rep[i*LENGTH+j] = wmInt[j];
      buffer[i*LENGTH+j] = 1-wmInt[j];
    }
  }

  // initialization of syncronization sequences
  int synSeq0[nb_replicate*LENGTH];
  int synSeq1[nb_replicate*LENGTH];
  for (size_t i = 0; i < nb_replicate*LENGTH; i++){
    synSeq0[i] = 0;
    synSeq1[i] = 1;
  }

    // initialize the video reader and writer
    VideoCapture cap("../../SampleVideo_1280x720_2mb.mp4");
    double fps = cap.get(CAP_PROP_FPS);
    VideoWriter video("../../figures/outcppV6.avi", VideoWriter::fourcc('M','J','P','G'), fps, Size(1280,720));
    // VideoWriter video("../../figures/outcpp.avi", -1 , fps, Size(1280,720));
    if(!cap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
    }

    // start the timer
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    bool done = false;
    Mat frame, res;


    while(!done){
      // read next frame and check if sequence is done
      cap >> frame;
      res = Mat::zeros(frame.size(), CV_8UC3);
      if (frame.empty()){
        done = true;
        break;
      }

      // empty WM between embeddings
      if(frameCount%3 == 2){
        // save the input frame as the output video
        video.write(frame);
      }else{
        int * emb;
        switch ((frameCount/GOP)%3) {
          case 0:
            // embed synSeq of 0s
            if(frameCount%GOP < 2)
              emb = &synSeq0[0];
            else
              emb = &synSeq1[0];
            break;
          case 1:
            // embed synSeq of 1s
            if(frameCount%GOP < 2)
              emb = &synSeq1[0];
            else
              emb = &synSeq0[0];
            break;
          case 2:
            // embed the watermark
            if(frameCount%GOP < 2)
              emb = &wm_rep[0];
            else
              emb = &buffer[0];
            break;
        }
        // actually embed the watermark
        wmV6(frame, emb, LENGTH*nb_replicate, res, alpha);
        // save the frame in the output video
        video.write(res);
    }
      ++frameCount;
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
