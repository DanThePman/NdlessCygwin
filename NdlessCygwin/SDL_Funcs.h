#include <SDL/SDL.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Printing.h"
#include "FloatToString.h"

SDL_Surface *screen;
nSDL_Font *font;
extern vector<string> menuList;
extern string totalUserInput;
extern int selectedIndex;
extern ngc_Printing* ngcPrintInstance;

void sdl_WaitUserKey(bool enterkey);
void requestInputBox();

string tostr(int num)
{
	char output[15];

	snprintf(output, 15, "%i", num);
	return string(output);
}

void sdl_Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		printf("Die SDL-Engine konnte nicht initialisiert werden : %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	screen = SDL_SetVideoMode(320, 240, has_colors ? 16 : 8, SDL_SWSURFACE);
	if (screen == NULL)
	{
		printf("Konnte Display nicht initialisieren: %s\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
	SDL_ShowCursor(SDL_DISABLE);

	font = nSDL_LoadFont(NSDL_FONT_VGA, 0, 0, 0);

	SDL_Rect rect = { 0, 0, 320, 240 };
	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 255, 255));
	SDL_Flip(screen);
}

void sdl_Finished()
{
	nSDL_FreeFont(font);
	SDL_FreeSurface(screen);
}

/*Print Text using SDL (not that fancy)*/
void sdl_PrintScreen(const char * msg, int x, int y)
{
	nSDL_DrawString(screen, font, x, y, msg);
	SDL_Flip(screen);
}

extern void FlySwitchWindow();
extern void PrintResults();
/*sets selection index after pressing enter*/
void sdl_InitSelection(int xstart, int ystart, float ystep, vector<string> menuStrings = menuList) 
{	
	int currentSelectionIndex = 0;	
	float lastSelectedY = -1;
	float lastSelectedX = -1;

selectionLoopBegin:
	ngcPrintInstance->SetTextClearBackgroundColor(0xFFFFFF);
	
	bool running = true, exit = false;
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
				bool downPressed = isKeyPressed(KEY_NSPIRE_DOWN);
				bool upPressed = isKeyPressed(KEY_NSPIRE_UP);
				bool enterPressed = isKeyPressed(KEY_NSPIRE_ENTER);
				bool escPressed = isKeyPressed(KEY_NSPIRE_ESC);
				
				if (escPressed)
				{
					exit = true;
					running = false;
					break;
				}
				
				if (enterPressed)
				{
					running = false;
					break;
				}
				if (!(downPressed || upPressed))
					break;

				/*not first run*/
				if (lastSelectedY != -1 && lastSelectedX != -1)
				{
					int lastIndex = currentSelectionIndex;
					/*restore normal font*/
					ngcPrintInstance->nGC_PrintText((char*)menuStrings[lastIndex].c_str(), lastSelectedX, lastSelectedY);
				}

				/*first run*/
				if (lastSelectedX == -1 && lastSelectedY == -1)
				{
					/*pressing up results in first index*/
					currentSelectionIndex += !upPressed ? 0 : -1;
				}
				else
					currentSelectionIndex += !upPressed ? 1 : -1;

				if (currentSelectionIndex < 0) currentSelectionIndex = menuStrings.size() - 1;
				if (currentSelectionIndex > menuStrings.size() - 1) currentSelectionIndex = 0;

				int selectedElement = currentSelectionIndex + 1;
				lastSelectedY = ystart + selectedElement*ystep;
				lastSelectedX = xstart;

				ngcPrintInstance->nGC_PrintText((char*)menuStrings[currentSelectionIndex].c_str(), lastSelectedX, lastSelectedY, Bold12);

				break;
			}
		}
	}

	selectedIndex = currentSelectionIndex;
	
	if (selectedIndex == menuList.size() - 1) //fertig selected
	{
		FlySwitchWindow();
		PrintResults();
	}
	else if (!exit) /*wanna input new values*/
	{
		requestInputBox();
		goto selectionLoopBegin;
	}
}

