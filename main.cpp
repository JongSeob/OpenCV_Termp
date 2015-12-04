//////////////////////////////////////////////////////////////////////////
// 작성일 : 2015/11/23
// 학  번 : 2011305051
// 이  름 : 윤종섭
// 설  명 : 디지털 영상처리 텀프로젝트
//////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "comctl32.lib")			//- for release purpose when using static library
#pragma comment(lib, "vfw32.lib") 

#include "common.h"
#include "func.h"

#define OUTPUT_PATH "E:/dip_termp/"

char InputFilePath[100]  = "d:/dip/Images/";					// Specify the directory for the file to be saved in
const char InputFileName[50]   = "the_return_of_the_king.avi";	// input video file

char OutputFilePath[6][100] = {OUTPUT_PATH ,				// output file
	OUTPUT_PATH ,
	OUTPUT_PATH ,
	OUTPUT_PATH ,
	OUTPUT_PATH ,
	OUTPUT_PATH }; 

const char OutputFileName[6][50]  = {"video_none.avi" ,				// output file
	"video_hist_equal.avi" ,
	"video_hist_stretch.avi" ,
	"video_gamma.avi" ,
	"video_unsharp.avi" ,
	"video_achromatic.avi" };

VideoCapture * global_cap;

void Menu();

Mat GetHistEqualOnColorImg(const Mat &src);
Mat GetUnsharpImg(const Mat &src);
Mat GetHistStretch(const Mat &src);

void Imadjust(const Mat &input, Mat &output, const double input_range[], const double output_range, const double gamma);

int main(void) 
{
	VideoCapture cap;

	global_cap = &cap;

	//////////////////////////////////////////////////////////////////////////
	// 1. Flags
	//////////////////////////////////////////////////////////////////////////

	int video_proc = VP_NONE;
	bool video_save_flag = false;
	bool video_play = true;

	int speed = 1;

	//////////////////////////////////////////////////////////////////////////
	// 2. File Path
	//////////////////////////////////////////////////////////////////////////

	// define InputFilePath, OutputFilePath

	strcat_s(InputFilePath,  InputFileName);

	for(int i=0; i < 6; i++)
		strcat_s(OutputFilePath[i], OutputFileName[i]);	


	//////////////////////////////////////////////////////////////////////////
	// 3. Window, Trackbar, Mouse
	//////////////////////////////////////////////////////////////////////////

	// Create Window and Trackbar

	int	videoTotalLength = GetVideoLengthInMSEC(InputFilePath);	// Maximum slider value
	int	slider_position = 0;							// Constantly updated slider value

	namedWindow("original", 1);
	createTrackbar("Time(msec)", "original", &slider_position, videoTotalLength, onTrackbarSlide);

	// Set the mouse event callback function

	setMouseCallback("original", mouse_callback);


	//////////////////////////////////////////////////////////////////////////
	// 4. Open The File and Define Video Information Values
	//////////////////////////////////////////////////////////////////////////

	// Open the VideoFile

	OpenVideoFile(cap, InputFilePath);

	Mat frame; // 영상에서 읽어낸 프레임을 저장할 객체

	CvSize FrameSize;

	FrameSize.width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH );
	FrameSize.height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT );

	double fps = cap.get(CV_CAP_PROP_FPS);

	int runningTime; // 현재 프레임이 동영상 실행 후 몇 msec후의 프레임인지를 나타낸다.

	//////////////////////////////////////////////////////////////////////////
	// 5. Ready to write a videofile
	//////////////////////////////////////////////////////////////////////////

	VideoWriter wrt;     // 관심구간을 제외한 비디오를 저장할 객체.

