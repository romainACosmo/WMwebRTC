#include <stdlib.h>
#include <numeric>
#include <math.h>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;

/*
operate the Discrete Cosine Function on a matrix of height and width multiple of 8
blocks (of 8x8) by blocks **in place**
precising inverse = 1 compute the inverse DCT
*/
void dct88(Mat const& src, bool inverse = 0){
  for (int i = 0; i < src.rows; i += 8)
    {
        for (int j = 0; j < src.cols; j+= 8)
        {
            Mat block = src(Rect(i, j, 8, 8));
            if(inverse){
              idct(block, block);
            }else{
              dct(block, block);
            }
        }
    }
}

  void printArray88(double arr[8][8])
  {
     int i,j;
     for(i = 0; i<8; i++)
     {
       printf("\n");
       for(j = 0; j<8; j++)
       {
         printf("%f\t", arr[i][j]);
       }
     }
  }

  void printArray(double arr[], int len)
  {
     for(int i = 0; i<len; i++)
     {
       printf("%f\t", arr[i]);
     }
     printf("\n");
  }

  void printArray(bool arr[], int len)
  {
     for(int i = 0; i<len; i++)
     {
       printf("%d\n", arr[i]);
     }
     printf("\n");
  }

  void printArray(int arr[], int len)
  {
     for(int i = 0; i<len; i++)
     {
       printf("%d, ", arr[i]);
     }
     printf("\n");
  }

  String type2str(int type) {
    String r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch ( depth ) {
      case CV_8U:  r = "8U"; break;
      case CV_8S:  r = "8S"; break;
      case CV_16U: r = "16U"; break;
      case CV_16S: r = "16S"; break;
      case CV_32S: r = "32S"; break;
      case CV_32F: r = "32F"; break;
      case CV_64F: r = "64F"; break;
      default:     r = "User"; break;
    }

    r += "C";
    r += (chans+'0');

    return r;
  }

void str2Array(std::string wmStr, int length, int wmInt[]){
  for(int i = 0; i < length; ++i){
    wmInt[i] = wmStr[i] == '1' ? 1 : 0;
  }
  // printArray(wmInt, length);
}

void myXor(int a[], int b[], int len, int res[]){
  for(int i = 0; i < len; ++i)
    res[i] = a[i] == b[i] ? 0 : 1;
}

int count1(int a[], int len){
  int res = 0;
  for(int i = 0; i < len; ++i){
    if(a[i] > 0)
      ++res;
  }
  return res;
}

double getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);         // sum elements per channel
    // std::cout << s1 << std::endl << std::endl << std::endl;


    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10){ // for small values return zero
        // std::cout << "tooo small: " << sse << std::endl;
        return 0;
      }
    else
    {
        double  mse =sse /(double)(I1.channels() * I1.total());
        double psnr = 10.0*log10((255*255)/mse);
        return psnr;
    }
}

double absSum(double a[], int n)
{
    double res  = 0.0;
    for (size_t i = 0; i < n; ++i)
      res += fabs(a[i]);
    return res;
}

double countNZZ(Mat input)
{
    double res  = 0.0;
    for (size_t i = 0; i < input.cols; ++i)
      for (size_t j = 0; j < input.rows; ++j)
        res += fabs(input.at<float>(i,j)) > 10 ? 1 : 0;
    return res;
}

double arrayAbsSum(double a[], int n)
{
    double res  = 0.0;
    bool opp = false;
    for (size_t i = 0; i < n-1; i+=2) {
      // do not select if the 2 following coefficients have opposite signs
      if(a[i]*a[i+1] > 0)
        opp = true;
      res += fabs(a[i]+a[i+1]);
    }
    return opp ? 0 : res;
}

void extract(double orig[8], int wm[4]){
  for(int i = 0; i<4; ++i){
    wm[i] = orig[2*i] > orig[2*i+1] ? 1 : 0;
  }
}

void blk2vec(Mat blk, double res[8]){
  res[0] = blk.at<float>(0,1);
  res[1] = blk.at<float>(1,0);
  res[2] = blk.at<float>(2,0);
  res[3] = blk.at<float>(1,1);
  res[4] = blk.at<float>(0,2);
  res[5] = blk.at<float>(0,3);
  res[6] = blk.at<float>(1,2);
  res[7] = blk.at<float>(2,1);
}

void blk2vecV2(Mat blk, double res[8]){
  res[0] = blk.at<float>(1,0);
  res[1] = blk.at<float>(2,0);
  res[2] = blk.at<float>(1,1);
  res[3] = blk.at<float>(0,2);
  res[4] = blk.at<float>(0,3);
  res[5] = blk.at<float>(1,2);
  res[6] = blk.at<float>(2,1);
  res[7] = blk.at<float>(3,0);
}

void vec2blk(double vec[8], Mat orig_blk){
  orig_blk.at<float>(0,1) = vec[0];
  orig_blk.at<float>(1,0) = vec[1];
  orig_blk.at<float>(2,0) = vec[2];
  orig_blk.at<float>(1,1) = vec[3];
  orig_blk.at<float>(0,2) = vec[4];
  orig_blk.at<float>(0,3) = vec[5];
  orig_blk.at<float>(1,2) = vec[6];
  orig_blk.at<float>(2,1) = vec[7];
}

void vec2blkV2(double vec[8], Mat orig_blk){
  orig_blk.at<float>(1,0) = vec[0];
  orig_blk.at<float>(2,0) = vec[1];
  orig_blk.at<float>(1,1) = vec[2];
  orig_blk.at<float>(0,2) = vec[3];
  orig_blk.at<float>(0,3) = vec[4];
  orig_blk.at<float>(1,2) = vec[5];
  orig_blk.at<float>(2,1) = vec[6];
  orig_blk.at<float>(3,0) = vec[7];
}
