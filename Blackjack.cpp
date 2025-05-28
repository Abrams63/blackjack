#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <clocale>
#include <windows.h>
#include <locale>
#include <codecvt>

using namespace std;

// Перелік мастей карт
enum Suit { HEARTS, DIAMONDS, CLUBS, SPADES, SUIT_COUNT };

// Перелік рангів карт
enum Rank {
    ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN,
    EIGHT, NINE, TEN, JACK, QUEEN, KING, RANK_COUNT
};

class Card {
private:
    Rank rank;
    Suit suit;
    bool isFaceUp;

public:
    Card(Rank r, Suit s) : rank(r), suit(s), isFaceUp(true) {}

    // Повертає значення карти (туз = 11, інші за номіналом)
    int getValue() const {
        if (rank >= TWO && rank <= TEN) {
            return rank;
        }
        else if (rank >= JACK && rank <= KING) {
            return 10;
        }
        else { // ACE
            return 11;
        }
    }

    void flip() { isFaceUp = !isFaceUp; }
    bool isFaceUpCard() const { return isFaceUp; }

    friend std::ostream& operator<<(std::ostream& os, const Card& card) {
        const std::string RANKS[] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
        const std::string SUITS[] = { "\u2665", "\u2666", "\u2663", "\u2660" };

        if (card.isFaceUp) {
            os << RANKS[card.rank - 1] << SUITS[card.suit];
        }
        else {
            os << "XX";
        }
        return os;
    }
};

class Hand {
protected:
    std::vector<Card> cards;

public:
    void add(Card card) {
        cards.push_back(card);
    }

    void clear() {
        cards.clear();
    }

    int getTotal() const {
        int total = 0;
        int aceCount = 0;

        // Рахуємо суму без врахування тузів
        for (const Card& card : cards) {
            int value = card.getValue();
            total += value;
            if (card.getValue() == 11) aceCount++;
        }

        // Коригуємо суму, якщо є тузи і перебір
        while (total > 21 && aceCount > 0) {
            total -= 10; // Вважаємо туз за 1 замість 11
            aceCount--;
        }

        return total;
    }

    void flipFirstCard() {
        if (!cards.empty()) {
            cards[0].flip();
        }
    }

    // Метод для доступу до вектору карт
    const std::vector<Card>& getCards() const {
        return cards;
    }

    // Метод для отримання змінюваного вектора карт
    std::vector<Card>& getMutableCards() {
        return cards;
    }

    friend std::ostream& operator<<(std::ostream& os, const Hand& hand) {
        for (const Card& card : hand.cards) {
            os << card << " ";
        }
        os << "[" << hand.getTotal() << "]";
        return os;
    }

    bool isBusted() const { return getTotal() > 21; }
    bool hasBlackjack() const { return cards.size() == 2 && getTotal() == 21; }
};

class Deck {
private:
    std::vector<Card> cards;

public:
    Deck() {
        populate();
    }

    void populate() {
        cards.clear();
        for (int s = HEARTS; s < SUIT_COUNT; s++) {
            for (int r = ACE; r < RANK_COUNT; r++) {
                cards.push_back(Card(static_cast<Rank>(r), static_cast<Suit>(s)));
            }
        }
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    void deal(Hand& hand) {
        if (!cards.empty()) {
            hand.add(cards.back());
            cards.pop_back();
        }
    }

    bool isEmpty() const { return cards.empty(); }
};

class Player {
protected:
    Hand hand;
    std::string name;

public:
    Player(const std::string& n) : name(n) {}

    bool isBusted() const { return hand.isBusted(); }
    bool hasBlackjack() const { return hand.hasBlackjack(); }
    Hand& getHand() { return hand; }
    const Hand& getHand() const { return hand; }
    std::string getName() const { return name; }

    virtual bool isHitting() const = 0;

    void takeCard(Card card) { hand.add(card); }
    void clearHand() { hand.clear(); }
    void flipCards() { /* Базовий гравець не перевертає карти */ }

    virtual ~Player() {}
};

class HumanPlayer : public Player {
public:
    HumanPlayer(const std::string& name) : Player(name) {}

    bool isHitting() const override {
        std::cout << name << ", хочете взяти ще карту? (y/n): ";
        char response;
        std::cin >> response;
        return (response == 'y' || response == 'Y');
    }
};

class Dealer : public Player {
public:
    Dealer() : Player("Дилер") {}

    bool isHitting() const override {
        return getHand().getTotal() < 17;
    }

    void flipFirstCard() {
        hand.flipFirstCard();
    }

    void flipCards() {
        for (Card& card : hand.getMutableCards()) {
            if (!card.isFaceUpCard()) card.flip();
        }
    }
};

class Game {
private:
    Deck deck;
    HumanPlayer player;
    Dealer dealer;
    int playerMoney;
    int currentBet;

public:
    Game(std::string playerName)
        : player(playerName), playerMoney(1000), currentBet(0) {
    }

