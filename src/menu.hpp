
#ifndef MENU_HPP
#define MENU_HPP

#include "cards.hpp"
#include "engine.hpp"
#include "game.hpp"
#include "hand.hpp"
#include "player.hpp"
#include "showdown.hpp"
#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <limits>
#include <thread>

/**
 * @author: { @Override } : 20260808 11:47
 *
 * some design choices for now, for the generating cards we are using a
 * string_view over a string this is because a string_view is a lightweight,
 * read only object where as the string "owns" the char data
 **/

enum class MenuState { START, HELP, RANDOM, VERSION };

const auto clearScreen = []() { std::cout << "\033[2J\033[1;1H"; };

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

const auto displayModes = [](const std::string &userName) {
  clearScreen();

  std::string values =
      std::format("Please select the following *{}*: ", userName);
  lineCreator(values);
  std::vector<std::string> options = {"Play", "Options", "Quit"};
  listOptions(options);
};

inline int promptTableSize() {
  int size = 0;
  while (true) {
    std::cout << "\nHow many at the table? (" << MIN_SEATS << "-" << MAX_SEATS
              << "): ";
    if (std::cin >> size && size >= MIN_SEATS && size <= MAX_SEATS) {
      return size;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Pick a number between " << MIN_SEATS << " and " << MAX_SEATS
              << ".\n";
  }
}

inline void renderRoster(const Game &game) {
  clearScreen();
  lineCreator(std::format("Punch pot opens at {} HP - blinds {}/{}", BIG_BLIND,
                          game.smallBlind, game.bigBlind));

  std::cout << "\n";
  for (const auto &player : game.players) {
    std::cout << std::format("  {:<10} {:>3} HP   {} blocks{}\n", player.name,
                             player.health, player.blocks,
                             player.isHuman ? "   <- you" : "");
  }
  std::cout << std::format("\n  {} seated. Dealer button on {}.\n",
                           game.seatCount(), game.players[game.button].name);
}

inline std::string cardsOf(const Card *cards, int count) {
  std::string out;
  for (int i = 0; i < count; ++i) {
    out += getCardDesign(cards[i]);
    out += " ";
  }
  return out;
}

// Human at the keyboard. Prints its own prompt and reads a choice.
struct HumanAgent : Agent {
  Action act(const HandState &hand, const Game &game, int seatIndex) override {
    const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
    const Player &me = game.players[static_cast<std::size_t>(seat.id)];
    const int owed = toCall(hand, seatIndex);

    std::cout << std::format("\n  Your hand: {}   {} HP, {} blocks\n",
                             cardsOf(seat.hole.data(), 2), me.health,
                             me.blocks);
    std::cout << std::format("  Punch pot: {} HP   committed: {} HP\n",
                             hand.punchPot(), seat.committed);

    if (owed > 0) {
      std::cout << std::format("  [f]old  [c]all {} HP", owed);
    } else {
      std::cout << "  [f]old  [c]heck";
    }
    if (canRaise(hand, game, seatIndex)) {
      std::cout << std::format("  [r]aise (min {} HP, max {} HP)",
                               minRaiseTo(hand, game, seatIndex),
                               maxRaiseTo(hand, game, seatIndex));
    }
    std::cout << "\n  > ";

    std::string choice;
    if (!(std::cin >> choice))
      return Action{ActionType::FOLD, 0};

    if (choice == "f")
      return Action{ActionType::FOLD, 0};
    if (choice == "r" && canRaise(hand, game, seatIndex)) {
      std::cout << "  raise to how much HP? > ";
      int amount = 0;
      if (!(std::cin >> amount)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return Action{ActionType::CALL, 0};
      }
      return Action{ActionType::RAISE, amount};
    }
    return owed > 0 ? Action{ActionType::CALL, 0}
                    : Action{ActionType::CHECK, 0};
  }
};

inline std::string_view actionName(const Action &action) {
  switch (action.type) {
  case ActionType::FOLD:
    return "folds";
  case ActionType::CHECK:
    return "checks";
  case ActionType::CALL:
    return "calls";
  case ActionType::RAISE:
    return "raises";
  }
  return "";
}

inline Observer makeObserver() {
  Observer observer;

  observer.onStreet = [](const HandState &hand, const Game &game) {
    std::cout << std::format("\n===== {} =====   punch pot: {} HP\n",
                             streetName(hand.street), hand.punchPot());
    if (hand.boardCount > 0) {
      std::cout << "  Board: " << cardsOf(hand.board.data(), hand.boardCount)
                << "\n";
    }
  };

  observer.onAction = [](const HandState &hand, const Game &game, int seatIndex,
                         const Action &action) {
    const Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
    const Player &player = game.players[static_cast<std::size_t>(seat.id)];
    std::cout << std::format("  {:<10} {}", player.name, actionName(action));
    if (action.type == ActionType::CALL || action.type == ActionType::RAISE) {
      std::cout << std::format(" to {} HP", seat.streetCommitted);
    }
    if (seat.status == SeatStatus::ALL_IN)
      std::cout << "  (ALL IN)";
    std::cout << "\n";
  };

  return observer;
}

inline void playOneHand(const Game &game) {
  AgentTable agents;
  for (const Player &player : game.players) {
    if (player.isHuman) {
      agents.push_back(std::make_unique<HumanAgent>());
    } else {
      agents.push_back(std::make_unique<SimpleBot>());
    }
  }

  HandState hand = startHand(game);
  const Observer observer = makeObserver();
  playHand(hand, game, agents, observer);

  const Showdown result = resolveShowdown(hand);
  const auto nameOf = [&](int seatIndex) -> const std::string & {
    return game
        .players[static_cast<std::size_t>(
            hand.seats[static_cast<std::size_t>(seatIndex)].id)]
        .name;
  };

  if (!result.contested) {
    std::cout << std::format(
        "\n  {} takes the {} HP punch pot, everyone else folded.\n",
        nameOf(result.winners.front()), hand.punchPot());
  } else {
    std::cout << "\n  -- cards on their back --\n";
    for (int i = 0; i < hand.seatCount(); ++i) {
      const Seat &seat = hand.seats[static_cast<std::size_t>(i)];
      if (!seat.inHand())
        continue;
      const HandRank &rank = result.best[static_cast<std::size_t>(i)];
      std::cout << std::format("  {:<10} {}  {}\n", nameOf(i),
                               cardsOf(seat.hole.data(), 2),
                               categoryName(rank.cat));
    }

    for (std::size_t r = 0; r < result.tiebreak.size(); ++r) {
      const TieRound &tie = result.tiebreak[r];
      std::cout << std::format("\n  -- tied, redraw {} --\n", r + 1);
      for (std::size_t i = 0; i < tie.seats.size(); ++i) {
        std::cout << std::format("  {:<10} {}  {}\n", nameOf(tie.seats[i]),
                                 cardsOf(tie.hole[i].data(), 2),
                                 categoryName(tie.ranks[i].cat));
      }
    }

    if (result.unresolved) {
      std::cout << "\n  Board can't be beaten, settled by seat position.\n";
    }

    std::cout << "\n  ";
    if (result.winners.size() == 1) {
      const int champ = result.winners.front();
      Category won = result.best[static_cast<std::size_t>(champ)].cat;
      if (!result.tiebreak.empty()) {
        const TieRound &last = result.tiebreak.back();
        for (std::size_t i = 0; i < last.seats.size(); ++i) {
          if (last.seats[i] == champ)
            won = last.ranks[i].cat;
        }
      }
      std::cout << std::format("{} wins the {} HP punch pot with {}.\n",
                               nameOf(champ), hand.punchPot(),
                               categoryName(won));
    } else {
      std::cout << "Tie between ";
      for (std::size_t i = 0; i < result.winners.size(); ++i) {
        std::cout << nameOf(result.winners[i])
                  << (i + 1 < result.winners.size() ? ", " : "");
      }
      std::cout << std::format(" for the {} HP punch pot.\n", hand.punchPot());
    }
  }

  std::cout << "\n  -- damage if punched --\n";
  for (int i = 0; i < hand.seatCount(); ++i) {
    const Seat &seat = hand.seats[static_cast<std::size_t>(i)];
    const bool isWinner =
        std::find(result.winners.begin(), result.winners.end(), i) !=
        result.winners.end();
    std::cout << std::format("  {:<10} {} HP{}\n", nameOf(i),
                             std::max(MIN_PUNCH, seat.committed),
                             isWinner ? "   (winner, throws the punch)"
                             : seat.hasFolded() ? "   (folded)"
                                                : "");
  }
  std::cout << "\n  [punch phase not wired yet]\n";
}

inline void runStart(const std::string &devtag) {
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
  case 1: {
    const Game game = createGame(userName, promptTableSize());
    renderRoster(game);
    playOneHand(game);
    break;
  }
  case 2:
    std::cout << "Options...\n";
    break;
  case 3:
    std::cout << "Goodbye!\n";
    return;
  default:
    std::cout << "\n\nError: Invalid Choice\n\n";
    break;
  }
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
  case MenuState::RANDOM: {
    const auto randCard = getRandomPair();
    std::cout << randCard[0] << " " << randCard[1] << std::endl;
    break;
  }
  default:
    break;
  }
}

#endif
