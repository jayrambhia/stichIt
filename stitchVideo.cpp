#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/nonfree/nonfree.hpp>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "stitchimages.cpp"

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap1 = VideoCapture(1);
    VideoCapture cap2 = VideoCapture(2);

    int k;
    int move=0;
    Mat img1, img2, stitchedImage;
    while (1)
    {
        cap1 >> img1;
        cap2 >> img2;
        if (stitchImages(img2, img1, stitchedImage, move))
        {
            Mat feedImage(stitchedImage, Rect(move, 0, img1.cols, img1.rows));
            imshow("Stitched Image", stitchedImage);
            imshow("feed", feedImage);
        }
        k = waitKey(5);
        if (k == 27)
        {
            break;
        }
        else if (k == 'd')
        {
            if (move < 200)
            {
                move+=20;
            }
        }
        else if (k == 'a')
        {
            if (move > 0)
            {
                move-=20;
            }
        }
    }
}