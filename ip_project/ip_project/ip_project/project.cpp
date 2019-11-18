#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <Windows.h>

int face_detect(int, char **);  //동영상 실행 & 카메라 연결
void _face_detect_and_draw(IplImage *); //인식 후 네모를 그리기 위한 함수
void Mosaic(IplImage* image, CvRect *face); //모자이크 처리 함수

static CvMemStorage *storage = 0; //계산하기 위한 메모리 공간 생성
static CvHaarClassifierCascade *cascade = 0; //Haar 기반 cascade 분류기 변수 생성 

void gotoxy(int x, int y); //화면제어 함수

int main(int argc, char **argv) {
	int menu = 0;

	const char *Menu[] = { "1.얼굴 인식 : ", "2.종료 : "};

	system("cls");
	gotoxy(40, 10); //화면제어를 통해 출력 위치 변경 후 메뉴 출력
	printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
	gotoxy(40, 11);
	printf("┃           자동 모자이크            ┃\n");
	gotoxy(40, 12);
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	gotoxy(40, 13);
	printf("┃%-36s┃\n", Menu[0]);
	gotoxy(40, 14);
	printf("┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n");
	gotoxy(40, 15);
	printf("┃%-36s┃\n", Menu[1]);
	gotoxy(40, 16);
	printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\n");
	gotoxy(40, 17);
	printf(" 메뉴 선택 : ");
	scanf("%d", &menu);


	switch (menu) { //입력 된 값 전달
	case 1:
		face_detect(argc, argv);
		break;
	case 2:
		printf("종료 \n");
		break;
	default:
		gotoxy(40, 18);
		printf("ERROR : Wrong Input\n"); //원하던 값 이외의 값 입력시 출력
		break;
	}
	gotoxy(40, 19);
}
int face_detect(int argc, char **argv) { //동영상 실행 & 카메라 연결
	CvCapture *capture = 0; //영상의 정보를 얻을 구조체
	IplImage *frame; //정지영상을 저장할 변수
	const char *cascade_name = "C:/data/haarcascades/haarcascade_frontalface_alt2.xml"; //xml파일을 cascade_name에 할당 //xml파일 설명

	if (!(cascade = (CvHaarClassifierCascade *)cvLoad(cascade_name, 0, 0, 0))) { //xml파일을 읽어 얻는 HaarClassifierCascade를 할당
		fprintf(stderr, "ERROR: Couldn't load classifier cascade\n"); //읽어 오지 못 할 경우 출력 후 종료
		return -1;
	}

	storage = cvCreateMemStorage(0); //메모리 저장 공간 할당
	capture = cvCaptureFromCAM(0); //카메라 연결 초기화

	cvNamedWindow("M180518_이상욱_Result"); //영상을 출력하기 위한 윈도우를 만드는 함수

	while (!0) {
		if (!(frame = cvQueryFrame(capture))) //영상의 프레임을 잡고 저장해준다.
			break;
		_face_detect_and_draw(frame); //반환한 프레임을 영상 내 얼굴 추출 및 추출 결과 표시함수로 보낸다.
		if (cvWaitKey(10) >= 0) //키보드 입력을 기다린 후 키보드 값을 반환한다.
			break;

	}
	cvReleaseCapture(&capture); //capture의 메모리할당 해제
	cvDestroyWindow("M180518_이상욱_Result"); //윈도우를 해체합니다.
	return 0;
}

void Mosaic(IplImage* img, CvRect *face) { 
	int cnt = 0;
	int temp = 9;
	int w_point = 0, h_point = 0;
	int y_point = 0, x_point = 0;
	double R = 0, G = 0, B = 0;

	//영상의 색의 평균을 구해서 그 색을 채워준다.
	//색의 평균이기 때문에 영상이 흐릿하게 나온다.
	for (int i = 0; i < face->height / temp; i++) {
		for (int j = 0; j < face->width / temp; j++) {
			cnt = 0;
			B = 0; G = 0; R = 0;

			x_point = face->x + (j * temp);
			y_point = face->y + (i * temp);

			//변수에 계속 더해줍니다.
			for (int k = y_point; k < y_point + temp; k++) {
				for (int m = x_point; m < x_point + temp; m++) {
					CvScalar color;
					w_point = m;
					h_point = k;

					if (m >= img->width) {
						w_point = img->width - 1;
					}

					if (k >= img->height) {
						h_point = img->height - 1;
					}

					color = cvGet2D(img, h_point, w_point); //h,w에 있는 픽셀의 색을 저장합니다.

					B += color.val[0];
					G += color.val[1];
					R += color.val[2];
					cnt++;
				}
			}

			//평균을 계산합니다.
			B /= cnt; 
			G /= cnt;
			R /= cnt;

			CvScalar color;

			color.val[0] = B;
			color.val[1] = G;
			color.val[2] = R;

			CvPoint front;
			CvPoint rear;
				
			front.x = x_point;
			front.y = y_point;
			rear.x = x_point + temp;
			rear.y = y_point + temp;

			cvRectangle(img, front, rear, color, CV_FILLED, 8, 0);
			//위의 결과로 그려진 모자이크 사각형을 그립니다.
		}
	}
}

