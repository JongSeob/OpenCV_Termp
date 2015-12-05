#include "common.h"
#include "img_proc.h"
#include "func.h"

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

	// �Ʒ� ������ �ȵǴ� ����. runtime ���� *** ���� �ʿ�. 
	//vCout[0] = Lout * (vCin[0]/Lin); 				//vCout[1] = Lout * (vCin[1]/Lin);				//vCout[2] = Lout * (vCin[2]/Lin);

	//double gamma = 0.01;			// ������� Į�� ������ ������.
	//double gamma = 5.01;			// Į�� ������ �ſ� ������.
	//double gamma = 1.5;				// ������ Į��.
	//double gamma = 0.6;				// ���� Į��
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

	// Make a table for Histogram streteching
	int	min=30, max=180;
	int		 i,j;
	unsigned char LUT[256];
	for ( i=0; i<min; i++)		LUT[i] = 0;
	for ( i=max; i<=255; i++)	LUT[i] = 255;	
	for ( i=min; i<max; i++)	{	LUT[i] = (unsigned char) ( (float)i * 255.0 / (float)(max-min) - (255.0 * min / (float(max-min) ) ) );}

	for (j =0; j<src.rows; j++) 
		for (i =0; i<src.cols; i++) {
			src.at<cv::Vec3b>(j,i)[0] = LUT[ src.at<cv::Vec3b>(j,i)[0] ];	
			src.at<cv::Vec3b>(j,i)[1] = LUT[ src.at<cv::Vec3b>(j,i)[1] ];	
			src.at<cv::Vec3b>(j,i)[2] = LUT[ src.at<cv::Vec3b>(j,i)[2] ];	
		}

		return src;
}

cv::Mat GetGammaChangedImg( const Mat &img )
{
	Mat src = img;
	Mat dst;

	double gamma = 1.3;

	// scaled [0, 255] to [0, 1.0]
	src.convertTo(dst, CV_8UC3, 1.0 / 255.0);

#if 0
	img.convertTo(img, CV_32FC3);

	//cv::pow(img, gamma, dst);

	imshow("gamma !", dst);

	dst.convertTo(dst, CV_8UC3);
#endif

	dst = src * gamma;

	return dst;
}

cv::Mat GetAchromaticImg( const Mat &img )
{
	cv::Mat src = img;//imread("D:/dip/images/colorbar_chart.jpg");//img;

	// image*alpha + beta
	src.convertTo(src, CV_32F,1.0/255.0);

	//image.convertTo(image, CV_32F);
	cv::namedWindow("Input Image");	cv::imshow("Input Image",src);

	cv::Mat dst, dst2;
	cvtColor(src, dst, CV_BGR2HLS);

	// Split the image
	std::vector<cv::Mat> imageHLS, HLS;					// Vector whose elements are Mat(s).
	cv::split(dst, imageHLS);							// imageHLS[0], imageHLS[1], imageHLS[2]
	cv::split(dst, HLS);			
	//HLS=imageHLS;					Do not use!!!! They point to the same data content. This assignment does not copy data.

	//imageHLS[0].copyTo(HLS[0]);		// Q: Can you tell why this does not work?
	//imageHLS[1].copyTo(HLS[1]); imageHLS[2].copyTo(HLS[2]);

	HLS[0] += 90;					// Rotate hue 90 degrees in counterclockwise
	cv::merge(HLS, dst2);
	cvtColor(dst2, dst2, CV_HLS2BGR);
	cv::namedWindow("Hue rotated by 90 degrees"); cv::imshow("Hue rotated by 90 degrees",dst2);


	HLS[0] += 90;					// Rotate hue 90 degrees again. total 180.
	cv::merge(HLS, dst2);
	cvtColor(dst2, dst2, CV_HLS2BGR);
	cv::namedWindow("Hue rotated by 180 degrees"); cv::imshow("Hue rotated by 180 degrees",dst2);

	HLS[0] += 90;					// Rotate hue 90 degrees again. total 270. 
	cv::merge(HLS, dst2);
	cvtColor(dst2, dst2, CV_HLS2BGR);
	cv::namedWindow("Hue rotated by 270 degrees"); cv::imshow("Hue rotated by 270 degrees",dst2);

	HLS[0] += 90;					// Rotate hue 90 degrees again. total 360.
	cv::merge(HLS, dst2);
	cvtColor(dst2, dst2, CV_HLS2BGR);
	cv::namedWindow("Hue rotated by 360 degrees"); cv::imshow("Hue rotated by 360 degrees",dst2);

	HLS[0] += 90;					// Rotate hue 90 degrees again. total 360+90.
	cv::merge(HLS, dst2);
	cvtColor(dst2, dst2, CV_HLS2BGR);
	cv::namedWindow("Hue rotated by 360+90 degrees"); cv::imshow("Hue rotated by 360+90 degrees",dst2);
	
// ���������� �ϴ°�� �����޽����� �Բ� ����.
#if 0
	imageHLS[0] -= 90;					// Rotate hue 90 degrees in clockwise. must be the same as +270.
	cv::merge(imageHLS, dst2);
	cvtColor(dst2, dst2, CV_HLS2BGR);
	cv::namedWindow("Hue rotated by -90 degrees"); cv::imshow("Hue rotated by -90 degrees",dst2);

	imageHLS[0] -= 90;					// Rotate hue 90 degrees again in clockwise. must be the same as +180.
	cv::merge(imageHLS, dst2);
	//cvtColor(dst2, dst2, CV_HLS2BGR);
	cv::namedWindow("Hue rotated by -180 degrees"); cv::imshow("Hue rotated by -180 degrees",dst2);
#endif
	
	return img;
}