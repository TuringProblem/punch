#include "menu.hpp"
#include <iostream>

/**
 * @author: { @Override } : 20260808 11:47
 *
 * -----
 *  PUNCH is a cli driven application for going to your tickets/and repos
 * associated with you ticket.
 **/

int main(int argc, char **argv) {
  auto startCaller = [&](const std::string &message) {
    if (message == "start") {
      std::cout << "You've been punched by the falcon!" << std::endl;
      std::cout << "Designed by TuringProblem\n\n" << std::endl;
      displayMainScreen(1);
    }
  };

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " message" << std::endl;
    return 1;
  }

  startCaller(argv[1]);
}