void _face_detect_and_draw(IplImage *img) {
	static CvScalar colors[] = { { { 0,0,255 } } ,{ { 0,128,255 } },{ { 0,255,255 } },{ { 0,255,0 } },
	{ { 255,128,0 } },{ { 255,255,0 } } ,{ { 255,0,0 } },{ { 255,0,255 } } }; //표시 할 색을 저장합니다.
	double scale = 2.0; //계산 속도 빠르게 하기 위해 영상 2배 축소 처리

	IplImage *gray = cvCreateImage(cvSize(img->width, img->height), 8, 1); //넘겨 받은 img의 정보를 저장합니다.
	IplImage *small_Img = cvCreateImage(cvSize(cvRound(img->width / scale), cvRound(img->height / scale)), 8, 1); //넘겨 받은 img의 정보를 조정 후에 저장합니다.

	CvRect max; //가장 큰 영상의 영역을 저장하기 위한 변수
	CvRect face; //영상의 영역을 저장하기 위한 변수

	cvCvtColor(img, gray, CV_BGR2GRAY); //컬러 영상을 명암도 영상으로 변환합니다.
	cvResize(gray, small_Img, CV_INTER_LINEAR); //양선형 보간법으로 축소한 영상 데이터를 얻습니다. //양선형 보간법설명
	cvClearMemStorage(storage); //저장된 메모리를 초기화합니다.

	if (cascade) {
		double t = (double)cvGetTickCount(); //수행시간을 얻기 위한 시간 계산준비
		//영상 내의 얼굴 추출 후 seq형태로 반환한다.
		CvSeq *faces = cvHaarDetectObjects(small_Img, cascade, storage, 1.1, 2, 0, cvSize(30, 30), cvSize(300, 300)); //핵심 함수설명
		t = (double)cvGetTickCount() - t; //수행시간 계산
		printf("Detection tiem : %gms\n", t / ((double)cvGetTickFrequency() * 1000)); //수행시간 출력

		max.width = 0; 
		max.height = 0;

		//추출 한 얼굴을 사각형 형태로 차례대로 표시합니다.
		for (int i = 0; i < (faces ? faces->total : 0); i++) { 
			CvRect *r = (CvRect *)cvGetSeqElem(faces, i); //Memstorage안의 저장된 객체를 cvRect형으로 저장합니다.
		    //생성 할 사각형의 대각선에 위치한 두 모서리의 좌표를 찍어줍니다.
			CvPoint front;
			CvPoint rear;
			front.x = cvRound((r->x + r->width*0.5)*scale);
			front.y = cvRound((r->y + r->width*0.5)*scale);
			int radius = cvRound((r->width + r->height)*0.25*scale);
			rear.x = front.x + radius;
			rear.y = front.y + radius;
			front.x -= radius;
			front.y -= radius;
			cvRectangle(img, front, rear, colors[i & 8], 3, 8, 0); 
			//위에 지정한 두 점으로 인식한 모든 얼굴에 사각형 테두리 그리기

			//가장 큰 네모를 찾기위해 face에 저장합니다.
			face.x = front.x;
			face.y = front.y;
			face.width = rear.x - front.x;
			face.height = rear.y - front.y;

			//가장 큰 사각형을 모자이크 되지 않습니다.
			if (max.width * max.height < face.width * face.height) 
			{   //전에 사용했던 얼굴인식 사각형의 크기보다 지금 사용하는 얼굴인식 사각형의 크기가 클경우 
				//전의 사각형안은 모자이크하고 지금 사각형을 저장합니다. 크지 않을 경우 그냥 모자이크합니다.
				Mosaic(img, &max);
				max.x = face.x;
				max.y = face.y;
				max.width = face.width;
				max.height = face.height;
			}
			else
				Mosaic(img, &face);
		}
	}

	cvShowImage("M180517_이상욱_Result", img); //결과로 나온 영상 보여주기
	cvReleaseImage(&gray); //이미지를 해제합니다.
	cvReleaseImage(&small_Img); //이미지를 해제합니다.
}

void gotoxy(int x, int y) //화면을 제어 하기 위한 함수
{
	COORD pos = { x - 1, y - 1 };

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}