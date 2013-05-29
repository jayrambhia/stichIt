#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/nonfree/nonfree.hpp>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

int stitchImages(Mat& img1, Mat& img2, Mat& stitchedImage, int move)
{
    if (move <= 0)
    {
        stitchedImage = img1;
        return 1;
    }
    
    Mat g1(img1, Rect(0, 0, img1.cols, img1.rows));
    Mat g2(img2, Rect(0, 0, move+300, img2.rows));
    cvtColor(g1, g1, CV_BGR2GRAY);
    cvtColor(g2, g2, CV_BGR2GRAY);

    vector<KeyPoint> keypoints_roi, keypoints_img;  /* keypoints found using SIFT */
    Mat descriptor_roi, descriptor_img;             /* Descriptors for SIFT */
    FlannBasedMatcher matcher;                      /* FLANN based matcher to match keypoints */
    vector<DMatch> matches, good_matches;
    SIFT sift;
    int i, dist=80;
    
    sift(g1, Mat(), keypoints_roi, descriptor_roi);      /* get keypoints of ROI image */
    sift(g2, Mat(), keypoints_img, descriptor_img);         /* get keypoints of the image */
    printf("%ld no. of keypoints in right image\n", keypoints_img.size());
    matcher.match(descriptor_roi, descriptor_img, matches);

    double max_dist = 0; double min_dist = 1000;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptor_roi.rows; i++ )
    { 
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    for (i=0; i < descriptor_roi.rows; i++)
    {
        if (matches[i].distance < 3*min_dist)
        {
            good_matches.push_back(matches[i]);
        }
    }
    printf("%ld no. of matched keypoints in right image\n", good_matches.size());
    /* Draw matched keypoints */
    /*
    Mat img_matches;
    drawMatches(img1, keypoints_roi, img2, keypoints_img, 
                good_matches, img_matches, Scalar::all(-1), 
                Scalar::all(-1), vector<char>(), 
                DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    */
    vector<Point2f> keypoints1, keypoints2; 
    for (i=0; i<good_matches.size(); i++)
    {
        keypoints1.push_back(keypoints_img[good_matches[i].trainIdx].pt);
        keypoints2.push_back(keypoints_roi[good_matches[i].queryIdx].pt);
    }
    //printf("%ld %ld\n", keypoints1.size(), keypoints2.size());
    Mat H = findHomography( keypoints1, keypoints2, CV_RANSAC );
    
    int mRows = img2.rows;
    if (img1.rows> img2.rows)
    {
        mRows = img1.rows;
    }
    stitchedImage = Mat::zeros(img2.cols+img1.cols, mRows, CV_8UC3);
    warpPerspective(img2,stitchedImage,H,Size(img2.cols+img1.cols,mRows));
    Mat half(stitchedImage,Rect(0,0,img1.cols,img1.rows));
    img1.copyTo(half);
    //GaussianBlur(stitchedImage, stitchedImage, Size(3,3),0);

    return (1);
}