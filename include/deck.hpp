#ifndef DECK_HPP
#define DECK_HPP

#include "cards.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

/**
 * @author: { @Override } : 20260831
 * -----
 *  One real 52 card deck. Dealing walks a cursor forward instead of picking
 *  at random, so the same card can never come out twice in a hand.
 *
 *  Texas Hold'em needs exactly one deck. A multi deck shoe would allow five
 *  of a kind and break every hand ranking.
 **/

class Deck {
public:
  static constexpr std::size_t DECK_SIZE = 52;
  static constexpr int SUIT_COUNT = 4;
  static constexpr int RANK_COUNT = 13;

  Deck() { reset(); }

  void reset() noexcept {
    std::size_t i = 0;
    for (int suit = 0; suit < SUIT_COUNT; ++suit) {
      for (int rank = 0; rank < RANK_COUNT; ++rank) {
        cards_[i++] =
            Card{static_cast<CardType>(rank), static_cast<SuitType>(suit)};
      }
    }
    next_ = 0;
  }

  void shuffle() {
    std::shuffle(cards_.begin(), cards_.end(), rng());
    next_ = 0;
  }

  Card deal() {
    assert(next_ < DECK_SIZE && "dealt past the end of the deck");
    return cards_[next_++];
  }

  void burn() {
    assert(next_ < DECK_SIZE && "burned past the end of the deck");
    ++next_;
  }

  std::size_t remaining() const noexcept { return DECK_SIZE - next_; }

private:
  std::array<Card, DECK_SIZE> cards_{};
  std::size_t next_ = 0;
};

#endif
