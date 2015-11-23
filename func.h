//////////////////////////////////////////////////////////////////////////
// 작성일자 : 2015/10/29
// 학번 : 2011305051
// 이름 : 윤종섭
// 설명 : func.cpp 에서 정의한 함수들의 선언부
//////////////////////////////////////////////////////////////////////////

#ifndef _func_h__
#define _func_h__

int OpenVideoFile(VideoCapture &cap, const char videoFilePath[]); // 비디오 열고 성공여부 검사
int RestartVideo(VideoCapture &cap, const char videoFilePath[]);  // 영상 닫은 후 다시 열기
void SetVideoPosTo(VideoCapture &cap, const int msec); // 다음 출력할 프레임의 위치를 변경
int GetVideoLengthInMSEC(const char videoFilePath[]); // 비디오의 길이를 msec단위로 반환

void onTrackbarSlide(int pos, void * userdata = 0); /* pos means trackbar position. It means threshold value here. */;
void mouse_callback(int event, int x, int y, int, void * userdata = 0);


#endif // func_h__
