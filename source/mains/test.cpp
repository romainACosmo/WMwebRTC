#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "../helpers.cpp"
#include "../WM/wmV1.cpp"
#include "../WM/wmV2.cpp"
#include "../WM/wmV3.cpp"
#include "../WM/wmV4.cpp"
#include "../WM/studyDCT.cpp"
#define LENGTH 512

using namespace std;

// g++ $(pkg-config --cflags --libs opencv) -std=c++11  test.cpp -o test

int main(int argc, char** argv )
{
  double A[8][8] = {
    {52, 55, 61, 66, 70, 61, 64, 73},
    {63, 59, 55, 90, 109, 85, 69, 72},
    {62, 59, 68, 113, 144, 104, 66, 73},
    {63, 58, 71, 122, 154, 106, 70, 69},
    {67, 61, 68, 104, 126, 88, 68, 70},
    {79, 65, 60, 70, 77, 68, 58, 75},
    {85, 71, 64, 59, 55, 61, 65, 83},
    {87, 79, 69, 68, 65, 76, 78, 94},
};

  const char wmStr2[] = "00110010001111000000100000011111100011111011000000110011111100010001101000101110000100000101110101010001000000010101100101111100100111100001001100100101111111100000000101001001111100101000000110001000000101010010001101101001110111000011110111101011011011111000111110010011100011110001011000001101011101110110100101101101011010011000110010001110110110001111001101011010000101101111011101100010011011110101010101001111111111011010101101011100111100000110011110000001101101101110010100100001010001000000001010001110";

  unsigned char tmp1 = 0x6A;
  int tmp2 = 22;
  cout << tmp1 << endl; // j
  cout << int(tmp1) << endl; // 106
  cout << tmp2 << endl; // 45
  cout << (unsigned char)(tmp2) << endl; // j
  cout << tmp1+tmp2 << endl; // 151
  cout << int(tmp1+tmp2) << endl; // 151
  cout << static_cast<unsigned char>(tmp1+tmp2) << endl; // ?
  cout << (unsigned char)(tmp1+tmp2) << endl; // ?
  unsigned char tmp3 = tmp2;
  unsigned char tmp4 = tmp1+tmp2;
  cout << tmp3 << endl; // -
  cout << tmp4 << endl; // ?



  // VideoCapture cap("../../figures/outcppV6.avi");
  // double fps = cap.get(CAP_PROP_FPS);
  // VideoWriter video("../../figures/outcppV6_dammaged.avi", VideoWriter::fourcc('M','J','P','G'), fps, Size(1280,720));

  // Mat frame;
  // int copied = 0;
  // do {
  //   cap >> frame;
  //   while(copied < 35){
  //     video.write(frame);
  //     ++copied;
  //   }
  //   video.write(frame);
  // } while(!frame.empty());
  //
  // // When everything done, release the video capture object
  // cap.release();
  // video.release();
  //
  // // Closes all the frames
  // destroyAllWindows();

  return 0;
}
