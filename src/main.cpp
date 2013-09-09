// memory leak detection
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif

#include "SoccerBot.h"

#include <iostream>

int main(int argc, char* argv[]) {
	#ifdef _DEBUG
		std::cout << "! Memory leak detection is enabled" << std::endl;
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	#endif

	// test memory leak
	char* leak = new char[1000];

	bool showGui = false;

	if (argc > 0) {
        std::cout << "! Parsing command line options" << std::endl;

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "gui") == 0) {
                showGui = true;

                std::cout << "  > Showing the GUI" << std::endl;
            } else {
                std::cout << "  > Unknown command line option: " << argv[i] << std::endl;

				return 1;
            }
        }
    }

	if (!showGui) {
        std::cout << "  > Start with 'gui' option to show GUI" << std::endl;
    }

	SoccerBot* soccerBot = new SoccerBot();

	soccerBot->showGui = showGui;

	soccerBot->setup();
	soccerBot->run();

	delete soccerBot;
	soccerBot = NULL;

	std::cout << "-- Properly Terminated --" << std::endl;

    return 0;
}
