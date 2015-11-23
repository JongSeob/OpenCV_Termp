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

enum {VP_NONE, VP_HIST_EQUAL, VP_HIST_STRETCH, VP_GAMMA, VP_UNSHARP, VP_ACHROMATIC};

void Menu();

int main(void) 
{
	VideoCapture cap;

	global_cap = &cap;

	//////////////////////////////////////////////////////////////////////////
	// Flags
	//////////////////////////////////////////////////////////////////////////

	int video_proc = VP_NONE;
	bool video_save_flag = false;
	bool video_play = true;

	int speed = 1;


	// 1. define InputFilePath, OutputFilePath

	strcat_s(InputFilePath,  InputFileName);

	for(int i=0; i < 6; i++)
		strcat_s(OutputFilePath[i], OutputFileName[i]);	


	// 2. Create Window and Trackbar

	int	videoTotalLength = GetVideoLengthInMSEC(InputFilePath);	// Maximum slider value
	int	slider_position = 0;							// Constantly updated slider value

	namedWindow("original", 1);
	createTrackbar("Time(msec)", "original", &slider_position, videoTotalLength, onTrackbarSlide);

	// 3. Set the mouse event callback function

	setMouseCallback("original", mouse_callback);

	// 4. Open the VideoFile

	OpenVideoFile(cap, InputFilePath);

	Mat frame; // ���󿡼� �о �������� ������ ��ü

	CvSize FrameSize;

	FrameSize.width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH );
	FrameSize.height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT );

	double fps = cap.get(CV_CAP_PROP_FPS);

	// 5. ready to write a videofile

	VideoWriter wrt;     // ���ɱ����� ������ ������ ������ ��ü.

#define	CODEC CV_FOURCC('D', 'I', 'V', 'X')
		
	int runningTime; // ���� �������� ������ ���� �� �� msec���� ������������ ��Ÿ����.

	Menu();

	while(1)
	{	
		if (video_play == true)
		{
			cap.read(frame);

			if(frame.empty()) 
			{
				/////////////////////////////////////////////////////////////////////////////////////
				// �������� ����Ǹ� ���� �������� �����. ���α׷��� ������� �ʰ� �Ϸ��� �������.
				/////////////////////////////////////////////////////////////////////////////////////
				
				if(video_save_flag == true)
				{
					wrt.release();
					video_save_flag = false;
				}

				RestartVideo(cap, InputFilePath);

				cap.read(frame);

				cout << endl;

				system("cls");

				Menu();
				
				continue;			
			}

			switch (video_proc)
			{
			case VP_HIST_EQUAL   : break;
			case VP_HIST_STRETCH : break;
			case VP_GAMMA		 : break;
			case VP_UNSHARP		 : break;
			case VP_ACHROMATIC   : break;
			default				 : break;
			}
			
			if(video_save_flag == true)
				wrt.write(frame);

			runningTime = cap.get(CV_CAP_PROP_POS_MSEC);

			slider_position = runningTime;

			createTrackbar("Time(msec)", "original", &slider_position, videoTotalLength, onTrackbarSlide);
			//setTrackbarPos("Time(msec)", "original", runningTime); // ������ �����ϸ鼭 Ʈ���ٵ� ���� ������. 
			// �����ϸ� ���� �ӵ��� �ʹ� �������� ������.			

			imshow("original", frame);
		}
				
		int key;

		key = waitKey(1000.0/(fps*speed));	// �������� fps�� ����� �ӵ��� ������ ����ϱ� ���� �д� �ӵ��� ����.

		if(key == 0x1b)
			break;

		if( (key - '0') == video_proc )
			continue;
		else if( ((key - '0') >= VP_NONE) && ((key - '0') <= VP_ACHROMATIC)   )
		{
			wrt.release();
			cout << "VideoWrite finished." << endl;
			video_save_flag = false;
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





