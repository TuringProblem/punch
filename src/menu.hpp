
#ifndef MENU_HPP
#define MENU_HPP

#include <chrono>
#include <iostream>
#include <thread>

/**
 * @author: { @Override } : 20260808 11:47
 **/

/**
 *
 **/
enum class MenuState {
  START,
  HELP,
  VERSION,
};

const auto lineCreator = [](const std::string &word) {
  size_t size = word.size();
  for (size_t i = 0; i < size; i++) {
    std::cout << "-";
  }

  std::cout << "\n" << word << "\n";

  for (size_t i = 0; i < size; i++) {
    std::cout << "-";
  }
  std::cout << "\n";
};

const auto clearScreen = []() { std::cout << "\033[2J\033[1;1H"; };

void runStart(const std::string &devtag) {
  std::cout << "\n\n" << devtag << "\n\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  clearScreen();
  // now since this game is a poker game - we need to figure out what type of
  // game the user wants to play
  std::cout << "Please Enter Org: ";
  std::string orgName;
  std::getline(std::cin, orgName);

  if (std::cin.fail()) {
    std::cout << "\n\nError: Invalid Org Name\n\n";
    return;
  }
}

inline void screenHandler(const MenuState &menu) {
  switch (menu) {
  case MenuState::START:
    lineCreator("Welcome to PUNCH menu!");
    runStart("Developed by TuringProblem");
    break;
  case MenuState::HELP:
    lineCreator("This is the help Menu");
    break;
  case MenuState::VERSION:
    lineCreator("Punch 0.1");
    break;
  default:
    break;
  }
}

#endif
