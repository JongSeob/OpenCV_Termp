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

	Mat frame; // 영상에서 읽어낸 프레임을 저장할 객체

	CvSize FrameSize;

	FrameSize.width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH );
	FrameSize.height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT );

	double fps = cap.get(CV_CAP_PROP_FPS);

	// 5. ready to write a videofile

	VideoWriter wrt;     // 관심구간을 제외한 비디오를 저장할 객체.

#define	CODEC CV_FOURCC('D', 'I', 'V', 'X')
		
	int runningTime; // 현재 프레임이 동영상 실행 후 몇 msec후의 프레임인지를 나타낸다.

	Menu();

	while(1)
	{	
		if (video_play == true)
		{
			cap.read(frame);

			if(frame.empty()) 
			{
				/////////////////////////////////////////////////////////////////////////////////////
				// 동영상이 종료되면 같은 동영상을 재실행. 프로그램이 종료되지 않게 하려고 집어넣음.
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
			//setTrackbarPos("Time(msec)", "original", runningTime); // 영상이 진행하면서 트랙바도 같이 움직임. 
			// 실행하면 영상 속도가 너무 느려져서 제외함.			

			imshow("original", frame);
		}
				
		int key;

		key = waitKey(1000.0/(fps*speed));	// 동영상의 fps와 비슷한 속도로 영상을 출력하기 위해 읽는 속도를 조절.

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





