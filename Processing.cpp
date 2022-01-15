
#include "define.h"


int			Proc_LineCount;
LineInfo	Proc_LineInfos[MAX_READ_LINE];
int			OldLineTop;
int			WordAreaOld[MAX_READ_WORD];



void	Proc_BlobConstraint(cv::Mat* dst, cv::Mat* Lables, cv::Mat* Stats)
{
	LABLE_LTYPET* dataLables = (LABLE_LTYPET*)(Lables->data);
	uchar* dataDst = (uchar*)(dst->data);
	int* dataStats = (int*)(Stats->data);
	int  TempInt;

	for (int row = 0; row < dst->rows; row++){
		for (int col = 0; col < dst->cols; col++)
		{
			TempInt = row * dst->cols + col;

			if (dataLables[TempInt]){
				if (dataStats GETPIX(dataLables[TempInt], CC_STAT_AREA, Stats->cols) < PROCESSING_AREA){
					dataDst GETPIX(row, col, dst->cols) = 0;
					continue;
				}

				if (dataStats GETPIX(dataLables[TempInt], CC_STAT_WIDTH, Stats->cols) > PROCESSING_WIDTH){
					dataDst GETPIX(row, col, dst->cols) = 0;
					continue;
				}

				if (dataStats GETPIX(dataLables[TempInt], CC_STAT_HEIGHT, Stats->cols) > PROCESSING_HEIGHT){
					dataDst GETPIX(row, col, dst->cols) = 0;
					continue;
				}

				dataDst GETPIX(row, col, dst->cols) = 255;
			}
		}
	}
}


#if(TESS_LANG_COMBINATION)
int	Proc_FindTextLine_Ang(cv::Mat* Lables, cv::Mat Stats, int nLable)
{
	LABLE_LTYPET* dataLables = (LABLE_LTYPET*)(Lables->data);
	int*	dataStats = (int*)(Stats->data);
	int		AverageArea = 0;
	//int		MaxWidth = 0;
	////int		MaxHeight = 0;

	// Find average & maximum width, lenght, height
	for (int i = 0; i < nLable; i++){
		AverageArea += dataStats GETPIX(i, CC_STAT_AREA, Stats->cols);

		//if (MaxWidth < dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols)){
		//	MaxWidth = dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols);
		//}
		//if (MaxHeight < dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols)){
		//	MaxHeight = dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols);
		//}
	}
	AverageArea /= nLable;

	// 
	int TempInt1;
	int TempInt2;
	AverageArea /= 5;
	for (int i = 0; i < nLable; i++){	// Lable bigger than 1/5 of average width
		if (dataStats GETPIX(i, CC_STAT_AREA, Stats->cols) > AverageArea)
		{
			if (dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols) > dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols) * 2){
				// Lable that width is over 2 times bigger than height (ex 'ㅡ' 'ㅗ' '받침ㄷ' '받침ㄴ' 'ㅎ의 -')
				TempInt1 = dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols) / 2;

				for (int row = dataStats GETPIX(i, CC_STAT_TOP, Stats->cols) - TempInt1; row < dataStats GETPIX(i, CC_STAT_TOP, Stats->cols) + TempInt1; row++){
					for (int col = dataStats GETPIX(i, CC_STAT_LEFT, Stats->cols);
						col < dataStats GETPIX(i, CC_STAT_LEFT, Stats->cols) + dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols); col++){
						// Searching

					}
				}

			}
			else{
				if (dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols) > dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols) * 2){
					// Lable that height is over 2 times bigger than width (ex 'ㅣ' 'ㅓ' 'ㅏ' 'ㅐ' 'l' '1' 't' 'i')
					TempInt1 = dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols) / dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols) / 2;


				}
				else{	// Lable that width and height is similer

				}
			}
		}
		else{	// Lable smaller than 1/5 of average width  (ex . ; ')

		}
	}


	for (int row = 0; row < Lables->rows; row++){
		for (int col = 0; col < Lables->cols; col++)
		{
			if (dataLables GETPIX(row, col, Lables->cols)){

			}
		}
	}
}

