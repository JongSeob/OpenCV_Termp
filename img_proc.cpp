#include "common.h"
#include "img_proc.h"
#include "func.h"

// main.cpp
extern int stretchMin;
extern int stretchMax;
extern double gamma;
extern int sigma;
extern int margin;

extern Point mouse_click_point; // func.cpp

Mat histSretchLut(1, 256, CV_8UC1);

cv::Mat GetHistEqualOnColorImg( const Mat &img )
{
	cv::Mat src = img;
	cv::Mat dst, gray, tmp;
	cv::Mat Lin, Lout, Cin, Cout;  
	cv::Mat vCin[3], vCout[3];     // vectorCin/out

	// Convert to gray and Apply Histogram Equalization
	cvtColor(img, gray, CV_BGR2GRAY);
	equalizeHist(gray, Lout);

	img.convertTo(Cin, CV_32FC3);
	cv::split(Cin, vCin);					// vCin[0] = B, vCin[1] = G, vCin[0] = R, 

	gray.convertTo(Lin,CV_32FC1);			// 32 bit data
	Lout.convertTo(Lout,CV_32FC1);			// 32 bit data

	// 아래 연산은 안되는 듯함. runtime 오류 *** 검토 필요. 
	//vCout[0] = Lout * (vCin[0]/Lin); 				//vCout[1] = Lout * (vCin[1]/Lin);				//vCout[2] = Lout * (vCin[2]/Lin);

	//double gamma = 0.01;			// 흑백으로 칼라 강도가 약해짐.
	//double gamma = 5.01;			// 칼라 강도가 매우 강해짐.
	//double gamma = 1.5;				// 선명한 칼라.
	//double gamma = 0.6;				// 약한 칼라
	double gamma = 1.0;				// No strength control
	cv::divide(vCin[0], Lin, tmp);		cv::pow(tmp, gamma, tmp); cv::multiply(Lout, tmp, vCout[0]);
	cv::divide(vCin[1], Lin, tmp);		cv::pow(tmp, gamma, tmp); cv::multiply(Lout, tmp, vCout[1]);
	cv::divide(vCin[2], Lin, tmp);		cv::pow(tmp, gamma, tmp); cv::multiply(Lout, tmp, vCout[2]);

	cv::merge(vCout, 3, dst);			// Merge 3 planes to a Mat.
	dst.convertTo(dst,CV_8UC3);

	return dst;
}

cv::Mat GetUnsharpImg( const Mat &img )
{
	Mat src = img;
	// Initialize common variables and objects over the test method sections.
	int ksize = 31;					// Kernel Size. must be odd positive.  Big kernel size causes no problem for simulation. It affects on time during real time implementation.
	float	sigma = 12.0;			// Test for the various sigmas, eg. 3, 7, ... etc. Can you tell the effect of the magnitude of sigma? It affects on local or global aspect of emphasis.
	float	scale = 2.0;			// Test for the various scales, eg. 3, 7, etc.  Can you tell the effect of scale? It affects on the strength of emphasis.

	cv::Mat Blur, dst, dstPositive, dstNegative;
	cv::GaussianBlur(src, Blur, cv::Size(ksize, ksize), sigma);		// kernel size = ksize
	//cv::namedWindow("GaussianBlur");		cv::imshow("GaussianBlur", Blur); 

	src.convertTo(src,CV_16S);
	Blur.convertTo(Blur, CV_16S);

#if 0
	//1) Show high frequence components, Unsharp mask
	// Unsharp mask = original image - Bluurred image
	// The unsharp mask is identified later as a negative 2nd derivative of the origonal image.
	dst = scale * (src-Blur);						// Unsharp mask
	dst.convertTo(dstPositive,CV_8U,1);
	dst.convertTo(dstNegative,CV_8U,-1);			// dstNegative contains Negative part of dst.

	cv::namedWindow("Positive Part of Unsharp Mask(original-Blur)"); 	cv::imshow("Positive Part of Unsharp Mask(original-Blur)",dstPositive);
	cv::namedWindow("Negative Part of Unsharp Mask(original-Blur)"); 	cv::imshow("Negative Part of Unsharp Mask(original-Blur)",dstNegative);
#endif


	// 2) The unsharp mask is (original-Blur). Add it to original image.
	dst = src + scale * (src-Blur);
	dst.convertTo(dstPositive,CV_8U,1);
	dst.convertTo(dstNegative,CV_8U,-1);			// dstNegative contains Negative part of dst.

	//cv::namedWindow("Positive: original+UnsharpMask(original-Blur)"); 	cv::imshow("Positive: original+UnsharpMask(original-Blur)",dstPositive);
	//cv::namedWindow("Negative: original+UnsharpMask(original-Blur)"); 	cv::imshow("Negative: original+UnsharpMask(original-Blur)",dstNegative);

#if 0
	// 3) If we subtract the 2nd derivative from the original image, the output image is sharpened. 
	// Through trial and error, I found that the (Blur-original) is the 2nd derivative of the original image.
	dst = src - scale * (Blur-src);
	dst.convertTo(dstPositive,CV_8U,1);
	dst.convertTo(dstNegative,CV_8U,-1);			// dstNegative contains Negative part of dst.

	cv::namedWindow("Positive: original-2ndDerivative(Blur-original)"); 	cv::imshow("Positive: original-2ndDerivative(Blur-original)",dstPositive);
	cv::namedWindow("Negative: original-2ndDerivative(Blur-original)"); 	cv::imshow("Negative: original-2ndDerivative(Blur-original)",dstNegative);
#endif

	return dstPositive;	
}

