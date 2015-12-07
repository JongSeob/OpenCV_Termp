#ifndef _img_proc_h_
#define _img_proc_h_

Mat GetHistEqualOnColorImg(const Mat &img);
Mat GetUnsharpImg(const Mat &img);
Mat GetHistStretch(const Mat &img);
Mat GetGammaChangedImg(const Mat &img);
Mat GetAchromaticImg(const Mat &img);

void UpdateLut(Mat &lut, const int low, const int high);

Mat Imadjust( const Mat &src, Mat &dst, const double input_range[], const double output_range[], const double gamma /*= 1*/ );

#endif // img_proc_h__
