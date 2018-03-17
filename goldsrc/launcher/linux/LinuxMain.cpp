/// @file

#include <memory>

#include "LinuxApplication.hpp"

int main(int argc, char **argv)
{
	std::unique_ptr<IApplication> pApp = std::make_unique<CLinuxApplication>(argc, argv);
	
	return AppMain(pApp.get());
};