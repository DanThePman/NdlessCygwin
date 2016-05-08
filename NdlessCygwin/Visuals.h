#include "opengl.h"

#define euler 2.71828182846

extern ngc_Printing* ngcPrintInstance;
Gc maingc;

/*draws white background for ngc*/
void ngc_fillScreen()
{
	maingc = ngcPrintInstance->GetGraphicDevice();
	
	gui_gc_setRegion(maingc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	gui_gc_begin(maingc);
	gui_gc_setColorRGB(maingc, 255, 255, 255);
	
	gui_gc_fillRect(maingc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	gui_gc_blit_to_screen(maingc);
	
	gui_gc_finish(maingc);
}

/*renders Borders*/
void RenderBorder()
{
	maingc = ngcPrintInstance->GetGraphicDevice();
	gui_gc_setRegion(maingc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	gui_gc_begin(maingc);

	gui_gc_setColorRGB(maingc, 0, 0, 0);

#pragma region border

	gui_gc_drawLine(maingc, 5, 5, SCREEN_WIDTH - 5, 5);
	gui_gc_drawLine(maingc, 5, 5, 5, SCREEN_HEIGHT - 5);
	gui_gc_drawLine(maingc, 5, SCREEN_HEIGHT - 5, SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5);
	gui_gc_drawLine(maingc, SCREEN_WIDTH - 5, 5, SCREEN_WIDTH - 5, SCREEN_HEIGHT - 5);

#pragma endregion

	gui_gc_blit_to_screen(maingc);

	gui_gc_finish(maingc);
}

///*dest = finished position of scaled object ; start = window position to scale*/
//void ScaleGc(int startx1, int starty1, int startx2, int starty2, /**/ int destx1, int desty1, int destx2, int desty2)
//{
	//gui_gc_begin(maingc);
//
	//Gc copy = gui_gc_copy(maingc, SCREEN_WIDTH, SCREEN_HEIGHT);
//
	///*put scaling in copy*/
	//gui_gc_begin(copy);
		//gui_gc_blit_gc(maingc, startx1, starty1, startx2, starty2, copy, destx1, desty1, destx2, desty2);
	//gui_gc_finish(copy);
//
//#pragma region resetbackfield
	//gui_gc_setColorRGB(maingc, 255, 255, 255);
//
		//gui_gc_fillRect(maingc, 0, 0, destx1, SCREEN_HEIGHT);//prerendering left
		//gui_gc_fillRect(maingc, destx2, 0, SCREEN_WIDTH - destx2, SCREEN_HEIGHT);
//
		//gui_gc_fillRect(maingc, destx1, 0, destx2, desty1);//prerendering top
		//gui_gc_fillRect(maingc, 0, desty2, SCREEN_WIDTH, SCREEN_HEIGHT - desty2);
//
	//gui_gc_blit_to_screen(maingc);
//#pragma endregion
//
	///*put copy back in original*/
	//gui_gc_blit_gc(copy, destx1, desty1, destx2, desty2, maingc, destx1, desty1, destx2, desty2);
	//gui_gc_free(copy);
//
	//gui_gc_blit_to_screen(maingc);
//
	//gui_gc_finish(maingc);
//}

/*SLOW CODE*/
//void MinimizeWindow()
//{
	//maingc = ngcPrintInstance->GetGraphicDevice();
//
	//float curr_start_x = 0;
	//float curr_start_y = 0;
//
	//float curr_end_x = SCREEN_WIDTH;
	//float curr_end_y = SCREEN_HEIGHT;
//
	//float dest_start_x = 10;
	//float dest_start_y = 10;
//
	//float dest_end_x = SCREEN_WIDTH / 2;
	//float dest_end_y = SCREEN_HEIGHT / 2;
//
	//float screenFactor = SCREEN_WIDTH / SCREEN_HEIGHT;
	//float minusx = 30 * screenFactor, minusy = 30;
//
	//while (curr_end_x > dest_end_x || curr_end_y > dest_end_y)
	//{
		//ScaleGc(0, 0, curr_end_x, curr_end_y /*  */, 0, 0, curr_end_x- minusx, curr_end_y- minusy);
//
		//curr_end_x-= minusx;
		//curr_end_y-= minusy;
	//}
//}

struct rect
{
public:
	float w, h;

	/*polygon style buggy*/
	void fillOutside()
	{
		rect rectangle = *this;
		gui_gc_begin(maingc);
		gui_gc_setColorRGB(maingc, 102, 179, 255);

		/*top*/
		gui_gc_fillRect(maingc, 0, 0, SCREEN_WIDTH, rectangle.h);

		/*right*/
		gui_gc_fillRect(maingc, rectangle.w, 0, SCREEN_WIDTH - rectangle.w, SCREEN_HEIGHT);

		gui_gc_blit_to_screen(maingc);
		gui_gc_finish(maingc);
	}
};


/*Top right to bottom left*/
void ClearWindowAnimation()
{
	maingc = ngcPrintInstance->GetGraphicDevice();

	int currentWidth = SCREEN_WIDTH;
	int currentHeight = 0;

	float screenFactor = SCREEN_WIDTH / SCREEN_HEIGHT;
	const float minusx = 10 * screenFactor, plusy = 10;

	for (; currentWidth > 0; currentWidth-= minusx)
	{
		rect currentScalingRect;
		currentScalingRect.w = currentWidth;
		currentScalingRect.h = currentHeight;
		
		currentScalingRect.fillOutside();

		currentHeight += plusy;

		if (isKeyPressed(KEY_NSPIRE_ESC))
			break;
	}

	gui_gc_finish(maingc);
}

void OpenTaskMgr(glCurrentWindow mode) { glOpenTaskMgr(mode); }

/*f(x) = 5 * e^(-0.0089x) => f(180) = 1*/
float Unaccelarate_fx(float x)
{
	return 5* std::pow(euler, (-0.0089f*x));
}

void DoInputAnimation()
{
	int currentStartX = SCREEN_WIDTH;
	const int destX = SCREEN_WIDTH / 2 + 30;
	
	int width = SCREEN_WIDTH - currentStartX;
	
	for (; currentStartX > destX;)
	{
		gui_gc_begin(maingc);
		gui_gc_setColorRGB(maingc, 102, 179, 255);
		
		gui_gc_fillRect(maingc, currentStartX, 10, width, SCREEN_HEIGHT - 20);
		gui_gc_blit_to_screen(maingc);
	
		gui_gc_finish(maingc);
		
		/*negative accelaration*/
		currentStartX -= Unaccelarate_fx(width);
		width = SCREEN_WIDTH - currentStartX;
		
		RenderBorder();
	}
}

void DoInputAnmiationEnd()
{
	const int startX = SCREEN_WIDTH / 2 + 30;
	
	const int destWidth = SCREEN_WIDTH - startX;
	int currentWidth = 0;
	
	for (; currentWidth <= destWidth;)
	{
		gui_gc_begin(maingc);
		gui_gc_setColorRGB(maingc, 255, 255, 255);
		
		gui_gc_fillRect(maingc, startX, 10, currentWidth, SCREEN_HEIGHT - 20);
		gui_gc_blit_to_screen(maingc);
	
		gui_gc_finish(maingc);
		
		/*negative accelaration*/
		currentWidth += Unaccelarate_fx(currentWidth);
		
		RenderBorder();
	}
}

void PrintMenu()
{	
	ngc_fillScreen();
	RenderBorder();
	
	ngcPrintInstance->nGC_PrintText("Menu", 20, 10, Bold12);

	for (int i = 0; i < menuList.size(); i++)
	{
		int yFactor = i + 1;
		ngcPrintInstance->nGC_PrintText((char*)menuList[i].c_str(), 20, 15 + TEXTINPUTBOX_Height*yFactor);
	}
}

void PrintResults()
{
	ngc_fillScreen();
	RenderBorder();
	
	//ngcPrintInstance->nGC_PrintText((char*)totalUserInput.c_str());
	
	CalculationType type;
	string solvingWay;
	sdlCalcuate(type, solvingWay);
	
	ngcPrintInstance->nGC_PrintText((char*)solvingWay.c_str(), 20, 20);
	
	sdl_WaitUserKey();
	OpenTaskMgr(glCurrentWindow::Menu);
}

