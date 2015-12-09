#include "common.h"
#include "img_proc.h"
#include "func.h"
#include <Windows.h>

// GPLOT_PATH : GnuPlot �������� ������
// GPLOT_DRAW_HISTOGRAM_SCRIPT_FILE_PATH : ������ GnuPlot ��ɾ ������ �ִ� ��ũ��Ʈ ����
#define GPLOT_PATH									L"C:\\Program Files\\gnuplot\\bin\\wgnuplot.exe"
#define GPLOT_DRAW_HISTOGRAM_SCRIPT_FILE_PATH		L"E:\\dip_termp\\RunGnuPlotScript.txt"

// main.cpp
extern double stretchMin;
extern double stretchMax;
extern double gamma;
extern int sigma;
extern int margin;

extern int key;

extern Point mouse_click_point; // func.cpp

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

	double gamma = 1.0;				// No strength control
	cv::divide(vCin[0], Lin, tmp);		cv::pow(tmp, gamma, tmp); cv::multiply(Lout, tmp, vCout[0]);
	cv::divide(vCin[1], Lin, tmp);		cv::pow(tmp, gamma, tmp); cv::multiply(Lout, tmp, vCout[1]);
	cv::divide(vCin[2], Lin, tmp);		cv::pow(tmp, gamma, tmp); cv::multiply(Lout, tmp, vCout[2]);

	cv::merge(vCout, 3, dst);			// Merge 3 planes to a Mat.
	dst.convertTo(dst,CV_8UC3);
	
	if(key == 'h' || key == 'H')
	{
		cout << "Draw Histogram" << endl;

		// �ȼ����� ������ ������ �迭.
		int histogram[256] = {0, };
		
		// �� �ȼ����� ������ ����.
		for(int row = 0; row < gray.size().height; row++)
		{
			for(int column = 0; column < gray.size().width; column++)
			{
				histogram[gray.at<uchar>(row, column)]++;
			}
		}

		// GnuPlot�� �̿��� ������׷����� ����� ����������
 		FILE * fp = fopen("../histogram_data.dat", "w");

		// �ȼ��� �ȼ�����
		for(int i = 0; i < 256; i++)
			fprintf(fp, "%d %d\n", i, histogram[i]);
 
 		fclose(fp);
		
		ShellExecute(NULL, L"open", GPLOT_PATH, \
			GPLOT_DRAW_HISTOGRAM_SCRIPT_FILE_PATH, NULL, SW_SHOWNORMAL);
	}
	
	return dst;
}

cv::Mat GetUnsharpImg( const Mat &img )
{
	Mat src = img;
	
	static int ksize_arr[7] = {11, 19, 31, 41, 53, 65, 71};

	float	sigma = (float)(::sigma);//12.0;			// Test for the various sigmas, eg. 3, 7, ... etc. Can you tell the effect of the magnitude of sigma? It affects on local or global aspect of emphasis.
	float	scale = 2.0;			// Test for the various scales, eg. 3, 7, etc.  Can you tell the effect of scale? It affects on the strength of emphasis.
	int ksize = ksize_arr[ (int)(sigma / 5) ];
		
	cv::Mat Blur, dst, dstPositive, dstNegative;
	cv::GaussianBlur(src, Blur, cv::Size(ksize, ksize), sigma);		// kernel size = ksize
	
	src.convertTo(src,CV_16S);
	Blur.convertTo(Blur, CV_16S);
	
	dst = src + scale * (src-Blur);
	dst.convertTo(dstPositive,CV_8U,1);
	dst.convertTo(dstNegative,CV_8U,-1);			// dstNegative contains Negative part of dst.

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
	
	double input_range[2] = {stretchMin, stretchMax};
	double output_range[2] = {0, 1};

	Imadjust(gray, L_out, input_range, output_range, 1);

	// gray to color channel

	img.convertTo(Cin, CV_32FC3);      // Cin = original color image

	split(Cin, vCin);

	gray.convertTo(L_in, CV_32FC1);    // L_in  = original gray     (Lut in)
	L_out.convertTo(L_out, CV_32FC1);  // L_out = gray HistStretch  (Lut out)
		
	for(int i = 0; i < 3; i++)
	{
		// vCout[i] = Lout * (vCin[i] / L_in) ^ gamma
		divide(vCin[i], L_in, tmp);			//  vCin[i] / L_in
		multiply(L_out, tmp, vCout[i]);		//  Lout * (vCin[i] / L_in) ^ gamma
	}

	merge(vCout, 3, dst);
	dst.convertTo(dst, CV_8UC3);	

	return dst;
}

cv::Mat GetGammaChangedImg( const Mat &img )
{
	Mat src;
	img.copyTo(src);
	
	Mat dst;

	double input_range[2] = {0, 1};
	double output_range[2] = {0, 1};

	Imadjust(src, dst, input_range, output_range, gamma);
	
	src.convertTo(src, CV_32FC3, 1.0 / 255.0);
	
	cv::pow(src, gamma, dst);
	
	dst.convertTo(dst, CV_8UC3, 255);

	return dst;
}

