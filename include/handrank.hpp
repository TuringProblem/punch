#ifndef HANDRANK_HPP
#define HANDRANK_HPP

#include "cards.hpp"
#include <array>
#include <cstdint>
#include <string_view>

/**
 * @author: { @Override } : 20260831
 * -----
 *  Best five card hand out of seven (two hole cards plus the five board
 *  cards). Standard Texas Hold'em rankings.
 *
 *  CardType stores ACE first so it prints in a natural order, but at
 *  showdown an ace is high (14). It also plays low in the wheel,
 *  A-2-3-4-5, which straightHigh handles with an extra low bit.
 **/

enum class Category {
  HIGH_CARD,
  PAIR,
  TWO_PAIR,
  TRIPS,
  STRAIGHT,
  FLUSH,
  FULL_HOUSE,
  QUADS,
  STRAIGHT_FLUSH,
};

constexpr std::string_view categoryName(Category cat) noexcept {
  switch (cat) {
  case Category::HIGH_CARD:
    return "High Card";
  case Category::PAIR:
    return "Pair";
  case Category::TWO_PAIR:
    return "Two Pair";
  case Category::TRIPS:
    return "Three of a Kind";
  case Category::STRAIGHT:
    return "Straight";
  case Category::FLUSH:
    return "Flush";
  case Category::FULL_HOUSE:
    return "Full House";
  case Category::QUADS:
    return "Four of a Kind";
  case Category::STRAIGHT_FLUSH:
    return "Straight Flush";
  }
  return "";
}

struct HandRank {
  Category cat = Category::HIGH_CARD;
  std::array<int, 5> kickers{}; // descending tiebreakers

  // Compares category first, then kickers left to right. Exactly poker
  // ordering, for free.
  auto operator<=>(const HandRank &) const = default;
  bool operator==(const HandRank &) const = default;
};

// Ace is 14 at showdown, king 13, down to two.
constexpr int highValue(CardType type) noexcept {
  return type == CardType::ACE ? 14 : static_cast<int>(type) + 1;
}

// Top card of the best straight in this rank mask, or 0 for none.
constexpr int straightHigh(std::uint16_t mask) noexcept {
  if (mask & (1u << 14))
    mask |= 1u << 1; // ace plays low for the wheel
  for (int high = 14; high >= 5; --high) {
    const auto need = static_cast<std::uint16_t>(0x1Fu << (high - 4));
    if ((mask & need) == need)
      return high;
  }
  return 0;
}

inline HandRank evaluate7(const std::array<Card, 7> &cards) {
  int rankCount[15] = {};
  int suitCount[4] = {};
  std::uint16_t mask = 0;
  std::uint16_t suitMask[4] = {};

  for (const Card &card : cards) {
    const int value = highValue(card.type);
    const auto suit = static_cast<std::size_t>(card.suit);
    ++rankCount[value];
    ++suitCount[suit];
    mask |= static_cast<std::uint16_t>(1u << value);
    suitMask[suit] |= static_cast<std::uint16_t>(1u << value);
  }

  // Highest `want` distinct ranks, skipping anything already used.
  const auto fill = [&](std::array<int, 5> &out, int at, std::uint16_t from,
                        int skipA, int skipB, int want) {
    for (int value = 14; value >= 2 && want > 0; --value) {
      if (value == skipA || value == skipB)
        continue;
      if (from & (1u << value)) {
        out[static_cast<std::size_t>(at++)] = value;
        --want;
      }
    }
  };

  int flushSuit = -1;
  for (int suit = 0; suit < 4; ++suit) {
    if (suitCount[suit] >= 5)
      flushSuit = suit;
  }

  HandRank best;

  if (flushSuit >= 0) {
    const std::uint16_t fm = suitMask[static_cast<std::size_t>(flushSuit)];
    if (const int high = straightHigh(fm); high > 0) {
      best.cat = Category::STRAIGHT_FLUSH;
      best.kickers[0] = high;
      return best;
    }
  }

  int quad = 0, tripA = 0, tripB = 0, pairA = 0, pairB = 0;
  for (int value = 14; value >= 2; --value) {
    switch (rankCount[value]) {
    case 4:
      if (!quad)
        quad = value;
      break;
    case 3:
      (tripA ? tripB : tripA) = value;
      break;
    case 2:
      (pairA ? pairB : pairA) = value;
      break;
    default:
      break;
    }
  }

  if (quad) {
    best.cat = Category::QUADS;
    best.kickers[0] = quad;
    fill(best.kickers, 1, mask, quad, 0, 1);
    return best;
  }

  // A second trips set plays as the pair half of a full house.
  if (tripA && (tripB || pairA)) {
    best.cat = Category::FULL_HOUSE;
    best.kickers[0] = tripA;
    best.kickers[1] = tripB > pairA ? tripB : pairA;
    return best;
  }

  if (flushSuit >= 0) {
    best.cat = Category::FLUSH;
    fill(best.kickers, 0, suitMask[static_cast<std::size_t>(flushSuit)], 0, 0,
         5);
    return best;
  }

  if (const int high = straightHigh(mask); high > 0) {
    best.cat = Category::STRAIGHT;
    best.kickers[0] = high;
    return best;
  }

  if (tripA) {
    best.cat = Category::TRIPS;
    best.kickers[0] = tripA;
    fill(best.kickers, 1, mask, tripA, 0, 2);
    return best;
  }

  if (pairA && pairB) {
    best.cat = Category::TWO_PAIR;
    best.kickers[0] = pairA;
    best.kickers[1] = pairB;
    fill(best.kickers, 2, mask, pairA, pairB, 1);
    return best;
  }

  if (pairA) {
    best.cat = Category::PAIR;
    best.kickers[0] = pairA;
    fill(best.kickers, 1, mask, pairA, 0, 3);
    return best;
  }

  best.cat = Category::HIGH_CARD;
  fill(best.kickers, 0, mask, 0, 0, 5);
  return best;
}

#endif
