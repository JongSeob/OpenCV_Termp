//////////////////////////////////////////////////////////////////////////
// �ۼ��� : 2015/11/23
// ��  �� : 2011305051
// ��  �� : ������
// ��  �� : ������ ����ó�� ��������Ʈ
//////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "comctl32.lib")			//- for release purpose when using static library
#pragma comment(lib, "vfw32.lib") 

#include "common.h"
#include "func.h"
#include "img_proc.h"

#define OUTPUT_PATH "E:/dip_termp/"

char InputFilePath[100]  = "d:/dip/Images/";					// Specify the directory for the file to be saved in
const char InputFileName[50]   = "bike.avi";//"the_return_of_the_king.avi";	// input video file

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

	Mat frame; // ���󿡼� �о �������� ������ ��ü

	CvSize FrameSize;

	FrameSize.width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH );
	FrameSize.height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT );

	double fps = cap.get(CV_CAP_PROP_FPS);

	int runningTime; // ���� �������� ������ ���� �� �� msec���� ������������ ��Ÿ����.

	//////////////////////////////////////////////////////////////////////////
	// 5. Ready to write a videofile
	//////////////////////////////////////////////////////////////////////////

	VideoWriter wrt;     // ���ɱ����� ������ ������ ������ ��ü.

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
				// �������� ����Ǹ� ���� �������� �����. ���α׷��� ������� �ʰ� �Ϸ��� ����־���.
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
			case VP_GAMMA		 : frame = GetGammaChangedImg(frame);
				                   break;
			case VP_UNSHARP		 : frame = GetUnsharpImg(frame);
				                   break;
			case VP_ACHROMATIC   : frame = GetAchromaticImg(frame);
								   break;
			default				 : break;
			}

			if(video_save_flag == true)
				wrt.write(frame);

			runningTime = (int)cap.get(CV_CAP_PROP_POS_MSEC);

			slider_position = runningTime;

			createTrackbar("Time(msec)", "original", &slider_position, videoTotalLength, onTrackbarSlide);
			//setTrackbarPos("Time(msec)", "original", runningTime); // �� �Լ��� �̿��ϸ� ������ �����Ÿ�.

			imshow("original", frame);
		}

		int key;

		key = (int)waitKey(1000.0/(fps*speed));	// �������� fps�� ����� �ӵ��� ������ ����ϱ� ���� �д� �ӵ��� ����.

		if(key == 0x1b)
			break;



		// �ߺ����� �� ��� ó������ �ǵ��ư�
		if( (key - '0') == video_proc )
			continue;
		// �ߺ������� �ƴ� ��� ���� ������ �����Ѵ�.
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
		case '0'  : video_proc = VP_NONE;			// ���� ���� ���
			cout << "No Image Processing" << endl;
			break;		
		case '1'  : video_proc = VP_HIST_EQUAL;		// ������׷� ��Ȱȭ
			cout << "Histogram Equalization" << endl;
			break;		
		case '2'  : video_proc = VP_HIST_STRETCH;	// ������׷� ��Ʈ��Ī
			cout << "Histogram Stretching " << endl;
			break;		
		case '3'  : video_proc = VP_GAMMA;			// ���� ��ȯ
			cout << "Change Gamma " << endl;
			break;		
		case '4'  : video_proc = VP_UNSHARP;		// Unsharp Masking
			cout << "Unsharp Masking" << endl;
			break;		
		case '5'  : video_proc = VP_ACHROMATIC;		// Ư�� hue�� ���ϰ� �������� ��ä��
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