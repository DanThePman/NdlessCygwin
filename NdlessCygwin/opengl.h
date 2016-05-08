#include "nGL\gl.h"
#include "Textures\menu.h"
#include "Textures\result.h"
#include <string>

#define SCREEN_RELATION SCREEN_WIDTH / SCREEN_HEIGHT
/*streches the width of a quad in order to fill the screen*/
#define ScreenFill(value) value*SCREEN_RELATION

/*for taskmgr*/
#define WindowCount 3

const VERTEX QuadPlane[] =
{
	{ ScreenFill(-100), -100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(-100), 100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(100), 100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(100), -100, 0, 0, 0, 0xFFFF },

	/*to rotate around y*/
	{ ScreenFill(-100), 100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(-100), -100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(100), -100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(100), 100, 0, 0, 0, 0xFFFF },
	
};

enum glCurrentWindow
{
	/*also input*/
	Menu,
	Results,
};

class WindowTranslation
{
private:
	int win1X, win1Z;
	int win2X, win2Z;
	const int OnFrontZTranslation = 100;
	const int windowTranslationZSpeed = 10;
public:
	int GetFrontWindowZ() { return OnFrontZTranslation; }
	int GetTranslationZSpeed() { return windowTranslationZSpeed; }
	
	void SetWin1TranslationXZ(int x, int z){win1X = x; win1Z = z;}
	void SetWin2TranslationXZ(int x, int z){win2X = x; win2Z = z; }
	
	int GetTranslatedX(glCurrentWindow window)
	{
		switch (window)
		{
		case glCurrentWindow::Menu:
			return win1X;
		case glCurrentWindow::Results:
			return win2X;
		}
	}
	
	int GetTranslatedZ(glCurrentWindow window)
	{
		switch (window)
		{
		case glCurrentWindow::Menu:
			return win1Z;
		case glCurrentWindow::Results:
			return win2Z;
		}
	}
};

COLOR *framebuffer;
WindowTranslation* windowTranslater;




/*Const linear field of view*/
void InitOpenGl()
{
	nglInit();
	// Allocate the framebuffer
	framebuffer = new COLOR[SCREEN_WIDTH * SCREEN_HEIGHT];
	windowTranslater = new WindowTranslation();
	nglSetBuffer(framebuffer);
}

void UninitOpenGl()
{
	// Deinitialize nGL
	nglUninit();
	delete[] framebuffer;
	delete windowTranslater;
}

