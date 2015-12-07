//////////////////////////////////////////////////////////////////////////
// 작성일자 : 2015/10/29
// 학번 : 2011305051
// 이름 : 윤종섭
// 설명 : 영상 open,restart , 영상 길이 반환(msec 단위)
//		  이벤트처리 함수          
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "func.h"
#include "img_proc.h"

// main.cpp 에서 정의한 객체, 문자열들.

extern VideoCapture * global_cap;

extern int stretchMin;
extern int stretchMax;
extern double gamma;
extern int sigma;
extern int margin;

extern Mat histSretchLut;

int OpenCamera( VideoCapture &cap, const int camera_num )
{
	cap.open(camera_num);	

	if(!cap.isOpened()) {
		printf("Failed to open camera\n");
		return -1;
	}

	return 0;
}

// Callback function for mouse events
void mouse_callback(int event, int x, int y, int, void * userdata /* = 0 */)
{
	VideoCapture &cap = *global_cap;

	Mat img;

	Point hue_loc(x,y);

	if(event == EVENT_LBUTTONUP || event == EVENT_RBUTTONUP)
	{
		cap.retrieve(img);

		cout << "Point = " << hue_loc << endl;
		//cout << "img at = " << img.at<int>(x, y) << endl;
		
	}
}

void onTrackbarStretchMin( int pos, void * userdata /*= 0*/ )
{
	UpdateLut(histSretchLut, pos, stretchMax);

	stretchMin = pos;
}

void onTrackbarStretchMax( int pos, void * userdata /*= 0*/ )
{
	UpdateLut(histSretchLut, stretchMin, pos);

	stretchMax = pos;
}

void onTrackbarGamma( int pos, void * userdata /*= 0*/ )
{
	gamma = pos * 0.1;
	
	cout << "gamma = " << gamma << endl;
}

void onTrackbarUnsharpSigma( int pos, void * userdata /*= 0*/ )
{
	VideoCapture &cap = *global_cap;
}

void onTrackbarHueMargin( int pos, void * userdata /*= 0*/ )
{
	VideoCapture &cap = *global_cap;
}

int CreateTrackbar( TrackbarInfo &trackbar, TrackbarCallback onChange /*= 0*/ )
{
	return createTrackbar(trackbar.trackbarName, trackbar.windowName, &trackbar.defaultValue, trackbar.maxValue, onChange);	
}
