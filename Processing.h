


/*  잡영처리 + 다시 이진화  */

void	Proc_BlobConstraint(cv::Mat* dst, cv::Mat* Lables, cv::Mat* Stats);




/*  줄을 찾고 그 줄 정보를 반환 - 잘린 글자 처리가 선행되어야 성능 좋음  */

#define		PROC_NOLINE		1000			// 그냥 상수. 계산에 쓰이는 녀석은 아님
#define		PROC_LINECONST	300				// 'PROC_LINECONST 만큼이 양 옆 검색범위 (그냥 크게 잡아둠)
typedef struct{
	int		Top;
	int		Left;
	int		Bottom;
	int		Right;
	int		nWord;							// 글자 수
	//int		WordArea[MAX_READ_WORD];		// 각 글자의 면적
}LineInfo;
extern		int			Proc_LineCount;
extern		LineInfo	Proc_LineInfos[MAX_READ_LINE];
// 포인터를 받는 이유는 메모리 절약 위해서. 내부에서 변경하며 사용하는 Info 함수에서는 그냥 연결 -> 바꾸면 좋겠는데..
int			Proc_FindTextLine_Ang(cv::Mat* Lables, cv::Mat *Stats, int nLable);		// 각도 반환 : 없을시 PROC_NOLINE 반환
void		Proc_FindTextLine_Info(cv::Mat* Lables, cv::Mat Stats, int nLable);		// Proc_LineInfos 설정 : 각도 맞춰진 상태




/*  현재 줄 판별  */

extern	int		OldLineTop;			// 읽던 줄의 Top
int		Proc_FindContinueLine();	// 읽던 줄을 찾기
int		Proc_FindNewLine();			// 새로운 줄(화면 중심에 가까운 줄)을 찾기



/*  이미지 시프트 측정 - 일정 이상 시프트 되어야지만 문자인식(속도 느리기 때문)  */

extern	int		WordAreaOld[MAX_READ_WORD];			// 읽던 줄 각 글자의 면적
void	Proc_GetShiftedValue(int LineNum);




/*  잘린 글자 처리  */

void	Proc_DeleteCuttedWord(cv::Mat* Image, int Connectivity);




/*  최종 문자열 생성 - 반환은 성공여부 0 or 1  */

#define ERROR_ENDURANCE_DIV		3
#define PROC_GIVEUP_CONST		5
bool	Proc_MakeFinalText(char* outText, char* confirmedText);




//================================= 미사용 =========================================//


/*  픽셀 크기가 비교값 보다 작은 레이블 제거  */

void	Proc_BlobSmallSizeConstraint(cv::Mat* Lables, cv::Mat* Stats, int Area);




/*  가로 혹은 세로 크기가 비교값 보다 큰 레이블 제거  */
/*  이걸로 필터링시 누가 밑줄 그어놓으면 다 지워버리게 되는 문제점  */

void	Proc_BlobBigSizeConstraint(cv::Mat* Lables, cv::Mat* Stats, int nWidth, int nHeight);

