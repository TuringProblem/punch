
#ifndef MENU_HPP
#define MENU_HPP

#include <chrono>
#include <format>
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

const auto listOptions = [](const std::vector<std::string> &options) {
  int counter = 0;
  for (const auto &option : options) {
    std::cout << ++counter << ". " << option << "\n";
  }
};
const auto clearScreen = []() { std::cout << "\033[2J\033[1;1H"; };
const auto displayModes = [](const std::string &userName) {
  clearScreen();

  std::string values =
      std::format("Please select the following *{}*: ", userName);
  lineCreator(values);
  std::vector<std::string> options = {"Play", "Options", "Quit"};
  listOptions(options);
};

void runStart(const std::string &devtag) {
  std::cout << "\n\n" << devtag << "\n\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  clearScreen();
  std::cout << "Please enter your name: ";
  std::string userName;
  std::getline(std::cin, userName);
  if (std::cin.fail()) {
    std::cout << "\n\nError: Invalid Org Name\n\n";
    return;
  }

  displayModes(userName);
}

inline void displayGame() {
  lineCreator("Please Select the following: ");
  std::cout << "\n";
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