void	Proc_FindTextLine_Info(cv::Mat* Lables, cv::Mat* Stats, int nLable)
{

}
#else
int	Proc_FindTextLine_Ang(cv::Mat* Lables, cv::Mat *Stats, int nLable)
{
	LABLE_LTYPET* dataLables = (LABLE_LTYPET*)(Lables->data);
	int*	dataStats = (int*)(Stats->data);
	int		AverageHeight = 0;

	// Get Average height
	for (int i = 1; i < nLable; i++){
		AverageHeight += dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols);
	}
	AverageHeight /= nLable;
	AverageHeight /= 2;

	for (int i = nLable/2; i < nLable; i++){	// Lable bigger than 1/2 of average height, use middle lable as possible
		if (dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols) > AverageHeight)
		{
			int WordPerLine = 1;
			int row = dataStats GETPIX(i, CC_STAT_TOP, Stats->cols) + (dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols) / 2);
			int col = dataStats GETPIX(i, CC_STAT_LEFT, Stats->cols) - 1;
			int StartRow = row;
			int StartCol = dataStats GETPIX(i, CC_STAT_LEFT, Stats->cols) + (dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols) / 2);
			int	colNow = col;

			while (col > colNow - PROC_LINECONST){
				// Search to left side until length of PROC_LINECONST
				int LableNum = dataLables GETPIX(row, col, Lables->cols);
				if (LableNum){	// If pixel exist
					if (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats->cols) > AverageHeight){		// Lable bigger than 1/2 of average height
						row = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats->cols) + (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats->cols) / 2);
						col = dataStats GETPIX(LableNum, CC_STAT_LEFT, Stats->cols) - 1;
						StartRow = row;
						StartCol = dataStats GETPIX(LableNum, CC_STAT_LEFT, Stats->cols) + (dataStats GETPIX(LableNum, CC_STAT_WIDTH, Stats->cols) / 2);
						colNow = col;
						WordPerLine++;
					}
				}
				col--;
				if (col < 2)		break;
			}

			row = dataStats GETPIX(i, CC_STAT_TOP, Stats->cols) + (dataStats GETPIX(i, CC_STAT_HEIGHT, Stats->cols) / 2);
			col = dataStats GETPIX(i, CC_STAT_LEFT, Stats->cols) + dataStats GETPIX(i, CC_STAT_WIDTH, Stats->cols) + 1;
			colNow = col;

			while (col < colNow + PROC_LINECONST){
				// Search to right side until length of PROC_LINECONST
				int LableNum = dataLables GETPIX(row, col, Lables->cols);
				if (LableNum){	// If pixel exist
					if (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats->cols) > AverageHeight){		// Lable bigger than 1/2 of average height
						row = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats->cols) + (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats->cols) / 2);
						col = dataStats GETPIX(LableNum, CC_STAT_LEFT, Stats->cols) + dataStats GETPIX(LableNum, CC_STAT_WIDTH, Stats->cols) + 1;
						colNow = col;
						WordPerLine++;
					}
				}
				col++;
				if (col > Lables->cols - 2)		break;
			}

			if (WordPerLine > 3){	// line detect success
				// calculate tilting
				float	Angle = (float)(StartRow - row) / (float)(colNow - StartCol);
				//printf("%f   %f\n", Angle, atanf(Angle) * 180 / PI);
				return	(int)(atanf(Angle) * 180 / PI);
			}
			// line detect fail. Keep searching next line
		}
		//else{}	// Lable smaller than 1/2 of average height
	}
	// line detect fail

	return PROC_NOLINE;
}

