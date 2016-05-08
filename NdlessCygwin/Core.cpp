#include "SDL_Funcs.h"
#include "Visuals.h"
#include "Textures/background.h"

ngc_Printing* ngcPrintInstance;

vector<string> menuList = { "Anzahl" , "Treffer" , "Gewinn/Verlust", "Standardabweichung", "Erwartungswert", 
"Wahrscheinlichkeit" , "Mehrere Werte" , "Fertig" };
		
string totalUserInput = "";
int selectedIndex = 0;
	
int main(void) {
	sdl_Init();
	InitOpenGl();
	ngcPrintInstance = new ngc_Printing();
	
	PrintMenu();
	sdl_InitSelection(20, 15, TEXTINPUTBOX_Height);
	
	UninitOpenGl();
	sdl_Finished();
	delete ngcPrintInstance;
	return 0;
}