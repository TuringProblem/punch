#ifndef CARDPAIR_HPP
#define CARDPAIR_HPP
#include "cards.hpp"
#include <string>

/**
 * @author: { @Override } : 20260816 00:56
 * --------------------------------------
 *
 *
 * there are 52 cards in a deck - and using 4 decks per table
 *
 **/

template <typename T, typename U> struct CardPair {
  T first;
  U second;
  getFirst(const CardPair<T, U> &pair) const { return pair.first; }
  getSecond(const CardPair<T, U> &pair) const { return pair.second; }
  getCardMap(const SuitType &suit) const;
};

#endif
