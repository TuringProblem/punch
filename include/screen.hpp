#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <chrono>
#include <iostream>
#include <thread>

/**
 * @author: { @Override } : 20260808 11:47
 **/

/**
 * What will a Screen have?
 *
 **/

const auto clearScreen = []() { std::cout << "\033[2J\033[1;1H"; };

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
  int choice;
  std::cin >> choice;
  if (std::cin.fail()) {
    std::cout << "\n\nError: Invalid Choice\n\n";
    return;
  }
  switch (choice) {
  case 1:
    std::cout << "Playing...\n";
    break;
  case 2:
    std::cout << "Options...\n";
    break;
  case 3:
    std::cout << "Goodbye!\n";
    exit(0);
    break;
  default:
    break;
  }
}

#endif