void	Proc_FindTextLine_Info(cv::Mat* Lables, cv::Mat Stats, int nLable)
{
	LABLE_LTYPET* dataLables = (LABLE_LTYPET*)(Lables->data);
	int*	dataStats = (int*)(Stats.data);
	int		AverageHeight = 0;

	// Get Average height
	for (int i = 1; i < nLable; i++){
		AverageHeight += dataStats GETPIX(i, CC_STAT_HEIGHT, Stats.cols);
	}
	AverageHeight /= nLable;
	AverageHeight /= 2;
	Proc_LineCount = 0;

	for (int i = 1; i < nLable; i++){	// Lable bigger than 1/2 of average height
		if (!dataStats GETPIX(i, CC_STAT_AREA, Stats.cols))		continue;	// Lable which already confirmed
		if (dataStats GETPIX(i, CC_STAT_HEIGHT, Stats.cols) > AverageHeight)
		{
			int WordPerLine = 0;
			int row = dataStats GETPIX(i, CC_STAT_TOP, Stats.cols) + (dataStats GETPIX(i, CC_STAT_HEIGHT, Stats.cols) / 2);
			int col = dataStats GETPIX(i, CC_STAT_LEFT, Stats.cols) - 1;
			int	colNow = col;
			Proc_LineInfos[Proc_LineCount].Top = dataStats GETPIX(i, CC_STAT_TOP, Stats.cols);
			Proc_LineInfos[Proc_LineCount].Bottom = dataStats GETPIX(i, CC_STAT_TOP, Stats.cols) + dataStats GETPIX(i, CC_STAT_HEIGHT, Stats.cols);
			Proc_LineInfos[Proc_LineCount].Left = dataStats GETPIX(i, CC_STAT_LEFT, Stats.cols);
			Proc_LineInfos[Proc_LineCount].Right = dataStats GETPIX(i, CC_STAT_LEFT, Stats.cols) + dataStats GETPIX(i, CC_STAT_WIDTH, Stats.cols);
			//Proc_LineInfos[Proc_LineCount].WordArea[WordPerLine] = dataStats GETPIX(i, CC_STAT_AREA, Stats.cols);
			dataStats GETPIX(i, CC_STAT_AREA, Stats.cols) = 0;		// Mark confirmed lable
			WordPerLine++;

			while (col > colNow - PROC_LINECONST){
				// Search to left side until length of PROC_LINECONST
				int LableNum = dataLables GETPIX(row, col, Lables->cols);
				if (LableNum){	// If pixel exist
					if (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols) > AverageHeight){		// Lable bigger than 1/2 of average height
						row = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols) + (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols) / 2);
						col = dataStats GETPIX(LableNum, CC_STAT_LEFT, Stats.cols) - 1;
						Proc_LineInfos[Proc_LineCount].Left = dataStats GETPIX(LableNum, CC_STAT_LEFT, Stats.cols);
						if (Proc_LineInfos[Proc_LineCount].Top > dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols))
							Proc_LineInfos[Proc_LineCount].Top = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols);
						if (Proc_LineInfos[Proc_LineCount].Bottom < dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols) + dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols))
							Proc_LineInfos[Proc_LineCount].Bottom = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols) + dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols);
						//Proc_LineInfos[Proc_LineCount].WordArea[WordPerLine] = dataStats GETPIX(LableNum, CC_STAT_AREA, Stats.cols);
						dataStats GETPIX(LableNum, CC_STAT_AREA, Stats.cols) = 0;	// Mark confirmed lable
						colNow = col;
						WordPerLine++;
					}
				}
				col--;
				if (col < 2)		break;
			}

			row = dataStats GETPIX(i, CC_STAT_TOP, Stats.cols) + (dataStats GETPIX(i, CC_STAT_HEIGHT, Stats.cols) / 2);
			col = dataStats GETPIX(i, CC_STAT_LEFT, Stats.cols) + dataStats GETPIX(i, CC_STAT_WIDTH, Stats.cols) + 1;
			colNow = col;

			while (col < colNow + PROC_LINECONST){
				// Search to right side until length of PROC_LINECONST
				int LableNum = dataLables GETPIX(row, col, Lables->cols);
				if (LableNum){	// If pixel exist
					if (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols) > AverageHeight){		// Lable bigger than 1/2 of average height
						row = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols) + (dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols) / 2);
						col = dataStats GETPIX(LableNum, CC_STAT_LEFT, Stats.cols) + dataStats GETPIX(LableNum, CC_STAT_WIDTH, Stats.cols) + 1;
						Proc_LineInfos[Proc_LineCount].Right = dataStats GETPIX(LableNum, CC_STAT_LEFT, Stats.cols) + dataStats GETPIX(LableNum, CC_STAT_WIDTH, Stats.cols);
						if (Proc_LineInfos[Proc_LineCount].Top > dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols))
							Proc_LineInfos[Proc_LineCount].Top = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols);
						if (Proc_LineInfos[Proc_LineCount].Bottom < dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols) + dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols))
							Proc_LineInfos[Proc_LineCount].Bottom = dataStats GETPIX(LableNum, CC_STAT_TOP, Stats.cols) + dataStats GETPIX(LableNum, CC_STAT_HEIGHT, Stats.cols);
						//Proc_LineInfos[Proc_LineCount].WordArea[WordPerLine] = dataStats GETPIX(LableNum, CC_STAT_AREA, Stats.cols);
						dataStats GETPIX(LableNum, CC_STAT_AREA, Stats.cols) = 0;	// Mark confirmed lable
						colNow = col;
						WordPerLine++;
					}
				}
				col++;
				if (col > Lables->cols - 2)		break;
			}

			if (WordPerLine > 3){	// line detect success. Keep searching next line
				Proc_LineInfos[Proc_LineCount].nWord = WordPerLine;
				Proc_LineCount++;
			}
			// line detect fail. Keep searching next line
		}
		//else{}	// Lable smaller than 1/2 of average height
	}
	//if (Proc_LineCount){}	// if there is at least 1 line
	//else{}	// line detect fail
}
#endif

