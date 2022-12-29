
#include "InstrumentCallback.h"
#include "Bypass.h"
#include "main.h"

int main()
{
	if (!InitSymbol())
	{
		printf("InitSymbol Failed\n");
		return 0;
	}

	if (!InitInstrumentCallback())
	{
		printf("InitInstrumentCallback Failed\n");
		return 0;
	}	

	while (1)
	{
		SuperStealthyCheat();
	}

	return 0;
}

noinl void SuperStealthyCheat()
{
	//some hack written by different people :| 
	static BOOL NoobEnabled = false;
	static BOOL EasyEnabled = false;
	static BOOL SkilledEnabled = false;
	static BOOL ExpertEnabled = false;
	static BOOL EvilEnabled = false;

	if (GetKeyNoob(VK_HOME))
	{
		NoobEnabled = !NoobEnabled;
	}
	printf(NoobEnabled ? "[Noob] Enabled\n\n\n" : "[Noob] Disabled\n\n\n");

	if (GetKeyEasy(VK_HOME))
	{
		EasyEnabled = !EasyEnabled;
	}
	printf(EasyEnabled ? "[Easy] Enabled\n\n\n" : "[Easy] Disabled\n\n\n");

	if (GetKeySkilled(VK_HOME))
	{
		SkilledEnabled = !SkilledEnabled;
	}
	printf(SkilledEnabled ? "[Skilled] Enabled\n\n\n" : "[Skilled] Disabled\n\n\n");

	if (GetKeyExpert(VK_HOME))
	{
		ExpertEnabled = !ExpertEnabled;
	}
	printf(ExpertEnabled ? "[Expert] Enabled\n\n\n" : "[Expert] Disabled\n\n\n");

	if (GetKeyEvil(VK_HOME))
	{
		EvilEnabled = !EvilEnabled;
	}
	printf(EvilEnabled ? "[Evil] Enabled\n\n\n" : "[Evil] Disabled\n\n\n");


	getchar();//delete this if your are ready
	Sleep(1000);
	printf("\x1B[2J\x1B[H\n");
	//system("cls"); really big function especially when you run it on the first time

}