int GetSaturationSigma( const Mat &img )
{
	Mat src;	
	img.copyTo(src);

	Mat vHSV[3];
	
	cvtColor(src, src, CV_BGR2HSV);
	
	split(src, vHSV);

	int average = 0; // ���
	int sum = 0;     // �հ�	
	int differ = 0; // ����
	double variance = 0; // �л�
	double sigma = 0;  // ǥ������
	int countOfPixel = src.size().width * src.size().height; // �� ����
	
	// Saturation ���� ����� ���Ѵ�. //
	for(int row = 0; row < src.size().height; row++)
	{
		for (int column = 0; column < src.size().width; column++)
		{
			sum += vHSV[1].at<uchar>(row, column);
		}
	}

	average = sum / countOfPixel;


	// ��� ���� �̿��� �л��� ���Ѵ�. //

	sum = 0;

	for(int row = 0; row < src.size().height; row++)
	{
		for (int column = 0; column < src.size().width; column++)
		{
			differ = vHSV[1].at<uchar>(row, column) - average; // �� �ȼ��� ������ ���Ѵ�.
			sum += differ * differ;							   // ���� �������� ���� ���Ѵ�.
		}
	}

	// ���� ������ ���(�л�) �� ���Ѵ�.
	variance = sum / countOfPixel;

	// �л��� ������(ǥ������)�� ���Ѵ�.
	sigma = sqrt(variance);

	return (int)sigma;
}

cv::Mat GetAchromaticImg( const Mat &img )
{
	cv::Mat src;
	img.copyTo(src);
	
	Mat vHSV_Origin[3];
	Mat vHSV_Ach[3];
	
	// ���콺�� Ŭ���� �ȼ� ��ǥ
	static Point point;

	// ROI ���� = ���콺 �����͸� �߽����� �� 30x30 �簢��
	static Rect ROI_rect(img.size().width / 2, img.size().height / 2, 10, 10);
	static Mat ROI_img = img(ROI_rect);

	cvtColor(src, src, CV_BGR2HSV);

	split(src, vHSV_Origin);
	split(src, vHSV_Ach);

	// Color to Achromatic

	vHSV_Ach[1] = 0;

	// Ŭ���� �ȼ��� Hue, Saturation ��
	static int clicked_hue;
	static int clicked_saturation;

	// ���ο� ��ġ�� Ŭ���ϸ� point�� ���� ����
	if(point != ::mouse_click_point)
	{
		point = mouse_click_point;
		cout << "���ŵ� point �� : " << point << endl;
		
		// Ŭ���� �ȼ��� Hue, Saturation ��
		clicked_hue = vHSV_Origin[0].at<uchar>(point);
		clicked_saturation = vHSV_Origin[1].at<uchar>(point);

		cout << "Ŭ���� �ȼ��� Hue �� :" << (unsigned int)clicked_hue << endl;

		ROI_rect.width = 50;
		ROI_rect.height = 50;
		ROI_rect.x = max(point.x-(ROI_rect.width/2 -1), 0);
		ROI_rect.y = max(point.y-(ROI_rect.height/2 -1), 0);

		// �簢���� �̹����� ����� ��� �̹��� �������� �������� ��ǥ�� �̵��Ѵ�.
		ROI_rect.x = ((ROI_rect.x + ROI_rect.width) >= src.size().width) ? (src.size().width - ROI_rect.width) : ROI_rect.x;
		ROI_rect.y = ((ROI_rect.y + ROI_rect.height) >= src.size().height) ? (src.size().height - ROI_rect.height) : ROI_rect.y;

		cout << "rect = " << ROI_rect << endl;
		
		ROI_img = img(ROI_rect);
	}

	int sigma = 0;

	sigma = GetSaturationSigma(ROI_img);
	
	cout << "sigma = " << sigma << endl;

	if( point.x > 0 && point.y > 0)
	{
		for(int height = 0; height < img.size().height; height++)
		{
			for(int width = 0; width < img.size().width; width++)
			{
				// ���� �ȼ��� Hue ���� clicked_hue �� 10�̻� ���̰� ���� �ʴ´ٸ�
				// ���� ä���� �ǵ�����.
				if( (vHSV_Origin[0].at<uchar>(height, width) >= max(clicked_hue - margin, 0) )			   &&
					(vHSV_Origin[0].at<uchar>(height, width) <  min(clicked_hue + margin, 255) )		   &&
					(vHSV_Origin[1].at<uchar>(height, width) >= max(clicked_saturation - sigma - margin, 0) )  &&
					(vHSV_Origin[1].at<uchar>(height, width) <  min(clicked_saturation + sigma + margin, 255) )
					)
				{					
					vHSV_Ach[1].at<uchar>(height,width) = vHSV_Origin[1].at<uchar>(height,width);

				}
			}
		}
	}
	
	Mat dst;

	merge(vHSV_Ach, 3, dst);

	cvtColor(dst, dst, CV_HSV2BGR);

	return dst;
}

void UpdateLut( Mat &lut, const int low, const int high, const int min, const int max )
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

	// low ~ high ������ ���� //
	int amount = high - low;

	// Output Scale�� Input Scale ũ��� ����. 
	int step = (max - min) / amount;

	for(int i = low; (i >= low) && (i < high); i++)
	{
		// i == low ~ high-1  =>   i - low ==  0 ~ amount// 
		lut.at<uchar>(i) = step * (i - low);
	}
}

cv::Mat Imadjust( const Mat &src, Mat &dst, const double input_range[], const double output_range[], const double gamma /*= 1*/ )
{
	if(input_range[0] < 0 || input_range[0] > 1 ||
		input_range[1] <= input_range[0] || input_range[1] > 1 ||
		output_range[0] < 0 || output_range[0] > 1 ||
		output_range[1] <= output_range[0] || output_range[1] > 1		
		)
	{
		cout << "invalid range" << endl;
		exit(0);
	}

	Mat lut(1, 256, CV_8UC1);
	
	UpdateLut(lut, (int)(255*input_range[0]), (int)(255*input_range[1]), (int)(255*output_range[0]), (int)(255*output_range[1]));
			
	LUT(src, lut, dst);

	dst.convertTo(dst, CV_32FC3, 1.0 / 255.0);

	pow(dst, gamma, dst);

	dst.convertTo(dst, CV_8UC3, 255);

	return dst;
}


