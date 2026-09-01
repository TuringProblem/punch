#ifndef RENDER_HPP
#define RENDER_HPP

#include "cards.hpp"
#include <array>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

/**
 * @author: { @Override } : 20260831
 *
 * Big readable cards, five lines tall:
 *
 *   ┌─────┐  ┌─────┐
 *   │A    │  │10   │
 *   │  ♠  │  │  ♥  │
 *   │    A│  │   10│
 *   └─────┘  └─────┘
 *
 * Box drawing and ♠♥♦♣ are in fonts basically everywhere, unlike the
 * U+1F0A1 playing card block which Windows Terminal has no glyphs for.
 *
 * Set PUNCH_ASCII=1 for a plain fallback, NO_COLOR=1 to drop the red.
 **/

inline constexpr int CARD_INNER = 5;
inline constexpr int CARD_LINES = 5;

inline bool asciiMode() {
  static const bool on = std::getenv("PUNCH_ASCII") != nullptr;
  return on;
}

inline bool colorMode() {
  static const bool on = std::getenv("NO_COLOR") == nullptr;
  return on;
}

constexpr std::string_view rankText(CardType type) noexcept {
  switch (type) {
  case CardType::ACE:
    return "A";
  case CardType::TWO:
    return "2";
  case CardType::THREE:
    return "3";
  case CardType::FOUR:
    return "4";
  case CardType::FIVE:
    return "5";
  case CardType::SIX:
    return "6";
  case CardType::SEVEN:
    return "7";
  case CardType::EIGHT:
    return "8";
  case CardType::NINE:
    return "9";
  case CardType::TEN:
    return "10";
  case CardType::JACK:
    return "J";
  case CardType::QUEEN:
    return "Q";
  case CardType::KING:
    return "K";
  }
  return "?";
}

inline std::string suitGlyph(SuitType suit) {
  if (asciiMode()) {
    switch (suit) {
    case SuitType::CLUBS:
      return "C";
    case SuitType::DIAMONDS:
      return "D";
    case SuitType::HEARTS:
      return "H";
    case SuitType::SPADES:
      return "S";
    }
    return "?";
  }
  switch (suit) {
  case SuitType::CLUBS:
    return "♣";
  case SuitType::DIAMONDS:
    return "♦";
  case SuitType::HEARTS:
    return "♥";
  case SuitType::SPADES:
    return "♠";
  }
  return "?";
}

inline bool isRed(SuitType suit) noexcept {
  return suit == SuitType::HEARTS || suit == SuitType::DIAMONDS;
}

inline std::string paint(const std::string &text, SuitType suit) {
  if (!colorMode() || !isRed(suit))
    return text;
  return "\033[31m" + text + "\033[0m";
}

struct BoxChars {
  std::string_view topLeft, topRight, bottomLeft, bottomRight, horizontal,
      vertical;
};

inline BoxChars boxChars() {
  if (asciiMode())
    return {"+", "+", "+", "+", "-", "|"};
  return {"┌", "┐", "└", "┘", "─", "│"};
}

inline std::string repeat(std::string_view unit, int times) {
  std::string out;
  for (int i = 0; i < times; ++i)
    out += unit;
  return out;
}

inline std::array<std::string, CARD_LINES> renderCard(const Card &card) {
  const BoxChars box = boxChars();
  const std::string bar = repeat(box.horizontal, CARD_INNER);
  const std::string rank(rankText(card.type));
  const std::string pad(static_cast<std::size_t>(CARD_INNER) - rank.size(),
                        ' ');
  const std::string side(box.vertical);

  return {
      std::string(box.topLeft) + bar + std::string(box.topRight),
      side + paint(rank, card.suit) + pad + side,
      side + "  " + paint(suitGlyph(card.suit), card.suit) + "  " + side,
      side + pad + paint(rank, card.suit) + side,
      std::string(box.bottomLeft) + bar + std::string(box.bottomRight),
  };
}

inline std::array<std::string, CARD_LINES> renderFaceDown() {
  const BoxChars box = boxChars();
  const std::string bar = repeat(box.horizontal, CARD_INNER);
  const std::string fill =
      asciiMode() ? repeat("#", CARD_INNER) : repeat("░", CARD_INNER);
  const std::string side(box.vertical);

  return {
      std::string(box.topLeft) + bar + std::string(box.topRight),
      side + fill + side,
      side + fill + side,
      side + fill + side,
      std::string(box.bottomLeft) + bar + std::string(box.bottomRight),
  };
}

// Lay faces out left to right and stitch them into one printable block.
inline std::string cardRow(const std::vector<Card> &cards,
                           std::string_view indent = "  ", int hidden = 0) {
  std::vector<std::array<std::string, CARD_LINES>> faces;
  faces.reserve(cards.size() + static_cast<std::size_t>(hidden));
  for (const Card &card : cards)
    faces.push_back(renderCard(card));
  for (int i = 0; i < hidden; ++i)
    faces.push_back(renderFaceDown());

  std::string out;
  for (int line = 0; line < CARD_LINES; ++line) {
    out += indent;
    for (const auto &face : faces) {
      out += face[static_cast<std::size_t>(line)];
      out += " ";
    }
    out += "\n";
  }
  return out;
}

inline std::string cardRow(const Card *cards, int count,
                           std::string_view indent = "  ") {
  return cardRow(std::vector<Card>(cards, cards + count), indent);
}

#endif
