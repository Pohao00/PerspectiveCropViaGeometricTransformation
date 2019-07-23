#include <cstdio>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

const float nOffset = 0, mOffset = -400; // 設定位移幅度
float tra[8] = {0};
float aa[3][3] = { {tra[0], tra[3], tra[6] },
{tra[1], tra[4], tra[7] },
{tra[2], tra[5],1} };
int temp = 0;

int arrayreverse(float A[8][16]) {
	for (int i = 0; i < 8; ++i) {
    if (A[i][i] == 0)
      for (int j=i+1; j<8; ++j)
        if (A[j][i] != 0) {
          for (int k=i; k < 8 * 2; ++k)
            swap(A[i][k], A[j][k]);
          break;
        }
        // 反矩陣不存在。
        if (A[i][i] == 0) return false;
        double t = A[i][i];
        for (int k = i; k < 8 * 2; ++k)
          A[i][k] /= t;
        // 消去時，所有的row都消去。
        for (int j = 0; j < 8; ++j)
          if (i != j && A[j][i] != 0)
          {
            double t = A[j][i];
            for (int k = i; k < 8 * 2; ++k)
              A[j][k] -= A[i][k] * t;
          }
  }

  for(int i = 8; i < 8 * 2; i++) {  //印出反矩陣！
    for(int j = 8 ; j < 8 * 2; j++) {
      cout << (int) A[i][j] << "  ";
    }   
    printf("\n");
  }
}

void asolution(float A[8][16], float B[8]) {
	for(int i = 0; i < 8; i++) {
    int l = 0;
    for(int j = 8; j < 8 * 2; j++) { 
      tra[i] = (A[i][j] * B[l] ) + tra[i];
      l++;
    }
  }
  aa[0][0] = tra[0];
  aa[1][0] = tra[1];
  aa[2][0] = tra[2];
  aa[0][1] = tra[3];
  aa[1][1] = tra[4];
  aa[2][1] = tra[5];
  aa[0][2] = tra[6];
  aa[1][2] = tra[7];

  for(int i = 0; i < 3; i++) {  //印出反矩陣！
    for(int j = 0; j < 3; j++) {
       cout << aa[i][j] << "  ";
    }
    printf("\n");
  }
}

void imgtrans(Mat src_img, Mat image2) {
  for(int i = 0; i < src_img.rows; i++) {	
    for(int j = 0; j < src_img.cols; j++) {
      int p1 = 0, p2 = 0;
      float denominator = aa[0][2] * i + aa[1][2] * j + aa[2][2];  
      p1 = (aa[0][0] * i + aa[1][0] * j + aa[2][0]) / denominator;  
      p2 = (aa[0][1] * i + aa[1][1] * j + aa[2][1]) / denominator;  
      if(p1 < 0 || p1 > (src_img.rows - 1) || p2 < 0 || p2 > (src_img.cols - 1) )  
        continue; 
      image2.at<Vec3b>(i, j)[0] = (src_img.at<Vec3b>(p1, p2)[0] );
      image2.at<Vec3b>(i, j)[1] = (src_img.at<Vec3b>(p1, p2)[1] );
      image2.at<Vec3b>(i, j)[2] = (src_img.at<Vec3b>(p1, p2)[2] );
    }
  }
  
  namedWindow("src", CV_WINDOW_NORMAL);
  namedWindow("dst", CV_WINDOW_NORMAL);
  imshow("src", src_img);
  imshow("dst", image2);
  imwrite("big banana.jpg",image2);

  waitKey(0);
}

int main(int argc, char *argv[] ) {
  Mat src_img = imread("123.jpg");
  Mat image2(src_img.rows, src_img.cols, CV_8UC3);
  
  if(src_img.empty() ) return -1;

  int w = src_img.cols,h = src_img.rows;
  float A[8][16] = { {0, 0, 1, 0, 0, 0, 0, 0,             1, 0, 0, 0, 0, 0, 0, 0},
  {w, 0, 1, 0, 0, 0, -1 * w * (w- nOffset), 0,   0, 1, 0, 0, 0, 0, 0, 0},
  {w, h, 1, 0, 0, 0, -1 * w * w, -1 * w * h,                     0, 0, 1, 0, 0, 0, 0, 0},
  {0, h, 1, 0, 0, 0, 0, 0,             0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0,                0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, w, 0, 1, -1 * w * mOffset, 0,                0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, w, h, 1, -1 * (h - mOffset) * w, -1 * h * h,                     0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, h, 1, 0, -1 * h * (h),   0, 0, 0, 0, 0, 0, 0, 1}
  };

  int AT[8][8] = { {1, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 0, 0, 0, 1}
  };
  float B[8] = {nOffset, w - nOffset - mOffset / 2, w - mOffset / 2, 0,          (0), mOffset, (h - mOffset), (h)};

  arrayreverse(A);
  asolution(A, B);
  imgtrans(src_img, image2);

  return 0;
}