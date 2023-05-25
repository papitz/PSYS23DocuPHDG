#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {

    const int rows = 500;
    const int cols = 500;

    cv::Mat matrix(rows, cols, CV_8UC3);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cv::Vec3b& pixel = matrix.at<cv::Vec3b>(i, j);
            pixel[0] = rand() % 256; // blue component
            pixel[1] = rand() % 256; // green component
            pixel[2] = rand() % 256; // red component
        }
    }

    // Display the matrix
    cv::imshow("Heat Matrix", matrix);
    cv::waitKey();

    return 0;
}
