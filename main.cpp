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
#include "img_proc.h"

#define OUTPUT_PATH "E:/dip_termp/"

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

int stretchMin = 30;
int stretchMax = 180;
double gamma = 1.0;
int sigma = 12;
int margin;

extern Mat histSretchLut;

//Mat lut(1, 256, CV_8UC1); // 1 x 256 크기의 Mat 클래스. Imadjust에서 사용하기 위한 LUT 정보를 미리 저장하고 있는다.

void Menu();

int main(void) 
{
	VideoCapture cap;

	global_cap = &cap;

	//////////////////////////////////////////////////////////////////////////
	// 1. 플래그 변수 생성
	//////////////////////////////////////////////////////////////////////////

	int video_proc = VP_NONE;
	bool video_save_flag = false;
	bool video_play = true;

	int speed = 1;

	//////////////////////////////////////////////////////////////////////////
	// 2. 저장된 영상 경로 설정
	//////////////////////////////////////////////////////////////////////////

	for(int i=0; i < 6; i++)
		strcat_s(OutputFilePath[i], OutputFileName[i]);	


	//////////////////////////////////////////////////////////////////////////
	// 3. 윈도우, 마우스 핸들러 생성
	//////////////////////////////////////////////////////////////////////////

	namedWindow("camera", 1);

	setMouseCallback("camera", mouse_callback);

	//////////////////////////////////////////////////////////////////////////
	// 4. 카메라 실행, 카메라 영상 정보를 변수에 저장
	//////////////////////////////////////////////////////////////////////////

	OpenCamera(cap, 0);

	//cap.open("D:/dip/images/the_return_of_the_king.avi");

	Mat frame; // 영상에서 읽어낸 프레임을 저장할 객체

	CvSize FrameSize;

	FrameSize.width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH );
	FrameSize.height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT );

	double fps = cap.get(CV_CAP_PROP_FPS);

	//////////////////////////////////////////////////////////////////////////
	// 5. 영상 저장을 위한 변수 선언.
	//////////////////////////////////////////////////////////////////////////

	VideoWriter wrt;     // 관심구간을 제외한 비디오를 저장할 객체.

#define	CODEC CV_FOURCC('D', 'I', 'V', 'X')


	//////////////////////////////////////////////////////////////////////////
	// 6. 트랙바 생성을 위한 구조체
	//////////////////////////////////////////////////////////////////////////

	// Histogram Stretching Min, Max

	TrackbarInfo trackbar_HistStretch_Min;

	trackbar_HistStretch_Min.windowName = "camera";
	trackbar_HistStretch_Min.trackbarName = "Min";
	trackbar_HistStretch_Min.defaultValue = stretchMin;
	trackbar_HistStretch_Min.maxValue = 255;

	TrackbarInfo trackbar_HistStretch_Max;

	trackbar_HistStretch_Max.windowName = "camera";
	trackbar_HistStretch_Max.trackbarName = "Max";
	trackbar_HistStretch_Max.defaultValue = stretchMax;
	trackbar_HistStretch_Max.maxValue = 255;
	
	// Gamma Change Value 

	TrackbarInfo trackbar_Gamma;

	trackbar_Gamma.windowName = "camera";
	trackbar_Gamma.trackbarName = "0.1 units ";
	trackbar_Gamma.defaultValue = gamma*10;
	trackbar_Gamma.maxValue = 30;
	
	// Unsharp Masking Sigma

	TrackbarInfo trackbar_Unsharp;

	trackbar_Unsharp.windowName = "camera";
	trackbar_Unsharp.trackbarName = "Sigma";
	trackbar_Unsharp.defaultValue = sigma;
	trackbar_Unsharp.maxValue = 30;	

	// Initialize Lut
	UpdateLut(histSretchLut, stretchMin, stretchMax);

	Menu();

	while(1)
	{	
		if (video_play == true)
		{
			cap.read(frame);

			if(frame.empty()) 
			{
				cout << "Camera ShutDown" << endl;
				exit(1);
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

			imshow("camera", frame);
		}

		//////////////////////////////////////////////////////////////////////////
		// Processing Key Input
		//////////////////////////////////////////////////////////////////////////

		int key;

		//key = (int)waitKey(1000.0/(fps*speed));	// 동영상의 fps와 비슷한 속도로 영상을 출력하기 위해 읽는 속도를 조절.
		key = (int)waitKey(1);		

		if(key == 0x1b)
			break;

		// 중복선택 된 경우
		if( (key - '0') == video_proc )
			continue;
		// 중복선택이 아닌 경우
		else if( (key - '0') >= VP_NONE && (key - '0') <= VP_ACHROMATIC )
		{
			// 새로운 이미지 저장을 위해 기존의 저장을 종료
			if(video_save_flag == true)
			{
				wrt.release();
				cout << "VideoWrite finished." << endl;
				video_save_flag = false;
			}

			// 트랙바를 새로 생성하기 위해 기존의 윈도우를 종료 후 재생성
			destroyWindow("camera");
			namedWindow("camera");
			setMouseCallback("camera", mouse_callback);
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
			CreateTrackbar(trackbar_HistStretch_Min, onTrackbarStretchMin);
			CreateTrackbar(trackbar_HistStretch_Max, onTrackbarStretchMax);
			break;		
		case '3'  : video_proc = VP_GAMMA;			// 감마 변환
			cout << "Change Gamma " << endl;
			CreateTrackbar(trackbar_Gamma, onTrackbarGamma);
			break;		
		case '4'  : video_proc = VP_UNSHARP;		// Unsharp Masking
			cout << "Unsharp Masking" << endl;
			CreateTrackbar(trackbar_Unsharp, onTrackbarUnsharpSigma);
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