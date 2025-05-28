# Blackjack

Blackjack (also called "21") is a popular casino card game in which players aim to get a total of 21 or as close to it as possible without exceeding it. The game is played against the dealer (a casino representative) rather than against other players.

---

## Project Overview

This project is a console-based implementation of the Blackjack game written in C++. It allows a human player to compete against a dealer, following the standard rules of Blackjack. The game includes features such as betting, card shuffling, and determining winners based on the total card values.

---

## Features

- Full implementation of Blackjack rules.
- Player betting system with a starting balance of $1000.
- Dealer logic for hitting or standing based on card totals.
- Card shuffling using a random number generator.
- Support for UTF-8 card symbols (♥, ♦, ♣, ♠) in the console.
- Detection of Blackjack and bust scenarios.
- Interactive gameplay with user input.

---

## How to Play

1. **Objective:** Get a card total of 21 or as close to it as possible without exceeding 21.
2. **Gameplay:**
   - Place a bet at the start of each round.
   - Both the player and the dealer are dealt two cards.
   - The player can choose to "hit" (take another card) or "stand" (end their turn).
   - The dealer must hit until their total is at least 17.
   - The winner is determined based on the card totals:
     - If the player exceeds 21, they lose.
     - If the dealer exceeds 21, the player wins.
     - If neither exceeds 21, the higher total wins.
3. **End of Game:** The game ends when the player runs out of money or chooses to quit.

---

## Classes and Structure

### 1. **Card**
Represents a playing card with a rank, suit, and face-up state.

### 2. **Hand**
Represents a collection of cards held by a player or dealer.

### 3. **Deck**
Represents a deck of 52 cards, with methods for shuffling and dealing.

### 4. **Player**
Base class for both the human player and the dealer.

### 5. **HumanPlayer**
Derived class for the human player, allowing interactive input.

### 6. **Dealer**
Derived class for the dealer, with logic for automated decisions.

### 7. **Game**
Manages the overall game flow, including betting, dealing cards, and determining winners.

---

## Requirements

- **Compiler:** A C++ compiler supporting C++11 or later (e.g., g++, MSVC).
- **Platform:** Windows (for UTF-8 console support via Windows API).

---

## How to Run

1. Compile the source code using a C++ compiler.
2. Run the executable.
3. Enter your name when prompted.
4. Follow the on-screen instructions to play the game.

---

## Example Gameplay

YOUR NAME: John YOU HAVE: $1000 BET: 100

--- ROUND START --- Dealer: XX 7 [??] John: A♠ 10♣ [21]

John WINS 1.5x! YOUR BALANCE: $1150 PLAY AGAIN? (y/n): n

---

## Author

This project was developed to demonstrate object-oriented programming principles in C++.