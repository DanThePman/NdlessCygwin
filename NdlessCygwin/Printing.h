#include <ngc.h>

#define TEXTINPUTBOX_X 50
#define TEXTINPUTBOX_Y 50
#define TEXTINPUTBOX_Width SCREEN_WIDTH - TEXTINPUTBOX_X
#define TEXTINPUTBOX_Height 20

using namespace std;

class ngc_Printing
{
private:
	Gc gc;
	int textbackgroundClearColor = 0xFFFFFF;
public:
	ngc_Printing() {
		gc = gui_gc_global_GC();
		gui_gc_setRegion(gc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		gui_gc_begin(gc);

		gui_gc_setColorRGB(gc, 255, 255, 255);
		gui_gc_fillRect(gc, 0, 0, 320, 240);
		gui_gc_blit_to_screen(gc);

		gui_gc_finish(gc);
	}

	Gc GetGraphicDevice() {
		return gc;
	}

	/*background color when text gets cleared*/
	void SetTextClearBackgroundColor(int color)
	{
		textbackgroundClearColor = color;
	}
	
	/*DONT DO THAT*/
	void FreeGraphicDevice()
	{
		gui_gc_free(gc);
	}

	void nGC_ClearText(int x, int y)
	{
		int w = TEXTINPUTBOX_Width - x, h = TEXTINPUTBOX_Height;
		gui_gc_begin(gc);

		gui_gc_setColor(gc, textbackgroundClearColor);
		gui_gc_fillRect(gc, x - 5, y, w, h + 5);
		gui_gc_blit_to_screen(gc);

		gui_gc_finish(gc);
	}

	/*Printing Text via nGC (Fancy method)*/
	void nGC_PrintText(char* text, int x = TEXTINPUTBOX_X, int y = TEXTINPUTBOX_Y, gui_gc_Font font = SerifRegular12)
	{
		int textlen = strlen(text);
		
		nGC_ClearText(x, y);

		if (textlen < 35)
		{
			gui_gc_begin(gc);
			gui_gc_setColor(gc, 0x000000);
			gui_gc_setFont(gc, font);
			
			char buffer[3*strlen(text) - 2];
			ascii2utf16(buffer, text, sizeof(buffer));
			gui_gc_drawString(gc, buffer, x, y, GC_SM_TOP);
			
			gui_gc_blit_to_screen(gc);
			gui_gc_finish(gc);
		}
		else
		{
			int starty = y;
			for (int i = 35; i < textlen; i += 35)
			{
				bool lastrun = textlen - i <= 35;
				
				string std_str(text);
				std_str = std_str.substr(i - 35, 35);
				char buffer[3*std_str.size() - 2];
				ascii2utf16(buffer, std_str.c_str(), sizeof(buffer));
				
#pragma region drawString
				gui_gc_begin(gc);
				gui_gc_setColor(gc, 0x000000);
				gui_gc_setFont(gc, font);
				
				gui_gc_drawString(gc,buffer,x, starty,GC_SM_TOP);
			
				gui_gc_blit_to_screen(gc);
				gui_gc_finish(gc);
#pragma endregion
				if (lastrun)
				{
					string std_str2(text);
					std_str2 = std_str2.substr(i, textlen - i);
					char buffer2[3*std_str2.size() - 2];
					ascii2utf16(buffer2, std_str2.c_str(), sizeof(buffer2));
					
					gui_gc_begin(gc);
					gui_gc_setColor(gc, 0x000000);
					gui_gc_setFont(gc, font);
				
					gui_gc_drawString(gc, buffer2, x, starty + TEXTINPUTBOX_Height, GC_SM_TOP);
			
					gui_gc_blit_to_screen(gc);
					gui_gc_finish(gc);
				}
				
				
				starty += TEXTINPUTBOX_Height;
			}
		}
	}
};