    void play() {
        while (playerMoney > 0) {
            placeBet();
            deck.populate();
            deck.shuffle();

            // Підготовка до раунду
            player.clearHand();
            dealer.clearHand();

            // Початкова роздача
            deck.deal(player.getHand());
            deck.deal(dealer.getHand());
            deck.deal(player.getHand());
            deck.deal(dealer.getHand());

            dealer.flipFirstCard();

            // Відображення рук
            std::cout << "\n--- ПОЧАТОК РАУНДУ ---\n";
            displayHands(false);

            // Перевірка на блекджек
            if (player.hasBlackjack() || dealer.hasBlackjack()) {
                checkBlackjacks();
                settleBet();
                continue;
            }

            // Хід гравця
            playerTurn();

            // Хід дилера, якщо гравець не програв
            if (!player.isBusted()) {
                dealerTurn();
            }

            // Визначення переможця
            determineWinner();
            settleBet();

            // Перевірка на кінець гри
            if (playerMoney <= 0) {
                std::cout << "ГРОШІ ЗАКІНЧИЛИСЬ. ГРА ЗАВЕРШЕНА\n";
                break;
            }

            // Запит на продовження
            std::cout << "ГРАТИ ЩЕ? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice != 'y' && choice != 'Y') break;
        }
    }

private:
    void placeBet() {
        do {
            std::cout << "\nУ ВАС Є: $" << playerMoney << "\nСТАВКА: ";
            std::cin >> currentBet;
        } while (currentBet <= 0 || currentBet > playerMoney);
    }

    void displayHands(bool showDealerHand = true) const {
        std::cout << dealer.getName() << ": ";
        if (showDealerHand) {
            std::cout << dealer.getHand() << std::endl;
        }
        else {
            // Показуємо лише другу карту дилера як XX
            std::cout << "XX " << dealer.getHand().getCards()[1] << " [??]\n";
        }

        std::cout << player.getName() << ": " << player.getHand() << "\n\n";
    }

    void checkBlackjacks() {
        bool playerBJ = player.hasBlackjack();
        bool dealerBJ = dealer.hasBlackjack();

        dealer.flipCards();
        displayHands(true);

        if (playerBJ && dealerBJ) {
            std::cout << "НІЧИЯ\n";
        }
        else if (playerBJ) {
            std::cout << player.getName() << " ВИГРАВ 1.5x!\n";
            playerMoney += static_cast<int>(currentBet * 1.5);
        }
        else if (dealerBJ) {
            std::cout << dealer.getName() << " ВИГРАВ! ВИ ПРОГРАЛИ\n";
            playerMoney -= currentBet;
        }
    }

    void playerTurn() {
        while (!player.isBusted() && player.isHitting()) {
            deck.deal(player.getHand());
            displayHands(false);
        }

        if (player.isBusted()) {
            std::cout << player.getName() << " ПРОГРАВ.\n";
        }
    }

    void dealerTurn() {
        dealer.flipCards();
        displayHands(true);

        while (dealer.isHitting()) {
            std::cout << dealer.getName() << " БЕРЕ КАРТУ\n";
            deck.deal(dealer.getHand());
            displayHands(true);
        }

        if (dealer.isBusted()) {
            std::cout << dealer.getName() << " ПЕРЕБРАВ. ВИ ВИГРАЛИ\n";
        }
    }

    void determineWinner() {
        if (player.isBusted()) return; // Гравець вже програв

        int playerTotal = player.getHand().getTotal();
        int dealerTotal = dealer.getHand().getTotal();

        dealer.flipCards();
        displayHands(true);

        if (dealer.isBusted()) {
            playerMoney += currentBet;
        }
        else if (playerTotal > dealerTotal) {
            std::cout << player.getName() << " ВИГРАВ!\n";
            playerMoney += currentBet;
        }
        else if (playerTotal < dealerTotal) {
            std::cout << dealer.getName() << " ВИГРАВ!\n";
            playerMoney -= currentBet;
        }
        else {
            std::cout << "НІЧИЯ!\n";
        }
    }

    void settleBet() {
        std::cout << "\n--- РАУНД ЗАВЕРШЕНО ---\n";
        std::cout << "ВАШ БАЛАНС: $" << playerMoney << "\n\n";
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, "");

    std::wstring playerName;
    std::wcout << L"ВАШЕ ІМ'Я: ";
    std::getline(std::wcin, playerName);

    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::string playerNameUtf8 = conv.to_bytes(playerName);

    Game game(playerNameUtf8);
    game.play();

    return 0;
}