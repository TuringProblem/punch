#ifndef MENU_HPP
#define MENU_HPP
#endif

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

}

void displayMainScreen(const int &num) {
  const auto lineCreator = [&](const std::string &word) {
    size_t size = word.size();
    for (size_t i = 0; i < size; i++) {
      std::cout << "-";
    }

    std::cout << "\n" << word << "\n";

    for (size_t i = 0; i < size; i++) {
      std::cout << "-";
    }
  };

  lineCreator("Welcome to PUNCH menu!");
}
