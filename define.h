#pragma once

// 명령줄 인수로 -psm 7 준 상태 -> tesseract single line 인식 -> 미적용상태



#pragma comment (lib, "opencv_world310d.lib")
#pragma warning (disable : 4819)


#include <opencv2\core\core.hpp>
//#include <opencv2/core/mat.hpp>
//#include <opencv2/imgcodecs.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\utility.hpp>

#include <allheaders.h>
#include <baseapi.h>

#include <cmath>
#include <iostream>
//#include <vector>		임베디드 환경을 가정해 STL은 피하는게 좋음


#pragma comment (lib, "liblept171.lib")
#pragma comment (lib, "libtesseract304.lib")


//using namespace std;
using namespace cv;
using namespace tesseract;


#define WINDOW_NAME		"Hearead"
#define WINDOW_COL		640
#define WINDOW_ROW		480
#define MSPF			100		// millisec per frame
#define END_KEY			27

#define TESS_LANG					"eng"
#define TESS_LANG_COMBINATION		0		// 조합문자 여부


#if (WINDOW_ROW * WINDOW_COL <= 307200)
	#define LABLE_LTYPE		CV_16U
	#define LABLE_LTYPET	ushort
#else
	#define LABLE_LTYPE		CV_32S
	#define LABLE_LTYPET	int
#endif

#define ATHRESHOLD_BLOCK	29		// odd number
#define LABLING_NEAR		8		// 4 or 8
#define PROCESSING_AREA		20
#define PROCESSING_WIDTH	WINDOW_COL * 4 / 5
#define PROCESSING_HEIGHT	WINDOW_ROW * 4 / 5
#define MAX_READ_CHAR		256		// Entire line
#define MAX_READ_WORD		128		// per line
#define	MAX_READ_LINE		10


#define GETPIX(row, col, cols)	[((row) * (cols)) + (col)]
#define PI					3.141592


#include "Processing.h"

