static const unsigned goto0_addrs[] = { 0x0, 0x0, 0x0, 0x1006C158 };		// ex_LUA_CALCALL1clon
#define goto0 SYSCALL_CUSTOM(goto0_addrs, int, int, int, char*, int, int, int, int)
static const unsigned goto1_addrs[] = { 0x0, 0x0, 0x0, 0x1006C210 };		// ex_LUA_CALCALL1
#define goto1 SYSCALL_CUSTOM(goto1_addrs, int, int, int, char*, int, int, int, int)
																			// TAG2UTF16( *string tag, * où sera retourné un * sur le string utf16)
static const unsigned goto2_addrs[] = { 0x0, 0x0, 0x0, 0x1006BFE0 };
#define goto2 SYSCALL_CUSTOM(goto2_addrs, void, char*, int, int, int)	// ex_TAG2UTF16_2

static const unsigned goto3_addrs[] = { 0x0, 0x0, 0x0, 0x10230BFC }; // refresh_docbrowser
#define goto3 SYSCALL_CUSTOM(goto3_addrs, void, int)


int calculate(const char * calc, char ** p_result, int mode) {

	puts("0OK");
	char calc16[2 * (strlen(calc) + 1)];
	ascii2utf16(calc16, calc, sizeof(calc16));

	if (mode == 0 && (goto0(0x0, 0x0, calc16, 0x10000000, 0, 0, 0) == 0))
	{
		puts("1OK-C");
	}
	else if (mode == 1 && (goto1(0x0, 0x0, calc16, 0x10000000, 0, 0, 0) == 0))
	{
		puts("1OK-C");
	}
	else
	{
		puts("1FAIL");
		*p_result = reinterpret_cast<char*>(malloc(0x10));
		strcpy(*p_result, "Syntax error");
		return 1;
	}

	char *tag = *((char**)0x10000000);
	goto2(tag, 0x10000004, 0x0, 0x0);

	puts("2OK");

	unsigned isresult = *((unsigned*)0x10000004);
	if (isresult != 0x0)
	{
		char *solve16 = *((char**)0x10000004);
		unsigned const buffSize = utf16_strlen(reinterpret_cast<const uint16_t*>(solve16)) + 1;
		char solve[buffSize];
		utf162ascii(solve, reinterpret_cast<const uint16_t*>(solve16), sizeof(solve) * 2);
		*p_result = reinterpret_cast<char*>(malloc(utf16_strlen(reinterpret_cast<const uint16_t*>(solve16)) + 1));
		strcpy(*p_result, solve);
	}
	else
	{
		*p_result = reinterpret_cast<char*>(malloc(0x10));
		strcpy(*p_result, "Done");
	}
	puts("3OK");

	return 0;
}