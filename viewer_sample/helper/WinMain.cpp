#ifdef _WIN32
#include <Windows.h>



extern int main(void);
HINSTANCE	g_hInstance;

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst,
    LPSTR lpszArgs, int nWinMode)
{


	g_hInstance = hThisInst;
	return main();

}


#endif
