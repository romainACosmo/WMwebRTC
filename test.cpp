#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "helpers.cpp"
#include "wmV1.cpp"
#define LENGTH 512

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
std::cout << A[5][3];
  // cv::Mat B(cv::Size(8, 8), CV_64F, A);
  // std::cout << B << std::endl << std::endl;
  // cv::Mat C = dct88(B);
  // std::cout << C << std::endl << std::endl;
  // cv::Mat D = dct88(C, 1);
  // std::cout << D << std::endl << std::endl;
  // cv::Mat E(cv::Size(8, 8), CV_64F);
  // D.convertTo(E, CV_8UC1);
  // std::cout << E << std::endl << std::endl;
  // double B[8] = {52, 55, 61, 66, 70, 61, 64, 73};

  // VideoCapture cap1("SampleVideo_1280x720_2mb.mp4");
  // VideoCapture cap2("outcpp.mp4");
  //
  // int frameCount = 0;
  // const char wmStr2[] = "00110010001111000000100000011111100011111011000000110011111100010001101000101110000100000101110101010001000000010101100101111100100111100001001100100101111111100000000101001001111100101000000110001000000101010010001101101001110111000011110111101011011011111000111110010011100011110001011000001101011101110110100101101101011010011000110010001110110110001111001101011010000101101111011101100010011011110101010101001111111111011010101101011100111100000110011110000001101101101110010100100001010001000000001010001110";
  // int wmInt[LENGTH] = {0};
  // str2Array(wmStr2, LENGTH, wmInt);
  //
  // while(1){
  //   Mat frame1;
  //   Mat frame2;
  //   cap1 >> frame1;
  //   cap2 >> frame2;
  //     if (frame1.empty() || frame2.empty())
  //       break;
  //     ++frameCount;
  //
  //     Mat res = Mat::zeros(frame1.size(), CV_8UC3);
  //
  //     Mat bgr1[3], bgr2[3], bgr3[3];
  //     split(frame1,bgr1);
  //     split(frame2,bgr2);
  //
  //     imshow("original blue", bgr1[0]);
  //     imshow("loaded blue", bgr2[0]);
  //     waitKey(0);
  //
  //     Mat res2 = Mat::zeros(frame1.size(), CV_8UC1);
  //     Mat res3 = Mat::zeros(frame1.size(), CV_8UC1);
  //
  //     int wmRes1[LENGTH] = {0};
  //     int wmRes2[LENGTH] = {0};
  //
  //     wmV1(frame1, wmInt, LENGTH, res);
  //     split(res,bgr3);
  //     exV1(frame2, wmRes1, LENGTH);
  //     exV1(res, wmRes2, LENGTH);
  //
  //     absdiff(bgr1[0], bgr2[0], res2);
  //     absdiff(bgr3[0], bgr2[0], res3);
  //     std::cout << "sum diff not WM & loaded " << cv::sum(res2)[0] << std::endl;
  //     std::cout << "sum diff WM & loaded " << cv::sum(res3)[0] << std::endl;
  //
  //     Mat tmp1 = bgr3[0](Rect(424, 424, 16, 16));
  //     Mat tmp2 = bgr2[0](Rect(424, 424, 16, 16));
  //     std::cout << tmp1 << std::endl;
  //     std::cout << tmp2 << std::endl;
  //
  //
  //     imshow( "Frame", res);
  //     // imshow("diff not WM & loaded", res2);
  //     // imshow("diff WM & loaded", res3);
  //     waitKey(0);
  //
  //
  //
  //     int resXor1[LENGTH] = {0};
  //     int resXor2[LENGTH] = {0};
  //     // int resXor3[LENGTH] = {0};
  //     myXor(wmInt, wmRes1, LENGTH, resXor1);
  //     myXor(wmInt, wmRes2, LENGTH, resXor2);
  //     // myXor(wmRes, wmRes, LENGTH, resXor2);
  //     // myXor(wmInt, wmRes, LENGTH, resXor3);
  //     // std::cout << "Hamming distance real WM & extracted WM: "<< count1(resXor2,LENGTH) << std::endl;
  //     // std::cout << "Hamming distance real WM & loaded WMv  : "<< count1(resXor1,LENGTH) << std::endl;
  //
  //
  //     char c = (char)waitKey(25);
  //     if(c == 27)
  //       break;
  //   }

  return 0;
}