void FlySwitchWindow()
{	
	glBindTexture(&menu);
	
	int zTranlationOffset = 0;
	for (;zTranlationOffset < 1000;)
	{
		glPushMatrix();
		glColor3f(0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glTranslatef(0, 0, 200 + zTranlationOffset);
		zTranlationOffset += 15;
		glBegin(GL_QUADS);
			nglAddVertices(QuadPlane, 4);
		glEnd();
		nglDisplay();
		glPopMatrix();
		
	}
	
	int angle = 0;
	for (; angle < 361;)
	{
		GLFix fixAngle = GLFix(angle);
		
		glPushMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(0.4f, 0.7f, 1.0f);
		glTranslatef(0, 0, 200 + zTranlationOffset);
		angle += 5;
		nglRotateY(fixAngle.normaliseAngle());
		glBegin(GL_QUADS);
			nglAddVertices(fixAngle < GLFix(90) || fixAngle > GLFix(270) ? QuadPlane : (QuadPlane + 4), 4);
		glEnd();
		glPopMatrix();
		nglDisplay();
	}
	
	for (; zTranlationOffset >= 0;)
	{
		glPushMatrix();
		glColor3f(0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glTranslatef(0, 0, 200 + zTranlationOffset);
		zTranlationOffset -= 15;
		glBegin(GL_QUADS);
		nglAddVertices(QuadPlane, 4);
		glEnd();
		nglDisplay();
		glPopMatrix();
	}
}

//===================================================================================================================

/*base windowplane win*/
VERTEX baseWindowPlane[] =
{
	{ ScreenFill(-100), -100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(-100), 100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(100), 100, 0, 0, 0, 0xFFFF },
	{ ScreenFill(100), -100, 0, 0, 0, 0xFFFF },
	
};

void InitTaskWindows()
{
	int minusz = 0;
	int plusx = 0;
	
	int win1x, win1z, win2x, win2z;
	
	for (; minusz <= 400 && !isKeyPressed(KEY_NSPIRE_ESC);)
	{
		win1x = -plusx, win1z = 200 + minusz;
		win2x = plusx, win2z = 200 + minusz;
		
		glBindTexture(&menu);
		
		glPushMatrix();
		glColor3f(0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
		
		glTranslatef(win1x, 0, win1z);
		glBegin(GL_QUADS);
			nglAddVertices(baseWindowPlane, 4);
		glEnd();
		
		glPopMatrix();
		
		glBindTexture(&menu);
		glPushMatrix();
		
		glBindTexture(&loesung);
		glTranslatef(win2x, 0, win2z);
		glBegin(GL_QUADS);
			nglAddVertices(baseWindowPlane, 4);
		glEnd();
		
		nglDisplay();
		glPopMatrix();
		
		minusz += 20;
		plusx += 8;
	}
	
	windowTranslater->SetWin1TranslationXZ(win1x, win1z);
	windowTranslater->SetWin2TranslationXZ(win2x, win2z);
}

void MoveTaskWindow(glCurrentWindow movingWindow, bool toFront)
{	
	int win1x = windowTranslater->GetTranslatedX(glCurrentWindow::Menu);
	int win2x = windowTranslater->GetTranslatedX(glCurrentWindow::Results);
	
	int win1z = windowTranslater->GetTranslatedZ(glCurrentWindow::Menu);
	int win2z = windowTranslater->GetTranslatedZ(glCurrentWindow::Results);
	
	int zoffset = windowTranslater->GetFrontWindowZ();
	int zspeed = windowTranslater->GetTranslationZSpeed();
	
	int win1DestZ;
	int win2DestZ;
	
	if (toFront)
	{
		win1DestZ = movingWindow != glCurrentWindow::Menu ? win1z + zoffset : win1z - zoffset;
		win2DestZ = movingWindow != glCurrentWindow::Results ? win2z + zoffset : win2z - zoffset;
	}
	else
	{
		win1DestZ = movingWindow != glCurrentWindow::Menu ? win1z - zoffset*2 : win1z + zoffset*2;
		win2DestZ = movingWindow != glCurrentWindow::Results ? win2z - zoffset * 2: win2z + zoffset * 2;
	}
	
	for (; abs(win1z - win1DestZ) > 10 || abs(win2z - win2DestZ) > 10;)
	{		
		glBindTexture(&menu);
		glPushMatrix();
		glColor3f(0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glTranslatef(win1x, 0, win1z);
		glBegin(GL_QUADS);
			nglAddVertices(baseWindowPlane, 4);
		glEnd();
		
		glPopMatrix();
		
		glBindTexture(&loesung);
		glPushMatrix();	
		
		glTranslatef(win2x, 0, win2z);
		glBegin(GL_QUADS);
			nglAddVertices(baseWindowPlane, 4);
		glEnd();
		
		nglDisplay();
		glPopMatrix();
		
		if (abs(win1z - win1DestZ) > 10)
		{
			win1z += win1z > win1DestZ ? -zspeed : zspeed;
		}
		if (abs(win2z - win2DestZ) > 10)
		{
			win2z += win2z > win2DestZ ? -zspeed : zspeed;
		}
	}
	
	windowTranslater->SetWin1TranslationXZ(win1x, win1z);
	windowTranslater->SetWin2TranslationXZ(win2x, win2z);
}
/*After pressing enter*/
void MaximizeTaskWindow(glCurrentWindow win)
{
	int win1x = windowTranslater->GetTranslatedX(glCurrentWindow::Menu);
	int win2x = windowTranslater->GetTranslatedX(glCurrentWindow::Results);
	
	int win1z = windowTranslater->GetTranslatedZ(glCurrentWindow::Menu);
	int win2z = windowTranslater->GetTranslatedZ(glCurrentWindow::Results);
	
	int zoffset = windowTranslater->GetFrontWindowZ();
	
	int destwinx = (int)win == 0 ? win1x : win2x, destwinz = (int)win == 0 ? win1z : win2z;
	
	for (; (abs(destwinz - (220 - zoffset)) > 10 || abs(destwinx) > 10) && !isKeyPressed(KEY_NSPIRE_ESC) /*x => 0*/;)
	{
		glBindTexture(&menu);
		
		glPushMatrix();	
		glColor3f(0.4f, 0.7f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glTranslatef(win1x, 0, win1z);
		glBegin(GL_QUADS);
			nglAddVertices(baseWindowPlane, 4);
		glEnd();
		
		glPopMatrix();
		
		glBindTexture(&loesung);
		glPushMatrix();
		
		glTranslatef(win2x, 0, win2z);
		glBegin(GL_QUADS);
			nglAddVertices(baseWindowPlane, 4);
		glEnd();
		
		nglDisplay();
		glPopMatrix();
		
		if ((int)win == 0)
		{
			win1z += (220 - zoffset) ? -20 : 20;
			win1x += win1x > 0 ? -8 : 8;
		}
		else
		{
			win2z += win2z > (220 - zoffset) ? -20 : 20;
			win2x += win2x > 0 ? -8 : 8;
		}
		destwinx = (int)win == 0 ? win1x : win2x, destwinz = (int)win == 0 ? win1z : win2z;
	}
	
	windowTranslater->SetWin1TranslationXZ(win1x, win1z);
	windowTranslater->SetWin2TranslationXZ(win2x, win2z);
}

extern void ClearWindowAnimation();
extern void PrintMenu();
extern void PrintResults();
extern std::string totalUserInput;

void glOpenTaskMgr(glCurrentWindow mode)
{
	ClearWindowAnimation();
	InitTaskWindows();
	
	int currentSelectionIndex = (int)mode, lastSelectedIndex = -1;
	bool running = true;

	while (running)
	{
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				/* Quit */
				running = false;
				break;
			case SDL_KEYDOWN:
				bool leftPressed = isKeyPressed(KEY_NSPIRE_LEFT);
				bool rightPressed = isKeyPressed(KEY_NSPIRE_RIGHT);
				bool enterPressed = isKeyPressed(KEY_NSPIRE_ENTER);
				
				bool firstRun = lastSelectedIndex == -1;
				
				if (enterPressed)
				{
					running = false;
					break;
				}
				
				currentSelectionIndex -= leftPressed ? 1 : -1;
				if (currentSelectionIndex < 0) currentSelectionIndex = 1;
				if (currentSelectionIndex > 1) currentSelectionIndex = 0;
				
				if (!firstRun)
				{
					//make old window small and new one large
					MoveTaskWindow((glCurrentWindow)lastSelectedIndex, false);
				}
				else /*scale current selected window*/
				MoveTaskWindow((glCurrentWindow)currentSelectionIndex, true);
				
				lastSelectedIndex = currentSelectionIndex;
			}
		}
	}
	
	MaximizeTaskWindow((glCurrentWindow)currentSelectionIndex);
	
	if (currentSelectionIndex == 0)
	{
		totalUserInput = "";
		PrintMenu();
		sdl_InitSelection(20, 15, TEXTINPUTBOX_Height);
	}
	else PrintResults();
}