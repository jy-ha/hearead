
#include "define.h"


int main()
{
	char* outText = NULL;
	char confirmedText[MAX_READ_CHAR];
	int Param1 = 22;

	TessBaseAPI TessApi;
	if (TessApi.Init(NULL, TESS_LANG)){
		fprintf(stderr, "Fail Init Tesseract\n");	exit(1);
	}

	VideoCapture OpencvCam(0);
	if (!OpencvCam.isOpened()){
		fprintf(stderr, "Fail Init Cam\n");	exit(1);
	}

	Mat RawImg(WINDOW_ROW, WINDOW_COL, CV_8UC3);
	Mat opencvImg(WINDOW_ROW, WINDOW_COL, CV_8UC1);
	Mat LabledImg, LableStats, LableCentroid;
	//Mat LabledImg2;

	int		nLables = 0;
	int		nAngle = 0;
	bool	bNewLine = true;
	int		nLineNow = 0;

	namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE);
	resizeWindow(WINDOW_NAME, WINDOW_COL, WINDOW_ROW);

	createTrackbar("Val", WINDOW_NAME, &Param1, 255);


	while (1)
	{
		OpencvCam >> RawImg;	// Get image from USB camera


		/*  Convert to Black & White  */
		cvtColor(RawImg, opencvImg, COLOR_RGB2GRAY);
		adaptiveThreshold(opencvImg, opencvImg, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, ATHRESHOLD_BLOCK, Param1);
		//threshold(opencvImg, opencvImg, Param1, 255, CV_THRESH_BINARY);



		/*  Lableling  */
		connectedComponentsWithStats(opencvImg, LabledImg, LableStats, LableCentroid, LABLING_NEAR, LABLE_LTYPE);
		Proc_BlobConstraint(&opencvImg, &LabledImg, &LableStats);		// Clearing
		//Proc_DeleteCuttedWord(&LabledImg, LABLING_NEAR);	// Delete words at the border - not used yet
		nLables = connectedComponentsWithStats(opencvImg, LabledImg, LableStats, LableCentroid, LABLING_NEAR, LABLE_LTYPE);
			// Lableling again --> Maybe I can concatenate with algorism above for efficiency



		/*  Calibrate image tilt - High calculation cost  */
		nAngle = Proc_FindTextLine_Ang(&LabledImg, &LableStats, nLables);
		if (nAngle == PROC_NOLINE){
			if (waitKey(MSPF) == END_KEY){ break; }
			continue;
		}
		


		/*  Pre-process line for detect  */
		Proc_FindTextLine_Info(&LabledImg, LableStats, nLables);
		if (!Proc_LineCount){
			if (waitKey(MSPF) == END_KEY){ break; }
			continue;
		}



		/*  Detect current line  */
		if (bNewLine)	nLineNow = Proc_FindNewLine();
		else			nLineNow = Proc_FindContinueLine();
		
		


		/*  image shift - not used  */
		//if (bNewLine){
		//	for (int i = 0; i < Proc_LineInfos[nLineNow].nWord; i++)
		//		WordAreaOld[i] = Proc_LineInfos[nLineNow].WordArea[i];
		//}
		//else			Proc_GetShiftedValue(nLineNow);



		/*  determine read or not using image shift - not used  */



		/*  Lable to image - for Debug  */
		LABLE_LTYPET* dataLables = (LABLE_LTYPET*)(LabledImg.data);
		Mat FinalArea(Proc_LineInfos[nLineNow].Bottom - Proc_LineInfos[nLineNow].Top, Proc_LineInfos[nLineNow].Right - Proc_LineInfos[nLineNow].Left, CV_8UC1);
		Mat FinalAreaAffined(Proc_LineInfos[nLineNow].Bottom - Proc_LineInfos[nLineNow].Top, Proc_LineInfos[nLineNow].Right - Proc_LineInfos[nLineNow].Left, CV_8UC1);
		uchar* FinalImg = (uchar*)(FinalArea.data);
		for (int row = 0; row < FinalArea.rows; row++){
			for (int col = 0; col < FinalArea.cols; col++){
				if (dataLables GETPIX(row + Proc_LineInfos[nLineNow].Top, col + Proc_LineInfos[nLineNow].Left, opencvImg.cols))
						FinalImg GETPIX(row, col, FinalArea.cols) = 255;
				else	FinalImg GETPIX(row, col, FinalArea.cols) = 0;
			}
		}
		if (nAngle > 3){
			cv::Point2f		src_center((float)FinalArea.cols, (float)FinalArea.rows);
			cv::Mat			rot_mat = getRotationMatrix2D(src_center, -nAngle, 1);
			warpAffine(FinalArea, FinalAreaAffined, rot_mat, FinalArea.size());
			FinalArea = FinalAreaAffined;
		}



		/*  image generate - for Debug  */
		uchar* dataImg = (uchar*)(opencvImg.data);
		for (int row = 0; row < LabledImg.rows; row++){
			for (int col = 0; col < LabledImg.cols; col++){
				if (dataLables GETPIX(row, col, LabledImg.cols))		dataImg GETPIX(row, col, opencvImg.cols) = 255;
				else													dataImg GETPIX(row, col, opencvImg.cols) = 0;
			}
		}
		for (int i = 0; i < Proc_LineCount; i++){
			rectangle(opencvImg, Point(Proc_LineInfos[i].Left, Proc_LineInfos[i].Top), Point(Proc_LineInfos[i].Right, Proc_LineInfos[i].Bottom), Scalar(250, 0, 0));
		}
		imshow(WINDOW_NAME, opencvImg);
		


		/*  Using Tesseract  */
		TessApi.SetImage((uchar*)FinalArea.data, FinalArea.cols, FinalArea.rows, 1, FinalArea.cols);

		if (bNewLine){		// if new line start
			outText = TessApi.GetUTF8Text();
			if (outText[0]){	// text readed
				strncpy(confirmedText, outText, MAX_READ_CHAR - 1);
				confirmedText[MAX_READ_CHAR - 1] = 0;
				bNewLine = false;
			}
		}
		else{				// same line as previous frame
			outText = TessApi.GetUTF8Text();
			Proc_MakeFinalText(outText, confirmedText);
		}
		printf("outText : %s\n", outText);
		printf("confirmedText : %s\n", confirmedText);


		// delete [] outText;
		
		if (waitKey(MSPF) == END_KEY)	break;
	}


	TessApi.End();
	OpencvCam.release();
	destroyWindow(WINDOW_NAME);
	return 0;
}
