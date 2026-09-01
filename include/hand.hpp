#ifndef HAND_HPP
#define HAND_HPP

#include "deck.hpp"
#include "game.hpp"
#include "seat.hpp"
#include <array>
#include <vector>

/**
 * @author: { @Override } : 20260831
 * -----
 *  HandState is one deal, start to finish. Rebuilt every hand.
 *
 *  Street order: PREFLOP (hole cards only) -> FLOP (3) -> TURN (4th)
 *  -> RIVER (5th) -> SHOWDOWN -> PUNCH. A betting round runs between
 *  every one of those.
 *
 *  Nothing in here prints. Rules produce data, rendering reads it.
 **/

enum class Street { PREFLOP, FLOP, TURN, RIVER, SHOWDOWN, PUNCH, DONE };

constexpr std::string_view streetName(Street street) noexcept {
  switch (street) {
  case Street::PREFLOP:
    return "Preflop";
  case Street::FLOP:
    return "Flop";
  case Street::TURN:
    return "Turn";
  case Street::RIVER:
    return "River";
  case Street::SHOWDOWN:
    return "Showdown";
  case Street::PUNCH:
    return "Punch";
  case Street::DONE:
    return "Done";
  }
  return "";
}

struct HandState {
  Deck deck;
  std::vector<Seat> seats;

  std::array<Card, 5> board{};
  int boardCount = 0;

  Street street = Street::PREFLOP;

  int currentBet = 0;
  int lastRaiseSize = 0;
  int actor = 0;
  int lastAggressor = -1;

  int seatCount() const noexcept { return static_cast<int>(seats.size()); }

  int punchPot() const noexcept {
    int level = 0;
    for (const Seat &seat : seats) {
      if (seat.committed > level)
        level = seat.committed;
    }
    return level;
  }

  int liveCount() const noexcept {
    int live = 0;
    for (const Seat &seat : seats) {
      if (seat.inHand())
        ++live;
    }
    return live;
  }
};

inline int nextLiveSeat(const HandState &hand, int from) noexcept {
  const int count = hand.seatCount();
  for (int step = 0; step < count; ++step) {
    const int idx = (from + step) % count;
    if (hand.seats[static_cast<std::size_t>(idx)].inHand())
      return idx;
  }
  return from;
}

inline int nextActableSeat(const HandState &hand, int from) noexcept {
  const int count = hand.seatCount();
  for (int step = 0; step < count; ++step) {
    const int idx = (from + step) % count;
    if (hand.seats[static_cast<std::size_t>(idx)].canAct())
      return idx;
  }
  return -1;
}

inline void commit(HandState &hand, const Game &game, int seatIndex,
                   int amount) {
  Seat &seat = hand.seats[static_cast<std::size_t>(seatIndex)];
  const Player &player = game.players[static_cast<std::size_t>(seat.id)];

  const int room = player.health - seat.committed;
  const int actual = amount < room ? amount : room;

  seat.committed += actual;
  seat.streetCommitted += actual;

  if (seat.committed >= player.health) {
    seat.status = SeatStatus::ALL_IN;
  }
}

inline void postBlinds(HandState &hand, const Game &game) {
  const int count = hand.seatCount();

  const int smallSeat = (count == 2) ? game.button : (game.button + 1) % count;
  const int bigSeat = (smallSeat + 1) % count;

  commit(hand, game, smallSeat, game.smallBlind);
  commit(hand, game, bigSeat, game.bigBlind);

  hand.currentBet = game.bigBlind;
  hand.lastRaiseSize = game.bigBlind;
  hand.lastAggressor = bigSeat;

  hand.actor = nextActableSeat(hand, (bigSeat + 1) % count);
}

inline HandState startHand(const Game &game) {
  HandState hand;

  for (const Player &player : game.players) {
    if (!player.isKnockedOut) {
      Seat seat;
      seat.id = player.id;
      hand.seats.push_back(seat);
    }
  }

  hand.deck.shuffle();

  for (int round = 0; round < 2; ++round) {
    for (Seat &seat : hand.seats) {
      seat.hole[static_cast<std::size_t>(round)] = hand.deck.deal();
    }
  }

  postBlinds(hand, game);
  return hand;
}

// Wipe per-street betting state. Postflop action starts left of the button.
inline void beginStreet(HandState &hand, const Game &game) {
  for (Seat &seat : hand.seats) {
    seat.streetCommitted = 0;
    seat.hasActed = false;
  }
  hand.currentBet = 0;
  hand.lastRaiseSize = game.bigBlind;
  hand.lastAggressor = -1;
  hand.actor = nextActableSeat(hand, (game.button + 1) % hand.seatCount());
}

inline void dealFlop(HandState &hand) {
  hand.deck.burn();
  for (int i = 0; i < 3; ++i) {
    hand.board[static_cast<std::size_t>(i)] = hand.deck.deal();
  }
  hand.boardCount = 3;
  hand.street = Street::FLOP;
}

inline void dealTurn(HandState &hand) {
  hand.deck.burn();
  hand.board[3] = hand.deck.deal();
  hand.boardCount = 4;
  hand.street = Street::TURN;
}

inline void dealRiver(HandState &hand) {
  hand.deck.burn();
  hand.board[4] = hand.deck.deal();
  hand.boardCount = 5;
  hand.street = Street::RIVER;
}

#endif
