#ifndef CARDS_HPP
#define CARDS_HPP

#include <map>
/**
 * @author: { @Override } : 20260816 00:56
 * --------------------------------------
 *
 *
 * there are 52 cards in a deck - and using 4 decks per table
 *
 **/

enum class CardType {
  ACE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  TEN,
  JACK,
  QUEEN,
  KING,
};

enum class SuitType {
  CLUBS,
  DIAMONDS,
  HEARTS,
  SPADES,
};

const std::map<CardType, std::string> getCardMap(const SuitType &suit) const;

#endif
