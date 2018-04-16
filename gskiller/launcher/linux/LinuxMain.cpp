/// @file

#include <memory>

#include "LinuxApplication.hpp"

int main(int argc, char **argv)
{
	std::unique_ptr<CApplication> pApp = std::make_unique<CLinuxApplication>(argc, argv);
	
	return pApp->Run();
};