int	Proc_FindContinueLine()
{
	int		Distance = WINDOW_ROW;
	int		LineNum = 0;
	int		TempDistance;

	for (int i = 0; i < Proc_LineCount; i++){
		TempDistance = std::abs(OldLineTop - Proc_LineInfos[i].Top);
		if (Distance > TempDistance){
			Distance = TempDistance;
			LineNum = i;
		}
	}
	OldLineTop = Proc_LineInfos[LineNum].Top;
	return LineNum;
}

int	Proc_FindNewLine()
{
	int		RowCenter = WINDOW_ROW / 2;
	int		Distance = WINDOW_ROW;
	int		LineNum = 0;
	int		TempDistance;

	for (int i = 0; i < Proc_LineCount; i++){
		TempDistance = std::abs(RowCenter - ((Proc_LineInfos[i].Top + Proc_LineInfos[i].Bottom) / 2));
		if (Distance > TempDistance){
			Distance = TempDistance;
			LineNum = i;
		}
	}
	OldLineTop = Proc_LineInfos[LineNum].Top;
	return LineNum;
}

void	Proc_GetShiftedValue(int LineNum)
{
	for (int i = 0; i < Proc_LineInfos[LineNum].nWord; i++){
		//WordAreaOld[i] = Proc_LineInfos[LineNum].WordArea[i];
	}
}

void	Proc_DeleteCuttedWord(cv::Mat* Image, int Connectivity)
{
	LABLE_LTYPET* dataImage = (LABLE_LTYPET*)(Image->data);

	for (int row = 0; row < Image->rows; row++){
		if (dataImage GETPIX(row, 0, Image->cols)){

		}
		if (dataImage GETPIX(row, Image->cols, Image->cols)){

		}
	}

	for (int col = 0; col < Image->cols; col++){
		if (dataImage GETPIX(0, col, Image->cols)){

		}
		if (dataImage GETPIX(Image->rows, col, Image->cols)){

		}
	}
}