#define	CODEC CV_FOURCC('D', 'I', 'V', 'X')


	//////////////////////////////////////////////////////////////////////////
	// 5. Variables for ImgProc's Trackbars
	//////////////////////////////////////////////////////////////////////////

	// Histogram Stretching Min, Max
	const char histStretchWindowName[50] = {"Histogram Stretch Min, Max"};
	const char histStretchTrackbarName[2][50] = {"Min", "Max"};
	int histStretch_default[2] = {0, 10};
	int histStretch_max = 30;

	// Gamma Change Value 
	const char gammaWindowName[50] = "Gamma";
	const char gammaTrackbarName[50] = "0.1 units ";
	int gamma_default = 0;
	int gamma_max = 30;
	int gamma = gamma_default;


	// Unsharp Masking Sigma
	const char unsharpWindowName[50] = "Unsharp Masking Sigma";
	const char unsharpTrackbarName[50] = "Sigma ";
	int sigma_default = 0;
	int sigma_max = 10;
	int sigma = sigma_default;

	Menu();

	while(1)
	{	
		if (video_play == true)
		{
			cap.read(frame);

			if(frame.empty()) 
			{
				/////////////////////////////////////////////////////////////////////////////////////
				// 동영상이 종료되면 같은 동영상을 재실행. 프로그램이 종료되지 않게 하려고 집어넣었음.
				/////////////////////////////////////////////////////////////////////////////////////

				if(video_save_flag == true)
				{
					wrt.release();
					video_save_flag = false;
				}

				RestartVideo(cap, InputFilePath);

				cap.read(frame);

				system("cls");

				Menu();

				continue;			
			}

			switch (video_proc)
			{
			case VP_HIST_EQUAL   : frame = GetHistEqualOnColorImg(frame);
				break;
			case VP_HIST_STRETCH : frame = GetHistStretch(frame);
				break;
			case VP_GAMMA		 : break;
			case VP_UNSHARP		 : frame = GetUnsharpImg(frame);
				break;
			case VP_ACHROMATIC   : break;
			default				 :  
				break;
			}

			if(video_save_flag == true)
				wrt.write(frame);

			runningTime = cap.get(CV_CAP_PROP_POS_MSEC);

			slider_position = runningTime;

			createTrackbar("Time(msec)", "original", &slider_position, videoTotalLength, onTrackbarSlide);
			//setTrackbarPos("Time(msec)", "original", runningTime); // 이 함수를 이용하면 영상이 버벅거림.

			imshow("original", frame);
		}

		int key;

		key = waitKey(1000.0/(fps*speed));	// 동영상의 fps와 비슷한 속도로 영상을 출력하기 위해 읽는 속도를 조절.

		if(key == 0x1b)
			break;

		

		// 중복선택 된 경우 처음으로 되돌아감
		if( (key - '0') == video_proc )
			continue;
		// 중복선택이 아닌 경우 영상 저장을 종료한다.
		else if( (key - '0') >= VP_NONE && (key - '0') <= VP_ACHROMATIC )
		{
			if(video_save_flag == true)
			{
				wrt.release();
				cout << "VideoWrite finished." << endl;
				video_save_flag = false;
			}
		}

		switch(key)
		{
		case '0'  : video_proc = VP_NONE;			// 원본 영상 출력
			cout << "No Image Processing" << endl;
			break;		
		case '1'  : video_proc = VP_HIST_EQUAL;		// 히스토그램 평활화
			cout << "Histogram Equalization" << endl;
			break;		
		case '2'  : video_proc = VP_HIST_STRETCH;	// 히스토그램 스트레칭
			cout << "Histogram Stretching " << endl;
			break;		
		case '3'  : video_proc = VP_GAMMA;			// 감마 변환
			cout << "Change Gamma " << endl;
			break;		
		case '4'  : video_proc = VP_UNSHARP;		// Unsharp Masking
			cout << "Unsharp Masking" << endl;
			break;		
		case '5'  : video_proc = VP_ACHROMATIC;		// 특정 hue만 취하고 나머지는 무채색
			cout << "Achromatic Color" << endl;
			break;		
		case ' '  : video_play = (video_play == true) ? false : true; break;
		case 's'  : if(video_save_flag == false)
					{
						wrt.open(OutputFilePath[video_proc], CODEC, fps, FrameSize);
						cout << "VideoWrite start." << endl;
						video_save_flag = true;
					}
					else
					{
						wrt.release();
						cout << "VideoWrite finished." << endl;
						video_save_flag = false;
					}
		default   : break;
		}
	} // end of while


	return 0;
}

void Menu()
{
	cout << "=================================================" << endl;
	cout << "==    '0'    : No Image Processing             ==" << endl;
	cout << "==    '1'    : HistogramEqualization           ==" << endl;
	cout << "==    '2'    : HistogramStretching             ==" << endl;
	cout << "==    '3'    : Change Gamma Value              ==" << endl;
	cout << "==    '4'    : UnsharpMasking                  ==" << endl;
	cout << "==    '5'    : Achromatic Color                ==" << endl;
	cout << "==    's'    : video write start / stop        ==" << endl;
	cout << "== SPACE_BAR : video pause / resume            ==" << endl;
	cout << "=================================================" << endl;
}

cv::Mat GetHistEqualOnColorImg( const Mat &src )
{
	cv::Mat dst, gray, tmp;
	cv::Mat Lin, Lout, Cin, Cout;  
	cv::Mat vCin[3], vCout[3];     // vectorCin/out

	// Convert to gray and Apply Histogram Equalization
	cvtColor(src, gray, CV_BGR2GRAY);
	equalizeHist(gray, Lout);

	src.convertTo(Cin, CV_32FC3);
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

cv::Mat GetUnsharpImg( const Mat &src )
{
	Mat img = src;
	// Initialize common variables and objects over the test method sections.
	int ksize = 31;					// Kernel Size. must be odd positive.  Big kernel size causes no problem for simulation. It affects on time during real time implementation.
	float	sigma = 12.0;			// Test for the various sigmas, eg. 3, 7, ... etc. Can you tell the effect of the magnitude of sigma? It affects on local or global aspect of emphasis.
	float	scale = 2.0;			// Test for the various scales, eg. 3, 7, etc.  Can you tell the effect of scale? It affects on the strength of emphasis.

	cv::Mat Blur, dst, dstPositive, dstNegative;
	cv::GaussianBlur(img, Blur, cv::Size(ksize, ksize), sigma);		// kernel size = ksize
	//cv::namedWindow("GaussianBlur");		cv::imshow("GaussianBlur", Blur); 

	img.convertTo(img,CV_16S);
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
	dst = img + scale * (img-Blur);
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

cv::Mat GetHistStretch( const Mat &src )
{
	Mat img = src;

	// Make a table for Histogram streteching
	int	min=30, max=180;
	int		 i,j;
	unsigned char LUT[256];
	for ( i=0; i<min; i++)		LUT[i] = 0;
	for ( i=max; i<=255; i++)	LUT[i] = 255;	
	for ( i=min; i<max; i++)	{	LUT[i] = (unsigned char) ( (float)i * 255.0 / (float)(max-min) - (255.0 * min / (float(max-min) ) ) );}

	for (j =0; j<img.rows; j++) 
		for (i =0; i<img.cols; i++) {
			img.at<cv::Vec3b>(j,i)[0] = LUT[ img.at<cv::Vec3b>(j,i)[0] ];	
			img.at<cv::Vec3b>(j,i)[1] = LUT[ img.at<cv::Vec3b>(j,i)[1] ];	
			img.at<cv::Vec3b>(j,i)[2] = LUT[ img.at<cv::Vec3b>(j,i)[2] ];	
		}

		return img;
}
