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
	createTrackbar("Time(msec)", "original", &slider_position, videoTotalLength, onTrackbarSlide, &cap);

	// 3. Set the mouse event callback function

	setMouseCallback("original", mouse_callback, (void *)&cap);

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

	// 6. Variable for gaussian blurring

	int ksize = 7;
	int sigma = 3.0;	

	int runningTime; // 현재 프레임이 동영상 실행 후 몇 msec후의 프레임인지를 나타낸다.

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

				cout << endl << "No image found!" << endl;
				cout << "and restart current video" << endl;

				RestartVideo(cap, InputFilePath);

				cap.read(frame);
				
				continue;			
			}			

			runningTime = cap.get(CV_CAP_PROP_POS_MSEC);

			slider_position = runningTime;

			createTrackbar("Time(msec)", "original", &slider_position, videoTotalLength, onTrackbarSlide);
			//setTrackbarPos("Time(msec)", "original", runningTime); // 영상이 진행하면서 트랙바도 같이 움직임. 
			// 실행하면 영상 속도가 너무 느려져서 제외함.

			imshow("original", frame);
		}

		if(video_save_flag == true)
			wrt.write(frame);
		
		int key;

		key = waitKey(1000.0/(fps*speed));	// 동영상의 fps와 비슷한 속도로 영상을 출력하기 위해 읽는 속도를 조절.

		if(key == 0x1b)
			break;

		switch(key)
		{
		case '0'  : video_proc = VP_NONE;			break;		// 원 영상만 출력
		case '1'  : video_proc = VP_HIST_EQUAL;		break;		// 히스토그램 평활화
		case '2'  : video_proc = VP_HIST_STRETCH;	break;		// 히스토그램 스트레칭
		case '3'  : video_proc = VP_GAMMA;			break;		// 감마 변환
		case '4'  : video_proc = VP_UNSHARP;		break;		// Unsharp Masking
		case '5'  : video_proc = VP_ACHROMATIC;		break;		// 특정 hue만 취하고 나머지는 무채색
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





