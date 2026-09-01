#ifndef GAME_HPP
#define GAME_HPP

#include "player.hpp"
#include "rules.hpp"
#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <vector>

/**
 * @author: { @Override } : 20260831
 * -----
 *  Game is the table. It owns the players and the state that rotates between
 *  hands (the button, the hand counter). It knows nothing about cards yet.
 *
 *  No I/O in this header. Rules produce data, rendering reads it.
 **/

struct Game {
  std::vector<Player> players;
  int button = 0; // seat index of the dealer, rotates each hand
  int smallBlind = SMALL_BLIND;
  int bigBlind = BIG_BLIND;
  int handNumber = 0;

  int seatCount() const noexcept { return static_cast<int>(players.size()); }

  int playersRemaining() const noexcept {
    return static_cast<int>(
        std::count_if(players.begin(), players.end(),
                      [](const Player &p) { return !p.isKnockedOut; }));
  }

  bool isOver() const noexcept { return playersRemaining() <= 1; }
};

constexpr inline std::array<std::string_view, MAX_SEATS - 1> BOT_NAMES = {
    "Southpaw", "Ivan", "Mac", "Duke", "Rocco"};

inline int clampTableSize(int requested) noexcept {
  return std::clamp(requested, MIN_SEATS, MAX_SEATS);
}

inline Game createGame(const std::string &humanName, const int tableSize) {
  const int seats = clampTableSize(tableSize);

  Game game;
  game.players.reserve(static_cast<std::size_t>(seats));

  Player human;
  human.id = 0;
  human.name = humanName.empty() ? "Player" : humanName;
  human.isHuman = true;
  game.players.push_back(std::move(human));

  for (int i = 1; i < seats; ++i) {
    Player bot;
    bot.id = static_cast<PlayerId>(i);
    bot.name = std::string(BOT_NAMES[static_cast<std::size_t>(i - 1)]);
    game.players.push_back(std::move(bot));
  }

  return game;
}

#endif