/*Using SDL Key events and ngc printings*/
std::string sdl_GetUserInput(int x, int y, int textClearBackgroundColor = 0xFFFFFF)
{
	ngcPrintInstance->SetTextClearBackgroundColor(textClearBackgroundColor);
	
	char inputTextArray[100] = "";
	SDL_EnableKeyRepeat(0, 0);

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
				bool spacePressed = isKeyPressed(KEY_NSPIRE_SPACE);
				bool enterPressed = isKeyPressed(KEY_NSPIRE_ENTER);
				bool backspacePressed = isKeyPressed(KEY_NSPIRE_DEL);
				if (enterPressed)
				{
					running = false;
					break;
				}
				else if (!backspacePressed)
				{
					const char* sdlKeyString = !spacePressed ? SDL_GetKeyName(event.key.keysym.sym) : " ";
					if (strlen(sdlKeyString) == 1)
					{
						strcat(inputTextArray, sdlKeyString);
						ngcPrintInstance->nGC_PrintText(inputTextArray, x, y);
					}
				}
				else if (backspacePressed)
				{
					std::string stdstr(inputTextArray);
					stdstr = stdstr.substr(0, stdstr.length() - 1);
					std::fill(inputTextArray, inputTextArray + sizeof(inputTextArray), NULL);
					sprintf(inputTextArray, stdstr.c_str());

					ngcPrintInstance->nGC_PrintText(inputTextArray, x, y);
				}
				break;
			}
		}
	}

	ngcPrintInstance->nGC_ClearText(x, y);
	return std::string(inputTextArray);
}

string getSelectionPrefix()
{
	switch (selectedIndex)
	{
	case 0: return "n=";
	case 1: return "k=";
	case 2: return "b=";
	case 3: return "s=";
	case 4: return "e=";
	case 5: return "p=";
	case 6: return "vec,";
	}
}

extern void DoInputAnimation();
extern void DoInputAnmiationEnd();

/*on menu selected item*/
void requestInputBox()
{
	DoInputAnimation();
	
	string userinput =  sdl_GetUserInput(SCREEN_WIDTH / 2 + 40, 20, 0x66B2FF);
	string prefix = getSelectionPrefix();
	
	userinput = prefix + userinput;
	totalUserInput += userinput + " ";
	
	DoInputAnmiationEnd();
}

void sdl_WaitUserKey(bool enterkey = false)
{
	while (any_key_pressed())
	{
		//wait
	}

	SDL_EnableKeyRepeat(0, 0);

	while (1)
	{
		if (enterkey ? isKeyPressed(KEY_NSPIRE_ENTER) : any_key_pressed()) {
			return;
		}
	}
}

void split(std::string str, std::string splitBy, std::vector<std::string>& tokens)
{
	tokens.push_back(str);
	size_t splitAt;
	size_t splitLen = splitBy.size();
	std::string frag;
	while (true)
	{
		frag = tokens.back();
		splitAt = frag.find(splitBy);
		if (splitAt == string::npos)
		{
			break;
		}
		tokens.back() = frag.substr(0, splitAt);
		tokens.push_back(frag.substr(splitAt + splitLen, frag.size() - (splitAt + splitLen)));
	}
}

enum CalculationType : int
{
	Erwartungswert,
	Varianz_Standartabweichung,

	Bernoulli,
	Binomialverteilung_Varianz_Erwartungswert,

	Intervall_Wahrscheinlichkeit
};

static string GetInputValues_lastInput;
static bool contains(const char* string)
{
	return GetInputValues_lastInput.find(string) != string::npos;
}

