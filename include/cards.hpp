#ifndef CARDS_HPP
#define CARDS_HPP

#include <map>
#include <random>
#include <string>
/**
 * @author: { @Override } : 20260816 00:56
 * --------------------------------------
 *
 *
 * there are 52 | 48 | 40 | 32 cards in a deck - and using 4 decks per table
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

// ... okay I want to use 🂡 🂱 🃁 🃑
constexpr std::string getCardDesign(const CardType &cardType,
                                    const SuitType &suitType) noexcept {
  if (suitType == SuitType::CLUBS) {
    switch (cardType) {
    case CardType::ACE:
      return "🃑";
      break;
    case CardType::TWO:
      return "🃒";
      break;
    case CardType::THREE:
      return "🃓";
      break;
    case CardType::FOUR:
      return "🃔";
      break;
    case CardType::FIVE:
      return "🃕";
      break;
    case CardType::SIX:
      return "🃖";
      break;
    case CardType::SEVEN:
      return "🃗";
      break;
    case CardType::EIGHT:
      return "🃘";
      break;
    case CardType::NINE:
      return "🃙";
      break;
    case CardType::TEN:
      return "🃚";
      break;
    case CardType::JACK:
      return "🃛";
      break;
    case CardType::QUEEN:
      return "🃝";
      break;
    case CardType::KING:
      return "🃞";
      break;
    }
  }

  if (suitType == SuitType::DIAMONDS) {
    switch (cardType) {
    case CardType::ACE:
      return "🃁";
      break;
    case CardType::TWO:
      return "🃂";
      break;
    case CardType::THREE:
      return "🃃";
      break;
    case CardType::FOUR:
      return "🃄";
      break;
    case CardType::FIVE:
      return "🃅";
      break;
    case CardType::SIX:
      return "🃆";
      break;
    case CardType::SEVEN:
      return "🃇";
      break;
    case CardType::EIGHT:
      return "🃈";
      break;
    case CardType::NINE:
      return "🃉";
      break;
    case CardType::TEN:
      return "🃊";
      break;
    case CardType::JACK:
      return "🃋";
      break;
    case CardType::QUEEN:
      return "🃍";
      break;
    case CardType::KING:
      return "🃎";
      break;
    }
  }

  if (suitType == SuitType::HEARTS) {
    switch (cardType) {
    case CardType::ACE:
      return "🂱";
      break;
    case CardType::TWO:
      return "🂲";
      break;
    case CardType::THREE:
      return "🂳";
      break;
    case CardType::FOUR:
      return "🂴";
      break;
    case CardType::FIVE:
      return "🂵";
      break;
    case CardType::SIX:
      return "🂶";
      break;
    case CardType::SEVEN:
      return "🂷";
      break;
    case CardType::EIGHT:
      return "🂸";
      break;
    case CardType::NINE:
      return "🂹";
      break;
    case CardType::TEN:
      return "🂺";
      break;
    case CardType::JACK:
      return "🂻";
      break;
    case CardType::QUEEN:
      return "🂽";
      break;
    case CardType::KING:
      return "🂾";
      break;
    }
  }

  if (suitType == SuitType::SPADES) {
    switch (cardType) {
    case CardType::ACE:
      return "🂡";
      break;
    case CardType::TWO:
      return "🂢";
      break;
    case CardType::THREE:
      return "🂣";
      break;
    case CardType::FOUR:
      return "🂤";
      break;
    case CardType::FIVE:
      return "🂥";
      break;
    case CardType::SIX:
      return "🂦";
      break;
    case CardType::SEVEN:
      return "🂧";
      break;
    case CardType::EIGHT:
      return "🂨";
      break;
    case CardType::NINE:
      return "🂩";
      break;
    case CardType::TEN:
      return "🂪";
      break;
    case CardType::JACK:
      return "🂫";
      break;
    case CardType::QUEEN:
      return "🂭";
      break;
    case CardType::KING:
      return "🂮";
      break;
    }
  }

  return "";
}

#endif
