#include "SoccerBot.h"
#include <iostream>

int main(int argc, char *argv[]) {
  SoccerBot *soccerBot = new SoccerBot();
  soccerBot->setup();
  soccerBot->start();
  soccerBot->join();
  delete soccerBot;
  soccerBot = NULL;

  std::cout << "-- Properly Terminated --" << std::endl << std::endl;

  return 0;
}
