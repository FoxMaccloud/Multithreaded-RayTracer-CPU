#include <iostream>
#include "Gui.h"

int main()
{
	std::unique_ptr<Gui> gui = std::make_unique<Gui>();
	gui->run();

	return 0;
}