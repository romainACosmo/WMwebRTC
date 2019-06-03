#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "helpers.cpp"
// #include "wmV1.cpp"
// #include "wmV2.cpp"
#include "wmV3.cpp"
// #include "studyDCT.cpp"

#define LENGTH 512 // 16384
#define NB_BLOCK_SELECTED 512
#define MODE 3
using namespace cv;

/**

g++ $(pkg-config --cflags --libs opencv) -std=c++11  ImageEmbed.cpp -o ImageEmbed

**/

int main(int argc, char** argv )
{

    Mat src = imread("figures/version3/alpha28.jpg", 1), res;
    Mat src2 = imread("IMG_2791.jpeg", 1);
    if ( !src.data )
    {
        printf("No image data \n");
        return -1;
    }
    res = Mat::zeros(src.size(), CV_8UC3);


    // length = 512
    const char wmStr2[] = "00110010001111000000100000011111100011111011000000110011111100010001101000101110000100000101110101010001000000010101100101111100100111100001001100100101111111100000000101001001111100101000000110001000000101010010001101101001110111000011110111101011011011111000111110010011100011110001011000001101011101110110100101101101011010011000110010001110110110001111001101011010000101101111011101100010011011110101010101001111111111011010101101011100111100000110011110000001101101101110010100100001010001000000001010001110";

    int wmInt[LENGTH] = {0};
    str2Array(wmStr2, LENGTH, wmInt);

    // wmV1(src, wmInt, LENGTH, res);
    int count = 28;
    wmV3(src, wmInt, LENGTH, res, count);

      Mat bgr1[3], bgr2[3];
      split(src,bgr1);
      split(src2,bgr2);
      std::cout << 1 << std::endl;

      // imshow("Diff full", bgr1[0]);
      // imshow("Diff 2", bgr2[0]);
      // waitKey(0);
      std::cout << 2 << std::endl;

      imwrite("figures/blueChannelWM.jpeg", bgr1[0]);
      std::cout << 3 << std::endl;

      imwrite("figures/blueChannelPhoto.jpeg", bgr2[0]);
      std::cout << 4 << std::endl;


      // Mat bgr1[3], bgr2[3];
      // split(res,bgr1);
      // std::cout << bgr1[0](Rect(0, 18, 8, 8)) << std::endl;

      // int wmRes[LENGTH] = {0};
      // // exV1(src, wmRes, LENGTH);
      // // int resXor[LENGTH] = {0};
      // // myXor(wmInt, wmRes, LENGTH, resXor);
      //
      // // split(res,bgr2);
      // // Mat tmp2 = bgr2[0](Rect(0, 16, 8, 8));
      // // tmp2.convertTo(tmp2,CV_32FC1);
      // // dct(tmp2, tmp2);
      // // std::cout << std::endl << tmp2 << std::endl;
      //
      // std::cout << getPSNR(src, res) << std::endl;
      // std::stringstream ss;
      // ss << "figures/version" << version << "/alpha" << count <<".jpg";
      // imwrite(ss.str(), res);
      // count+=4;
    // }
    // studyDCT(src, res, MODE, NB_BLOCK_SELECTED);
    // studyDCT(src2, res2, MODE, NB_BLOCK_SELECTED);



    // Mat bgr1[3], bgr2[3];
    // split(res,bgr1);
    // split(src2,bgr2);
    // // waitKey(0);
    // Mat dctTmp = bgr1[0](Rect(424, 424, 16, 16));
    // Mat dctTmp2 = bgr2[0](Rect(424, 424, 16, 16));
    // dctTmp.convertTo(dctTmp,CV_32FC1);
    // dctTmp2.convertTo(dctTmp2,CV_32FC1);
    //
    // res2 = Mat::zeros(dctTmp.size(), CV_32FC1);
    //
    // dct(dctTmp, dctTmp);
    // dct(dctTmp2, dctTmp2);
    //
    // absdiff(dctTmp, dctTmp2, res2);
    // std::cout << endl << dctTmp << std::endl;
    // std::cout << endl << dctTmp2 << std::endl;
    // std::cout << endl << res2 << std::endl;
    //
    //
    // idct(dctTmp, dctTmp);
    // idct(dctTmp2, dctTmp2);
    //
    // dctTmp.convertTo(dctTmp,CV_8UC1);
    // dctTmp2.convertTo(dctTmp2,CV_8UC1);



    // std::cout << endl << res2(Rect(424, 424, 16, 16)) << std::endl;

    // exV1(res, wmRes, LENGTH);
    // int resXor[LENGTH] = {0};
    // myXor(wmInt, wmRes, LENGTH, resXor);
    // std::cout << "Hamming distance: "<< count1(resXor,LENGTH) << std::endl;

    // imshow("Diff full", res2);

    // imshow("Watermarked blue", bgr1[0]);
    // waitKey(0);

    // imshow("Watermarked block blue", bgr1[0](Rect(424, 424, 16, 16)));
    // waitKey(0);
    // imshow("Photo blue", bgr2[0]);

    // imshow("Photo block blue", bgr2[0](Rect(424, 424, 16, 16)));
    // waitKey(0);



    // exV1(res, wmRes, LENGTH);
    // exV3(src2, wmRes, LENGTH);
    // int resXor[LENGTH] = {0};
    // myXor(wmInt, wmRes, LENGTH, resXor);
    // std::cout << "Hamming distance: "<< count1(resXor,LENGTH) << std::endl;
    return 0;
}
