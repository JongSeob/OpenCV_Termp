//////////////////////////////////////////////////////////////////////////
// �ۼ����� : 2015/10/29
// �й� : 2011305051
// �̸� : ������
// ���� : func.cpp ���� ������ �Լ����� �����
//////////////////////////////////////////////////////////////////////////

#ifndef _func_h__
#define _func_h__

int OpenCamera(VideoCapture &cap, const int camera_num);

void onTrackbarSlide(int pos, void * userdata = 0); /* pos means trackbar position. It means threshold value here. */;
void mouse_callback(int event, int x, int y, int, void * userdata = 0);


#endif // func_h__
