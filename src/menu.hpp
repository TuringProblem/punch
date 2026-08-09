
#ifndef MENU_HPP
#define MENU_HPP

#include <iostream>

/**
 * @author: { @Override } : 20260808 11:47
 **/

/**
 *
 *
 *
 **/
enum class MenuState {

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

void screenHandler(const int &num) {
  switch (num) {
  case 1:
    lineCreator("Welcome to PUNCH menu!");
    break;
  case 2:
    lineCreator("This is the help Menu");
    break;
  default:
    break;
  }
}

#endif
