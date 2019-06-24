#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../helpers.cpp"

using namespace cv;
using namespace std;

/**

g++ $(pkg-config --cflags --libs opencv) -std=c++11  PreprocessVideo.cpp -o PreprocessVideo

**/

int main(int argc, char** argv )
{
  if ( argc < 3 )
  {
      printf("Pass video file and original number of frames as parameter \n");
      return -1;
  }
  double orig_fps = atof(argv[2]);

    // initialize the video reader and writer
    VideoCapture cap(argv[1]);
    double fps = cap.get(CAP_PROP_FPS);
    int width = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);
    int total_frames = cap.get(CAP_PROP_FRAME_COUNT);
    int frame_to_keep = total_frames*orig_fps/fps;

    VideoWriter video("../../figures/preprocessed.mp4", cap.get(CAP_PROP_FOURCC), orig_fps, Size(width,height));

    if(!cap.isOpened() || !video.isOpened()){
      cout << "Error opening video stream or file" << endl;
      return -1;
    }

    // start the timer
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    double diff[total_frames-1];
    for (size_t i = 0; i < total_frames-1; i++) {
      diff[i] = 0.0;
    }

    int frame_count = 0;
    bool done = false;
    Mat prev_frame, curr_frame, diff_mat;


    while(!done){
      cap >> curr_frame;

      if (curr_frame.empty()){
        done = true;
        break;
      }

      if(frame_count > 0){
        absdiff(curr_frame, prev_frame, diff_mat);
        diff[frame_count-1] = sum(sum(diff_mat))[0];
      }
      prev_frame = curr_frame.clone();
      ++frame_count;
    }


    std::sort(diff, diff+total_frames-1, std::greater<double>());
    double threshold = diff[frame_to_keep-2];

    cout << "frame to keep: "<< frame_to_keep << " \nThreshold: " << threshold << endl;

    cap.set(CAP_PROP_POS_FRAMES, 0);
    done = false;

      while(!done){
        cap >> curr_frame;

        if (curr_frame.empty()){
          done = true;
          break;
        }

      if(frame_count == 0){
        video.write(curr_frame);
      }else{
        absdiff(curr_frame, prev_frame, diff_mat);
        if(sum(sum(diff_mat))[0] >= threshold){
          video.write(curr_frame);
        }
      }
      prev_frame = curr_frame.clone();
      ++frame_count;

    }

      // print the execution time
      std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
      std::cout << "Total time: " << time_span.count() << " seconds" << std::endl;

      // When everything done, release the video capture object
      cap.release();
      video.release();

      // Closes all the frames
      destroyAllWindows();

      return 0;
}
