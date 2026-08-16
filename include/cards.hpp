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

const std::string getCardValue(const SuitType &suit,
                               const CardType &card) const {
  switch (suit) {
  case SuitType::SPADES:
    switch (card) {
        case CardType::ACE;
          return "🂡"; break;
        case CardType::TWO;
          return "🂢"; break;
        case CardType::THREE;
          return "🂣"; break;
        case CardType::FOUR;
          return "🂤"; break;
        case CardType::FIVE;
          return "🂥"; break;
        case CardType::SIX;
          return "🂦"; break;
        case CardType::SEVEN;
          return "🂧"; break;
        case CardType::EIGHT;
          return "🂨"; break;
        case CardType::NINE;
          return "🂩"; break;
        case CardType::TEN;
          return "🂪"; break;
        case CardType::JACK;
          return "🂫"; break;
        case CardType::QUEEN;
          return "🂭"; break;
        case CardType::KING;
          return "🂮"; break;
    }
    break;
    case SuitType::DIAMONDS:
          switch (card) {
        case CardType::ACE;
          return "🃁"; break;
        case CardType::TWO;
          return "🃂"; break;
        case CardType::THREE;
          return "🃃"; break;
        case CardType::FOUR;
          return "🃄"; break;
        case CardType::FIVE;
          return "🃅"; break;
        case CardType::SIX;
          return "🃆"; break;
        case CardType::SEVEN;
          return "🃇"; break;
        case CardType::EIGHT;
          return "🃈"; break;
        case CardType::NINE;
          return "🃉"; break;
        case CardType::TEN;
          return "🃊"; break;
        case CardType::JACK;
          return "🃋"; break;
        case CardType::QUEEN;
          return "🃌"; break;
        case CardType::KING;
          return "🃍"; break;
    }
    break;
    case SuitType::CLUBS:
          switch (card) {
        case CardType::ACE;
          return "🃑"; break;
        case CardType::TWO;
          return "🃒"; break;
        case CardType::THREE;
          return "🃓"; break;
        case CardType::FOUR;
          return "🃔"; break;
        case CardType::FIVE;
          return "🃕"; break;
        case CardType::SIX;
          return "🃖"; break;
        case CardType::SEVEN;
          return "🃗"; break;
        case CardType::EIGHT;
          return "🃘"; break;
        case CardType::NINE;
          return "🃙"; break;
        case CardType::TEN;
          return "🃚"; break;
        case CardType::JACK;
          return "🃛"; break;
        case CardType::QUEEN;
          return "🃝"; break;
        case CardType::KING;
          return "🃜"; break;  
    }
    break;
    case SuitType::HEARTS:
          switch (card) {
        case CardType::ACE;
          return "🂡"; break;
        case CardType::TWO;
          return "🂢"; break;
        case CardType::THREE;
          return "🂣"; break;
        case CardType::FOUR;
          return "🂤"; break;
        case CardType::FIVE;
          return "🂥"; break;
        case CardType::SIX;
          return "🂦"; break;
        case CardType::SEVEN;
          return "🂧"; break;
        case CardType::EIGHT;
          return "🂨"; break;
        case CardType::NINE;
          return "🂩"; break;
        case CardType::TEN;
          return "🂪"; break;
        case CardType::JACK;
          return "🂫"; break;
        case CardType::QUEEN;
          return "🂭"; break;
        case CardType::KING;
          return "🂮"; break;
    }
    break;
  }
};

#endif
