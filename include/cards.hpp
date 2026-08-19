#ifndef CARDS_HPP
#define CARDS_HPP

#include <cstdlib>
#include <random>
#include <string_view>
#include <vector>
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
constexpr std::string_view getCardDesign(CardType cardType,
                                         SuitType suitType) noexcept {
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

inline std::mt19937 &rng() {
  static thread_local std::mt19937 gen{std::random_device{}()};
  return gen;
}

inline std::vector<std::string_view> getRandomPair() {
  std::uniform_int_distribution<int> suitDis(0, 3);
  std::uniform_int_distribution<int> cardDis(0, 12);

  std::vector<std::string_view> cards;
  cards.reserve(2);
  for (int i = 0; i < 2; ++i) {
    cards.push_back(getCardDesign(static_cast<CardType>(cardDis(rng())),
                                  static_cast<SuitType>(suitDis(rng()))));
  }
  return cards;
}

#endif
