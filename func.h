//////////////////////////////////////////////////////////////////////////
// 작성일자 : 2015/10/29
// 학번 : 2011305051
// 이름 : 윤종섭
// 설명 : func.cpp 에서 정의한 함수들의 선언부
//////////////////////////////////////////////////////////////////////////

#ifndef _func_h__
#define _func_h__

#include "common.h"

int OpenCamera(VideoCapture &cap, const int camera_num);

void mouse_callback(int event, int x, int y, int, void * userdata = 0);

int CreateTrackbar(TrackbarInfo &trackbar, TrackbarCallback onChange = 0);

void onTrackbarStretchMin(int pos, void * userdata = 0);
void onTrackbarStretchMax(int pos, void * userdata = 0);
void onTrackbarGamma(int pos, void * userdata = 0);
void onTrackbarUnsharpSigma(int pos, void * userdata = 0);
void onTrackbarHueMargin(int pos, void * userdata = 0);


#endif // func_h__
