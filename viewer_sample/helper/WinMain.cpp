#ifdef _WIN32
#include <Windows.h>



extern int main(void);

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst,
    LPSTR lpszArgs, int nWinMode)
{
	return main();

}


#endif
