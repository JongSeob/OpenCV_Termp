//////////////////////////////////////////////////////////////////////////
// �ۼ����� : 2015/10/29
// �й� : 2011305051
// �̸� : ������
// ���� : ��� �ҽ����� ���������� ����ϴ� ������ϵ��� ����
//////////////////////////////////////////////////////////////////////////

#ifndef _common_h__
#define _common_h__

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv.hpp"

using namespace cv;
using namespace std;

// Video Processing
enum {VP_NONE, VP_HIST_EQUAL, VP_HIST_STRETCH, VP_GAMMA, VP_UNSHARP, VP_ACHROMATIC};

#endif // common_h__