cv::Mat GetHistStretch( const Mat &img )
{
	Mat src = img;
	Mat dst;
	Mat tmp;

	Mat gray;
	Mat L_in, L_out;

	Mat Cin, Cout;
	Mat vCin[3], vCout[3];
	
	// Convert to gray and Histogram Stretching
	
	cvtColor(src, gray, CV_BGR2GRAY);
	
	LUT(gray, histSretchLut, L_out);

	// gray to color channel

	img.convertTo(Cin, CV_32FC3);      // Cin = original color image

	split(Cin, vCin);

	gray.convertTo(L_in, CV_32FC1);    // L_in  = original gray     (Lut in)
	L_out.convertTo(L_out, CV_32FC1);  // L_out = gray HistStretch  (Lut out)
		
	for(int i = 0; i < 3; i++)
	{
		// vCout[i] = Lout * (vCin[i] / L_in) ^ gamma
		divide(vCin[i], L_in, tmp);			//  vCin[i] / L_in
		pow(tmp, gamma, tmp);				//  (vCin[i] / L_in) ^ gamma
		multiply(L_out, tmp, vCout[i]);		//  Lout * (vCin[i] / L_in) ^ gamma
	}

	merge(vCout, 3, dst);
	dst.convertTo(dst, CV_8UC3);	

	return dst;
}

cv::Mat GetGammaChangedImg( const Mat &img )
{
	Mat src = img;
	
	Mat dst;
	
	src.convertTo(src, CV_32FC3, 1.0 / 255.0);
	
	cv::pow(src, 1 / gamma, dst);
	
	dst.convertTo(dst, CV_8UC3, 255);

	return dst;
}

cv::Mat GetAchromaticImg( const Mat &img )
{
	cv::Mat src = img;
	
	Mat vHSV_Origin[3];
	Mat vHSV_Ach[3];
	
	// 마우스로 클릭한 픽셀 좌표
	static Point point;

	cvtColor(src, src, CV_BGR2HSV);

	split(src, vHSV_Origin);
	split(src, vHSV_Ach);

	// Color to Achromatic

	vHSV_Ach[1] = 0;

	// 새로운 위치를 클릭하면 point의 값을 갱신
	if(point != ::mouse_click_point)
	{
		point = mouse_click_point;
		cout << "갱신된 point 값 : " << point << endl;
	}

	// 클릭한 픽셀의 Hue 값
	int clicked_hue = vHSV_Origin[0].at<int>(point);

	if( point.x > 0 && point.y > 0)
	{
		for(int height = 0; height < img.size().height; height++)
		{
			for(int width = 0; width < img.size().width; width++)
			{
				// 현재 픽셀의 Hue 값이 clicked_hue 와 10이상 차이가 나지 않는다면
				// 원래 채도로 되돌린다.
				if( (vHSV_Origin[0].at<int>(height, width) >= std::max(clicked_hue-200000000, 0) )  &&
					(vHSV_Origin[0].at<int>(height, width) < clicked_hue+300000000 ) 
					)
				{
					vHSV_Ach[1].at<int>(height,width) = vHSV_Origin[1].at<int>(height,width);
				}
			}
		}
	}

	cout << "2중 for문 종료" << endl;

	
	Mat dst;

	merge(vHSV_Ach, 3, dst);

	cvtColor(dst, dst, CV_HSV2BGR);

	return dst;
}

void UpdateLut( Mat &lut, const int low, const int high )
{
	if(low == high)
	{
		cout << "low == high is incorrect" << endl;
		return;
	}

	for(int i = 0; i < low; i++)
		lut.at<uchar>(i) = 0;

	for(int i = high; i < 256; i++)
		lut.at<uchar>(i) = 255;

	// low ~ high-1 사이의 개수 //
	int amount = (high - 1) - low;

	for(int i = low; (i >= low) && (i < high); i++)
	{
		// i == low ~ high-1  =>   i - low ==  0 ~ amount// 
		lut.at<uchar>(i) = (255 / amount) * (i - low);
	}
}

cv::Mat Imadjust( const Mat &src, Mat &dst, const double input_range[], const double output_range[], const double gamma /*= 1*/ )
{
	if(input_range[0] < 0 || input_range[0] > 1 ||
		input_range[1] < 0 || input_range[1] > 1 ||
		output_range[0] < 0 || output_range[0] > 1 ||
		output_range[1] < 0 || output_range[1] > 1		
		)
	{
		cout << "invalid range" << endl;
	}
	
	LUT(src, histSretchLut, dst);

	// intensity transform
/*
	for( y = 0; y < img_size.height; y++)
	{
		for (x = 0; x < img_size.width; x++)
		{
			val = ((uchar*)(img.data + src->widthStep*y))[x]; 
			val = pow((val - input_range[0])/err_in, gamma) * err_out + output_range[0];
			if(val>255) val=255; if(val<0) val=0; // Make sure src is in the range [low,high]
			((uchar*)(dst->imageData + dst->widthStep*y))[x] = (uchar) val;
	}
*/
	return dst;
}

