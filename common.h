//////////////////////////////////////////////////////////////////////////
// 작성일자 : 2015/10/29
// 학번 : 2011305051
// 이름 : 윤종섭
// 설명 : 모든 소스에서 공통적으로 사용하는 헤더파일들의 선언
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
