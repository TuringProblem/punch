#ifndef SHOWDOWN_HPP
#define SHOWDOWN_HPP

#include "hand.hpp"
#include "handrank.hpp"
#include <algorithm>
#include <array>
#include <vector>

/**
 * @author: { @Override } : 20260831
 * -----
 *  Who won the punch pot. Cards get compared only if two or more players
 *  are still in; otherwise the last player standing takes it uncontested.
 *
 *  Ties go to a redraw. The five board cards stay exactly where they are
 *  and every tied player is dealt two fresh hole cards off the rest of the
 *  deck. Best hand takes it. Still tied, redraw again.
 **/

struct TieRound {
  std::vector<int> seats;
  std::vector<std::array<Card, 2>> hole;
  std::vector<HandRank> ranks;
  std::vector<int> winners;
};

struct Showdown {
  std::vector<int> winners;
  std::vector<HandRank> best;
  bool contested = false;
  std::vector<TieRound> tiebreak;
  bool unresolved = false;
};

inline constexpr int MAX_TIE_ROUNDS = 8;

inline HandRank rankWith(const HandState &hand,
                         const std::array<Card, 2> &hole) {
  std::array<Card, 7> cards{};
  cards[0] = hole[0];
  cards[1] = hole[1];
  for (int i = 0; i < 5; ++i) {
    cards[static_cast<std::size_t>(2 + i)] =
        hand.board[static_cast<std::size_t>(i)];
  }
  return evaluate7(cards);
}

inline HandRank rankOf(const HandState &hand, int seatIndex) {
  return rankWith(hand, hand.seats[static_cast<std::size_t>(seatIndex)].hole);
}

inline std::vector<int> bestOf(const std::vector<int> &seats,
                               const std::vector<HandRank> &ranks) {
  std::size_t bestIdx = 0;
  for (std::size_t i = 1; i < seats.size(); ++i) {
    if (ranks[i] > ranks[bestIdx])
      bestIdx = i;
  }

  std::vector<int> winners;
  for (std::size_t i = 0; i < seats.size(); ++i) {
    if (ranks[i] == ranks[bestIdx])
      winners.push_back(seats[i]);
  }
  return winners;
}

inline Showdown resolveShowdown(HandState &hand) {
  Showdown result;
  result.best.resize(static_cast<std::size_t>(hand.seatCount()));

  // Folded everyone out. No cards are compared, no cards are shown.
  if (hand.liveCount() <= 1) {
    result.winners.push_back(nextLiveSeat(hand, 0));
    return result;
  }

  result.contested = true;

  std::vector<int> live;
  std::vector<HandRank> ranks;
  for (int i = 0; i < hand.seatCount(); ++i) {
    if (!hand.seats[static_cast<std::size_t>(i)].inHand())
      continue;
    const HandRank rank = rankOf(hand, i);
    result.best[static_cast<std::size_t>(i)] = rank;
    live.push_back(i);
    ranks.push_back(rank);
  }

  result.winners = bestOf(live, ranks);

  int round = 0;
  while (result.winners.size() > 1 && round < MAX_TIE_ROUNDS) {
    const std::size_t needed = result.winners.size() * 2;
    if (hand.deck.remaining() < needed) {
      result.unresolved = true;
      break;
    }

    TieRound tie;
    tie.seats = result.winners;
    for (std::size_t i = 0; i < tie.seats.size(); ++i) {
      const std::array<Card, 2> fresh{hand.deck.deal(), hand.deck.deal()};
      tie.hole.push_back(fresh);
      tie.ranks.push_back(rankWith(hand, fresh));
    }
    tie.winners = bestOf(tie.seats, tie.ranks);

    result.winners = tie.winners;
    result.tiebreak.push_back(std::move(tie));
    ++round;
  }

  if (result.winners.size() > 1) {
    result.unresolved = true;
    result.winners.resize(1);
  }

  return result;
}

#endif
