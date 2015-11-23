//////////////////////////////////////////////////////////////////////////
// �ۼ����� : 2015/10/29
// �й� : 2011305051
// �̸� : ������
// ���� : ���� open,restart , ���� ���� ��ȯ(msec ����)
//		  �̺�Ʈó�� �Լ�          
//////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "func.h"

// main.cpp ���� ������ ��ü, ���ڿ���.

extern char InputFilePath[100];

extern VideoCapture * global_cap;

int OpenVideoFile(VideoCapture &cap, const char videoFilePath[])
{
	cap.open(videoFilePath);

	if(!cap.isOpened()) {
		printf("Failed to open a video file\n");
		return -1;
	}

	return 0;
}

int RestartVideo(VideoCapture &cap, const char videoFilePath[])
{
	cap.release();
	
	return OpenVideoFile(cap, videoFilePath);	
}

void SetVideoPosTo(VideoCapture &cap, const int msec)
{
	RestartVideo(cap, InputFilePath);

	do 
	{
		cap.grab();
	} while (cap.get(CV_CAP_PROP_POS_MSEC) < msec);
}

int GetVideoLengthInMSEC( const char videoFilePath[] )
{
	VideoCapture cap;

	int video_length;
	double fps;
	double total_frames;
	
	OpenVideoFile(cap, videoFilePath);
	
	fps = cap.get(CV_CAP_PROP_FPS);
	total_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);

	//////////////////////////////////////////////////////////////////////////
	// ������ ���̸� �� ������ ���ϰ�,
	// 1000�� ���� msec������ ��ȯ�� �� ��ȯ.
	//////////////////////////////////////////////////////////////////////////

	video_length = (int)((total_frames / fps) * 1000);

	// �������� ����ð��� ��ȯ
	return video_length;
}

void onTrackbarSlide(int pos, void * userdata /* = 0 */)
{
	SetVideoPosTo(*global_cap, pos);
}

// Callback function for mouse events
void mouse_callback(int event, int x, int y, int, void * userdata /* = 0 */)
{
	VideoCapture &cap = *global_cap;

	// Original image
	static Mat img;

	// Flags updated according to left mouse button activity
	static bool Ldown = false, Lup = false;
	static bool Rdown = false, Rup = false;

	// Starting and ending points of the user's selection
	static Point corner1, corner2;
	
	// When the left mouse button is pressed, record its position and save it in corner1 
	if(event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN)
	{	
		if(event == EVENT_LBUTTONDOWN) Ldown = true;
		else if(event == EVENT_RBUTTONDOWN) Rdown = true;

		corner1.x = x;
		corner1.y = y;
		cout << "Corner 1 recorded at " << corner1 << endl;
	}

	// When the left mouse button is released, record its position and save it in corner2
	//////////////////////////////////////////////////////////////////////////
	// 
	//////////////////////////////////////////////////////////////////////////
	if(event == EVENT_LBUTTONUP || event == EVENT_RBUTTONUP)
	{
		// Also check if user selection is bigger than 20 pixels (jut for fun!)
		if(abs(x - corner1.x) > 20 && abs(y - corner1.y) > 20)
		{
			if(event == EVENT_LBUTTONUP) Lup = true;
			else if(event == EVENT_RBUTTONUP) Rup = true;

			// �� ��° ��ǥ�� �̹��� �ٱ����� ������ ��� coner2�� �̹��� ������ ���� �ִ� ��ǥ�� ����.
			corner2.x = max(0, min(x, (int)cap.get(CV_CAP_PROP_FRAME_WIDTH )));;
			corner2.y = max(0, min(y, (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT )));
			cout << "Corner 2 recorded at " << corner2 << endl << endl;
		}
		else
		{
			cout << "Please select a bigger region" << endl;
			if(event == EVENT_LBUTTONUP) Lup = false;
			else if(event == EVENT_RBUTTONUP) Rup = false;
		}
	}

	// Update the box showing the selected region as the user drags the mouse
	if( (Ldown == true && Lup == false) || (Rdown == true && Rup == false) )
	{
		Point pt;
		pt.x = x;
		pt.y = y;

		cap.retrieve(img);
		Mat img_clone = img.clone();
		rectangle(img_clone, corner1, pt, Scalar(0, 0, 255));
		imshow("original", img_clone);
		
	}

	// Define ROI and crop it out when both corners have been selected	
	if( (Ldown == true && Lup == true) || (Rdown == true && Rup == true) )
	{	
		// ROI
		Rect box;

		box.width = abs(corner1.x - corner2.x);
		box.height = abs(corner1.y - corner2.y);
		box.x = min(corner1.x, corner2.x);
		box.y = min(corner1.y, corner2.y);

		// Make an image out of just the selected ROI and display it in a new window
		Mat crop(img, box);
				
		cout << "crop Size = " << crop.size() << endl;	

		// ��Ŭ������ �簢���� �׸��� Blurring�� �� �̹����� ���
		if(Rdown == true && Rup == true)
		{
			int ksize = 7;
			int sigma = 3.0;

			GaussianBlur(crop, crop, Size(ksize, ksize), sigma);
		}
		
		namedWindow("Crop", WINDOW_NORMAL);
		//namedWindow("Crop", WINDOW_AUTOSIZE);  // WINDOW_AUTOSIZE�� ����ϸ� resizeWindow�� ����� �� ����.
		resizeWindow("Crop", box.width, box.height);		
		imwrite("output.jpg", crop);
		imshow("Crop", crop);
		
		if(Ldown == true && Lup == true) Lup = Ldown = false;
		else if(Rdown == true && Rup == true) Rup = Rdown = false;
	}

}