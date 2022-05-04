#include <iostream>
#include "Gui.h"

int main()
{
	std::unique_ptr<Gui> gui = std::make_unique<Gui>();
	auto window = gui->get_window();
	gui->run();

	return 0;
}