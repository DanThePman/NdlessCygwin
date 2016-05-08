#include <thread>

#define LED_MODE_ADDR	0x90110B00
#define RED 32
#define GREEN 16

class PressToTest
{
	static bool running;
	static int originalMode;

	static int getMem(int addr)
	{
		return *(volatile unsigned*)addr;
	}

	static void setMem(int addr, int val)
	{
		*(volatile unsigned*)addr = val;
	}

	static void Init()
	{
		originalMode = getMem(LED_MODE_ADDR);
		running = false;
	}
public:

	static void Stop()
	{
		running = false;
	}

	static void Simulate()
	{
		Init();

		running = true;

		int i = 1;
		while (running)
		{
			bool green = i % 2 == 0;

			setMem(LED_MODE_ADDR, green ? GREEN : RED);
			i++;

			if (i > 1000)
				i = 1;
			sleep(1000);
		}
	}
};



