#ifndef DECK_HPP
#define DECK_HPP

#include "cards.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

/**
 * @author: { @Override } : 20260831
 *
 * One real 52 card deck. Dealing walks a cursor forward instead of picking
 * at random, so the same card can never come out twice in a hand.
 *
 * Hold'em needs exactly one deck. A multi deck shoe would allow five of a
 * kind and break every hand ranking.
 **/

inline constexpr std::size_t DECK_SIZE = 52;
inline constexpr int SUIT_COUNT = 4;
inline constexpr int RANK_COUNT = 13;

struct Deck {
  std::array<Card, DECK_SIZE> cards{};
  std::size_t next = 0;
};

inline Deck makeDeck() noexcept {
  Deck deck;
  std::size_t i = 0;
  for (int suit = 0; suit < SUIT_COUNT; ++suit) {
    for (int rank = 0; rank < RANK_COUNT; ++rank) {
      deck.cards[i++] =
          Card{static_cast<CardType>(rank), static_cast<SuitType>(suit)};
    }
  }
  return deck;
}

inline std::size_t cardsLeft(const Deck &deck) noexcept {
  return DECK_SIZE - deck.next;
}

inline void shuffleDeck(Deck &deck) {
  std::shuffle(deck.cards.begin(), deck.cards.end(), rng());
  deck.next = 0;
}

inline Card dealCard(Deck &deck) {
  assert(deck.next < DECK_SIZE && "dealt past the end of the deck");
  return deck.cards[deck.next++];
}

inline void burnCard(Deck &deck) {
  assert(deck.next < DECK_SIZE && "burned past the end of the deck");
  ++deck.next;
}

#endif
