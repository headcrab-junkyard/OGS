/// @file

#include <cstdio>

#include "IApplication.hpp"

int AppMain(IApplication *apApp)
{
	bool bRestart{false};
	
	do
	{
		if(!apApp->Init())
			return EXIT_FAILURE;
		
		if(!apApp->Run())
			return EXIT_FAILURE;
		
		if(apApp->ShouldRestart())
			bRestart = true;
	}
	while(bRestart);
	
	return EXIT_SUCCESS;
};