/*after pressing "fertig"*/
void sdlCalcuate(CalculationType& type_out, std::string& solvingWay_out, std::string input = totalUserInput)
{
	solvingWay_out = "";
	GetInputValues_lastInput = input;

	int vecOccurrences = std::count(input.begin(), input.end(), "vec");

	/*Erwartungswert for sure*/
	bool bestand = contains("b=");
	/*standartabweichung for sure*/
	bool standartabweichung = contains("vec,");

	bool bernoulli = contains("p=") && contains("k=") && contains("n="); //if over binomErwartungswert
	bool Binomial_Erwartungswert = contains("n=") && contains("p=");

	/*Intervall_Wahrscheinlichkeit for sure*/
	bool Intervall_Wahrscheinlichkeit = contains("s=") && contains("p=");

	vector<float> vecNums1, vecNums2;

	/*standartabweichung*/
	if (vecOccurrences == 1)
	{
		int vecpos1 = input.find("vec,") + 4;
		int vecendpos1 = input.find(" ", vecpos1) != string::npos ? input.find(" ", vecpos1) : input.length();

		string vecstr = input.substr(vecpos1, vecendpos1 - vecpos1);
		vector<string> results;

		split(vecstr, ",", results);

		for (std::vector<string>::iterator it = results.begin(); it != results.end(); ++it)
			vecNums1.push_back(atof((*it).c_str()));

		type_out = CalculationType::Varianz_Standartabweichung;

		float amount = results.size();
		float variance = 0;
		float expectedVal = 0;

#pragma regaion get expected val
		if (input.find("e=") == string::npos) /*no exact expectation value*/
		{
			for (std::vector<float>::iterator vecnum = vecNums1.begin(); vecnum != vecNums1.end(); ++vecnum)
			{
				expectedVal += *vecnum;
			}
			expectedVal /= amount;
		}
		else
		{
			int expectedValstartpos = input.find("e=") + 2;
			int expectedValendpos = input.find(" ", expectedValstartpos) != string::npos ? input.find(" ", expectedValstartpos) : input.length();
			expectedVal = atof(input.substr(expectedValstartpos, expectedValendpos - expectedValstartpos).c_str());
		}	
#pragma endregion

		char expectedValStr[10];
		ftoa(expectedVal, expectedValStr, 2);
		
		for (std::vector<float>::iterator vecnum = vecNums1.begin(); vecnum != vecNums1.end(); ++vecnum)
		{
			float delta = *vecnum - expectedVal;
			variance += pow(delta, 2) / amount;
			
			char vecnumStr[10];
			ftoa(*vecnum, vecnumStr, 1);

			solvingWay_out += "(" + string(vecnumStr) + " - " + string(expectedValStr) + ")^2" ;
			solvingWay_out += *vecnum == vecNums1[vecNums1.size() - 1] ? "" : " + ";
		}

		char varianceStr[10];
		ftoa(variance, varianceStr, 2);
		
		char stdDeviationVal[10];
		ftoa(sqrtf(variance), stdDeviationVal, 2);
		
		
		solvingWay_out += " = " + string(varianceStr) + " = Varianz!    Standardabweichung = " + string(stdDeviationVal) + "!   " +
			"Erwartungswert: " + string(expectedValStr) + "!";
		return;

	}
	/*Erwartungswert*/
	else if (vecOccurrences == 2)
	{
		int vecpos1 = input.find("b=vec,") + 6;
		int vecendpos1 = input.find(" ", vecpos1) != string::npos ? input.find(" ", vecpos1) : input.length();

		int vecpos2 = input.find("p=vec,", vecendpos1) + 6;
		int vecendpos2 = input.find(" ", vecpos2) != string::npos ? input.find(" ", vecpos2) : input.length();

		string vec1str = input.substr(vecpos1, vecendpos1 - vecpos1);
		string vec2str = input.substr(vecpos2, vecendpos2 - vecpos2);

		vector<string> results1;
		vector<string> results2;
		split(vec1str, ",", results1);
		split(vec2str, ",", results2);

		//int sPos = input.find("vec", vecpos + 3);
		for (std::vector<string>::iterator it = results1.begin(); it != results1.end(); ++it)
			vecNums1.push_back(atof((*it).c_str()));

		for (std::vector<string>::iterator it = results2.begin(); it != results2.end(); ++it)
			vecNums2.push_back(atof((*it).c_str()));

		float expectedVal = 0;
		
		for (int i = 0; i < vecNums1.size(); i++)
		{
			float currentB = vecNums1[i], currentP = vecNums2[i] > 1 ? vecNums2[i] / 100 : vecNums2[i];
			
			char currentBStr[10];
			ftoa(currentB, currentBStr, 3);
			char currentPStr[10];
			ftoa(currentP, currentPStr, 3);
			
			solvingWay_out += string(currentBStr) + " * " + string(currentPStr);
			solvingWay_out += i == vecNums1.size() - 1 ? "" : " + ";
			
			expectedVal += currentB*currentP;
		}
		
		char expectedValStr[10];
		ftoa(expectedVal, expectedValStr, 2);
		
		solvingWay_out += " = " + string(expectedValStr) + " = Erwartungswert an Gewinn" ;
		return;
	}
	else
	{

	}
}
