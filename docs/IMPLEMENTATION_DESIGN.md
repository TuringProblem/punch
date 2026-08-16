# Implementation 

#### Player
```
Player = {
    name: string;
    health: 4;
    hand: Pair<CardType>;
    hasBLocks: boolean;
    blocks: number;
}
```

#### Suit 
```
enum Suit = {
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES
}
```

#### Value
```
enum Value = {
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
    KING
}
```


#### Card
```
CardType = {
    suit: Suit;
    value: Value;
}
```

#### Table

```
Table = {
    deck_size: number;
    deck: CardType[52 * deck_size]; // size of the deck
    dealer: Player;
    players: Player[];
}
```
