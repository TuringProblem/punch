#ifndef EQUITY_HPP
#define EQUITY_HPP

#include "deck.hpp"
#include "handrank.hpp"
#include <array>
#include <vector>

/**
 * @author: { @Override } : 20260831
 *
 * Monte Carlo equity. Fills in the cards nobody has seen yet, deals the
 * opponents random hands, and counts how often we win. Works on any street
 * because it just completes the board.
 *
 * This is what lets a bot know a flush draw on the flop is worth playing,
 * which raw hole card ranks could never tell it.
 **/

inline std::vector<Card> unseenCards(const std::array<Card, 2> &hole,
                                     const Card *board, int boardCount) {
  const auto taken = [&](const Card &c) {
    for (const Card &h : hole) {
      if (h.type == c.type && h.suit == c.suit)
        return true;
    }
    for (int i = 0; i < boardCount; ++i) {
      if (board[i].type == c.type && board[i].suit == c.suit)
        return true;
    }
    return false;
  };

  std::vector<Card> rest;
  rest.reserve(DECK_SIZE);
  for (int suit = 0; suit < SUIT_COUNT; ++suit) {
    for (int rank = 0; rank < RANK_COUNT; ++rank) {
      const Card card{static_cast<CardType>(rank), static_cast<SuitType>(suit)};
      if (!taken(card))
        rest.push_back(card);
    }
  }
  return rest;
}

inline double handEquity(const std::array<Card, 2> &hole, const Card *board,
                         int boardCount, int opponents, int trials) {
  if (opponents < 1)
    return 1.0;

  std::vector<Card> rest = unseenCards(hole, board, boardCount);
  const int missing = 5 - boardCount;
  const int needed = missing + opponents * 2;
  if (static_cast<int>(rest.size()) < needed)
    return 0.5;

  std::array<Card, 7> mine{};
  mine[0] = hole[0];
  mine[1] = hole[1];

  double score = 0.0;
  for (int t = 0; t < trials; ++t) {
    for (int i = 0; i < needed; ++i) {
      std::uniform_int_distribution<std::size_t> pick(
          static_cast<std::size_t>(i), rest.size() - 1);
      std::swap(rest[static_cast<std::size_t>(i)], rest[pick(rng())]);
    }

    std::array<Card, 5> full{};
    for (int i = 0; i < boardCount; ++i)
      full[static_cast<std::size_t>(i)] = board[i];
    for (int i = 0; i < missing; ++i) {
      full[static_cast<std::size_t>(boardCount + i)] =
          rest[static_cast<std::size_t>(i)];
    }
    for (int i = 0; i < 5; ++i)
      mine[static_cast<std::size_t>(2 + i)] = full[static_cast<std::size_t>(i)];

    const HandRank ours = evaluate7(mine);

    int better = 0, equal = 0;
    for (int o = 0; o < opponents; ++o) {
      std::array<Card, 7> theirs{};
      theirs[0] = rest[static_cast<std::size_t>(missing + o * 2)];
      theirs[1] = rest[static_cast<std::size_t>(missing + o * 2 + 1)];
      for (int i = 0; i < 5; ++i)
        theirs[static_cast<std::size_t>(2 + i)] =
            full[static_cast<std::size_t>(i)];

      const HandRank rank = evaluate7(theirs);
      if (rank > ours) {
        ++better;
        break;
      }
      if (rank == ours)
        ++equal;
    }

    if (better == 0)
      score += 1.0 / (1.0 + equal);
  }

  return score / trials;
}

#endif
