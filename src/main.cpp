#include <iostream>

#include "menu.hpp"

#include <map>

/**
 * @author: { @Override } : 20260808 11:47
 *
 * -----
 *  PUNCH is a cli driven application for going to your tickets/and repos
 * associated with you ticket.
 **/

// mmmm... what do I want to map...  the command "--start"
int main(int argc, char **argv) {

  std::map<std::string, void (*)()> commands = {
      {"--start", []() { screenHandler(MenuState::START); }},
      {"--help", []() { screenHandler(MenuState::HELP); }},
      {"--version", []() { screenHandler(MenuState::VERSION); }},
  };

  auto startCaller = [&](const std::string &message) {
    auto it = commands.find(message);
    if (it != commands.end()) {
      it->second();
    }
  };

  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " message" << std::endl;
    return 1;
  }

  startCaller(argv[1]);
}
