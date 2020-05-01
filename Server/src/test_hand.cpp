#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <functional>
#include <unordered_set>
#include <unistd.h>
#include "asio.hpp"
#include "chat_message.hpp"
#include "json.hpp"
#include "dealer.hpp"

#define BOOST_TEST_MODULE handtest
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE (hand_tests)

BOOST_AUTO_TEST_CASE(dealt_hands_have_five_cards)
{
    Game game;
    std::vector<std::vector<Card>> hands;

    for(int i = 0; i < PLAYER_COUNT_MAX; i++)
    {
        std::vector<Card> hand;
        game.deal_to_hand(game.deck, hand);
        hands.push_back(hand);
        BOOST_CHECK(hand.size() == 5);
    }
    BOOST_CHECK(game.deck.size() == 52-(PLAYER_COUNT_MAX*5));
}

BOOST_AUTO_TEST_CASE(replace_card_replaces_specified_cards)
{
    Game game;
    std::vector<Card> hand ={
        Card(SPADES, TWO, "2spades"), 
        Card(SPADES, FIVE, "5spades"),
        Card(DIAMONDS, ACE, "acediamonds"), 
        Card(HEARTS, JACK, "jackhearts"),
        Card(HEARTS, QUEEN, "jackhearts")
    };
    std::vector<Card> kept_cards = {
        Card(SPADES, FIVE, "5spades"),
        Card(DIAMONDS, ACE, "acediamonds"), 
        Card(HEARTS, QUEEN, "jackhearts")
    };

    std::vector<Card> discards = {
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, JACK, "jackhearts")
    };
    
    for(auto& card: hand)
    {
        game.deck.erase(std::remove(game.deck.begin(), game.deck.end(), card), game.deck.end()); // deletes matching card from hand
    }
    for(auto card: kept_cards)
    {
        BOOST_CHECK(std::find(hand.begin(), hand.end(), card) != hand.end());
    }

    game.replace_cards_from_hand(game.deck, hand, discards);
    for(auto card: discards)
    {
        BOOST_CHECK(std::find(hand.begin(), hand.end(), card) == hand.end());
    }
    BOOST_CHECK(hand.size() == 5);
}

BOOST_AUTO_TEST_SUITE_END()