bool	Proc_MakeFinalText(char* outText, char* confirmedText)
{
	// delete /n from tesseract output
	int		outTextlen = (int)strlen(outText) - 2;
	int		CnfTextlen = (int)strlen(confirmedText) - 2;
	if (outText[outTextlen] == '\n')		outText[outTextlen] = 0;
	if (confirmedText[CnfTextlen] == '\n')	confirmedText[CnfTextlen] = 0;


	int		TestCount;
	int		ErrorCount;

	if (CnfTextlen < 2)		return false;

	for (int t = 0; t < CnfTextlen - 2; t++)
	{
		if (t > PROC_GIVEUP_CONST)	return false;
		for (int i = 1; i < outTextlen; i++){
			if (confirmedText[CnfTextlen - 2 - t] == outText[outTextlen - i])	// search except last character of 'confirmedText' -> last character could be cutted at border
			{
				//printf("%d %d\n", outTextlen, CnfTextlen);
				TestCount = 1;
				ErrorCount = 0;

				for (int nTest = 1; outTextlen - nTest - i >= 0; nTest++){
					if (CnfTextlen - nTest - 2 < 0)		break;				// prevent memory allocation error
					if (confirmedText[CnfTextlen - nTest - 2 - t] == outText[outTextlen - nTest - i])		TestCount++;
					else																				ErrorCount++;
					if (ErrorCount > TestCount)			break;				// stop searching if too many error
				}

				if (ErrorCount < TestCount / ERROR_ENDURANCE_DIV){			// searching success
					if (CnfTextlen + i > MAX_READ_CHAR)		return false;	// exceed max lenght of matrix
					for (int a = 0; a < i - 1; a++){							// copy  confirmedText 의 마지막 글자를 겹치게 복사
						confirmedText[CnfTextlen - 1 + a - t] = outText[outTextlen - i + 1 + a];
					}
					confirmedText[CnfTextlen + i - 1 - t] = 0;						// NULL 문자

					return true;
				}
				//실패 -> 반복계속
			}
		}
	}
	return	false;
}


//================================= Not Used =========================================//


void	Proc_BlobSmallSizeConstraint(cv::Mat* Lables, cv::Mat* Stats, int Area)
{
	LABLE_LTYPET* dataLables = (LABLE_LTYPET*)(Lables->data);
	int* dataStats = (int*)(Stats->data);

	for (int row = 0; row < Lables->rows; row++){
		for (int col = 0; col < Lables->cols; col++)
		{
			if (dataLables GETPIX(row, col, Lables->cols)){
				if (dataStats GETPIX(dataLables GETPIX(row, col, Lables->cols), CC_STAT_AREA, Stats->cols) < Area){
					dataLables GETPIX(row, col, Lables->cols) = 0;
				}
			}
		}
	}
}


void	Proc_BlobBigSizeConstraint(cv::Mat* Lables, cv::Mat* Stats, int nWidth, int nHeight)
{
	LABLE_LTYPET* dataLables = (LABLE_LTYPET*)(Lables->data);
	int* dataStats = (int*)(Stats->data);

	for (int row = 0; row < Lables->rows; row++){
		for (int col = 0; col < Lables->cols; col++)
		{
			if (dataStats GETPIX(dataLables GETPIX(row, col, Lables->cols), CC_STAT_WIDTH, Stats->cols) > nWidth){
				dataLables GETPIX(row, col, Lables->cols) = 0;
				continue;
			}

			if (dataStats GETPIX(dataLables GETPIX(row, col, Lables->cols), CC_STAT_HEIGHT, Stats->cols) > nHeight){
				dataLables GETPIX(row, col, Lables->cols) = 0;
				continue;
			}
		}
	}
}
