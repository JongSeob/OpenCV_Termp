//////////////////////////////////////////////////////////////////////////
// �ۼ����� : 2015/10/29
// �й� : 2011305051
// �̸� : ������
// ���� : func.cpp ���� ������ �Լ����� �����
//////////////////////////////////////////////////////////////////////////

#ifndef _func_h__
#define _func_h__

int OpenVideoFile(VideoCapture &cap, const char videoFilePath[]); // ���� ���� �������� �˻�
int RestartVideo(VideoCapture &cap, const char videoFilePath[]);  // ���� ���� �� �ٽ� ����
void SetVideoPosTo(VideoCapture &cap, const int msec); // ���� ����� �������� ��ġ�� ����
int GetVideoLengthInMSEC(const char videoFilePath[]); // ������ ���̸� msec������ ��ȯ

void onTrackbarSlide(int pos, void * userdata = 0); /* pos means trackbar position. It means threshold value here. */;
void mouse_callback(int event, int x, int y, int, void * userdata = 0);


#endif // func_h__
