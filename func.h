//////////////////////////////////////////////////////////////////////////
// 작성일자 : 2015/10/29
// 학번 : 2011305051
// 이름 : 윤종섭
// 설명 : func.cpp 에서 정의한 함수들의 선언부
//////////////////////////////////////////////////////////////////////////

#ifndef _func_h__
#define _func_h__

int OpenCamera(VideoCapture &cap, const int camera_num);

void onTrackbarSlide(int pos, void * userdata = 0); /* pos means trackbar position. It means threshold value here. */;
void mouse_callback(int event, int x, int y, int, void * userdata = 0);


#endif // func_h__
