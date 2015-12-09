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

//Mat lut(1, 256, CV_8UC1); // 1 x 256 ũ���� Mat Ŭ����. Imadjust���� ����ϱ� ���� LUT ������ �̸� �����ϰ� �ִ´�.

void Menu();

int main(void) 
{
	VideoCapture cap;

	global_cap = &cap;

	//////////////////////////////////////////////////////////////////////////
	// 1. �÷��� ���� ����
	//////////////////////////////////////////////////////////////////////////

	int video_proc = VP_NONE;
	bool video_save_flag = false;
	bool video_play = true;

	int speed = 1;

	//////////////////////////////////////////////////////////////////////////
	// 2. ����� ���� ��� ����
	//////////////////////////////////////////////////////////////////////////

	for(int i=0; i < 6; i++)
		strcat_s(OutputFilePath[i], OutputFileName[i]);	


	//////////////////////////////////////////////////////////////////////////
	// 3. ������, ���콺 �ڵ鷯 ����
	//////////////////////////////////////////////////////////////////////////

	namedWindow("camera", 1);

	setMouseCallback("camera", mouse_callback);

	//////////////////////////////////////////////////////////////////////////
	// 4. ī�޶� ����, ī�޶� ���� ������ ������ ����
	//////////////////////////////////////////////////////////////////////////

	OpenCamera(cap, 0);

	//cap.open("D:/dip/images/the_return_of_the_king.avi");

	Mat frame; // ���󿡼� �о �������� ������ ��ü

	CvSize FrameSize;

	FrameSize.width = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH );
	FrameSize.height = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT );

	double fps = cap.get(CV_CAP_PROP_FPS);

	//////////////////////////////////////////////////////////////////////////
	// 5. ���� ������ ���� ���� ����.
	//////////////////////////////////////////////////////////////////////////

	VideoWriter wrt;     // ���ɱ����� ������ ������ ������ ��ü.

#define	CODEC CV_FOURCC('D', 'I', 'V', 'X')


	//////////////////////////////////////////////////////////////////////////
	// 6. Ʈ���� ������ ���� ����ü
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

		//key = (int)waitKey(1000.0/(fps*speed));	// �������� fps�� ����� �ӵ��� ������ ����ϱ� ���� �д� �ӵ��� ����.
		key = (int)waitKey(1);		

		if(key == 0x1b)
			break;

		// �ߺ����� �� ���
		if( (key - '0') == video_proc )
			continue;
		// �ߺ������� �ƴ� ���
		else if( (key - '0') >= VP_NONE && (key - '0') <= VP_ACHROMATIC )
		{
			// ���ο� �̹��� ������ ���� ������ ������ ����
			if(video_save_flag == true)
			{
				wrt.release();
				cout << "VideoWrite finished." << endl;
				video_save_flag = false;
			}

			// Ʈ���ٸ� ���� �����ϱ� ���� ������ �����츦 ���� �� �����
			destroyWindow("camera");
			namedWindow("camera");
			setMouseCallback("camera", mouse_callback);
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
			CreateTrackbar(trackbar_HistStretch_Min, onTrackbarStretchMin);
			CreateTrackbar(trackbar_HistStretch_Max, onTrackbarStretchMax);
			break;		
		case '3'  : video_proc = VP_GAMMA;			// ���� ��ȯ
			cout << "Change Gamma " << endl;
			CreateTrackbar(trackbar_Gamma, onTrackbarGamma);
			break;		
		case '4'  : video_proc = VP_UNSHARP;		// Unsharp Masking
			cout << "Unsharp Masking" << endl;
			CreateTrackbar(trackbar_Unsharp, onTrackbarUnsharpSigma);
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