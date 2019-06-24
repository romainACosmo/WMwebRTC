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
38 gop => 337 frames
g++ $(pkg-config --cflags --libs opencv) -std=c++11  VideoExtractV4.cpp -o VideoExtractV5


should find where overflow
**/

Mat next_frame(VideoCapture vc, double * tmp_count, double ratio);

int main(int argc, char** argv )
{
  // length = 512
  const char wmStr2[] = "00110010001111000000100000011111100011111011000000110011111100010001101000101110000100000101110101010001000000010101100101111100100111100001001100100101111111100000000101001001111100101000000110001000000101010010001101101001110111000011110111101011011011111000111110010011100011110001011000001101011101110110100101101101011010011000110010001110110110001111001101011010000101101111011101100010011011110101010101001111111111011010101101011100111100000110011110000001101101101110010100100001010001000000001010001110";

  int wmInt[LENGTH] = {0};
  int buffer[LENGTH] = {0};
  str2Array(wmStr2, LENGTH, wmInt);
  for (size_t i = 0; i < LENGTH; i++)
    buffer[i] = 1-wmInt[i];

    // VideoCapture cap("../../figures/captured_150.avi");
    // VideoCapture cap("../../figures/captured.avi");
    // VideoCapture cap("../../figures/outcppV6.mp4");
    VideoCapture cap("../../figures/preprocessed.mp4");
    if(!cap.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
    }

    double embedding_fps = 25;
    double embedding_width_ratio = 32.0/1280;
    double embedding_height_ratio = 32.0/720;

    double fps = cap.get(CAP_PROP_FPS);
    double width = cap.get(CAP_PROP_FRAME_WIDTH);
    double height = cap.get(CAP_PROP_FRAME_HEIGHT);
    int blk_width = embedding_width_ratio*width;
    int blk_height = embedding_height_ratio*height;

    double fps_ratio = fps/embedding_fps;

    cout << "Size: " << width << " x " << height << endl;
    cout << "Block Size: " << blk_width << " x " << blk_height << endl;

    // initialization of syncronization sequences
    int nb_blk = (width/blk_width)*(height/blk_height);
    int nb_replicate = nb_blk/LENGTH;
    int synSeq0[nb_blk];
    int synSeq1[nb_blk];
    // int prev_sync[nb_blk];
    for (size_t i = 0; i < nb_blk; i++){
      synSeq0[i] = 0;
      synSeq1[i] = 1;
      // prev_sync[i] = -1;
    }

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    int frame_count_embed = 0, prev_wm = -1;
    double frame_count_received = 0.0; // reset to % ratio every read

    bool done = false, sync = false;
    int wm_final[LENGTH];
    for (size_t i = 0; i < LENGTH; i++) {
      wm_final[i] = 0;
    }

    double last_neutral[nb_blk];
    for (size_t i = 0; i < nb_blk; i++) {
      last_neutral[i] = -1;
    }

    while(!done){
      double wm[nb_blk];

      int offset = frame_count_embed;
      while (!sync){

        cout << "start sync, count: " << frame_count_embed << " offset: " << offset << endl;

        // read the 6 next frames
        Mat frames[6];
        for (size_t i = 0; i < 6; i++) {
          frames[i] = next_frame(cap, &frame_count_received, fps_ratio);
          ++frame_count_embed;
          if (frames[i].empty()){
            sync = true;
            done = true;
            break;
          }
        }

        // if not EOF extract the potential WM
        if(!done){
          extractFull(frames, last_neutral, nb_blk, blk_width, blk_height, wm);

          int sum_wm = 0;
          for (size_t i = 0; i < nb_blk; i++)
            sum_wm += wm[i];

          // 75% must be == 1 to be considered as the 1 block
          if(sum_wm > nb_blk*0.8 && prev_wm == 0)
              sync = true; // syncSeq detected
          else if (sum_wm < nb_blk*0.2) // same for 0
            prev_wm = 0;
          else {
            // try with window shifted of 1 frame
            prev_wm = -1;
            ++offset;
            cap.set(CAP_PROP_POS_FRAMES, offset);
          }

          cout << "sum: " << sum_wm << ", sync: " << sync << endl;
        }
      } // end sync loop


      // read the 6 next frames
      Mat frames[6];
      for (size_t i = 0; i < 6; i++) {
        frames[i] = next_frame(cap, &frame_count_received, fps_ratio);
        ++frame_count_embed;
        if (frames[i].empty()){
          done = true;
          break;
        }
      }

      // if not EOF extract the potential WM
      if(!done){
        extractFull(frames, last_neutral, nb_blk, blk_width, blk_height, wm);
        int wm_tmp[LENGTH] = {0};
        for(int i = 0; i < LENGTH; ++i){
          for (int k = 0; k < nb_replicate; k++)
            wm_tmp[i] += wm[i+k*LENGTH];
          wm_tmp[i] = wm_tmp[i] > nb_replicate/2.0 ? 1 : 0;
          wm_final[i] += wm_tmp[i];
        }

        printArray(wm_tmp, LENGTH);
        int resXor[LENGTH] = {0};
        myXor(wmInt, wm_tmp, LENGTH, resXor);
        cout << "Hamming distance = " << count1(resXor,LENGTH) << endl;

        sync = false;
      }
    } // end done loop

    printArray(wm_final, LENGTH);
    for(int i = 0; i < LENGTH; ++i){
      wm_final[i] = wm_final[i] > 2 ? 1 : 0;
    }

    int resXorFinal[LENGTH] = {0};
    myXor(wmInt, wm_final, LENGTH, resXorFinal);
    cout << "count final: " << frame_count_embed << endl << "Total Hamming distance = " << count1(resXorFinal,LENGTH) << endl;

      // print the execution time
      std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
      std::cout << "Total time: " << time_span.count() << " seconds" << std::endl;

      // When everything done, release the video capture object
      cap.release();
      // Closes all the frames
      destroyAllWindows();

      return 0;
}

Mat next_frame(VideoCapture vc, double * tmp_count, double ratio){
  Mat frame, empty;
  Mat tmp_frame = Mat::zeros(Size(vc.get(CAP_PROP_FRAME_WIDTH), vc.get(CAP_PROP_FRAME_HEIGHT)), CV_8UC3);
  int count = 0;
  do{
    vc.read(frame);
    if(frame.empty())
      return empty;
    tmp_frame += frame;
    ++(*tmp_count);
    ++count;
  } while(*tmp_count < ratio && !frame.empty());
  tmp_frame /= count;
  *tmp_count -= ratio;
  return tmp_frame;
}
