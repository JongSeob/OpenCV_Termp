//////////////////////////////////////////////////////////////////////////
// �ۼ����� : 2015/10/29
// �й� : 2011305051
// �̸� : ������
// ���� : func.cpp ���� ������ �Լ����� �����
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
