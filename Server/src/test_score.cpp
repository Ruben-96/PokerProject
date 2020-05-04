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
#include <unordered_map>
#include "asio.hpp"
#include "chat_message.hpp"
#include "json.hpp"
#include "dealer.hpp"

#define BOOST_TEST_MODULE scoretest
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(test_scores)

BOOST_AUTO_TEST_CASE(all_hands_get_scored_within_bounds)
{
    Game game;
    std::vector<std::vector<Card>> hands;
    
    for(unsigned int i = 0; i < 5; i++)
    {
        std::vector<Card> hand;
        game.deal_to_hand(game.deck, hand);
        hands.push_back(hand);
    }

    for(unsigned int i = 0; i < hands.size(); i++)
    {
        Rank hand_score = game.score_hand(hands.at(i));
        BOOST_CHECK(hand_score >= 0 && hand_score < HAND_RANK_MAX);
    }
}

BOOST_AUTO_TEST_CASE(score_royal_flush)
{
    Game game;
    std::vector<Card> royal_flush = 
    {
        Card(SPADES, ACE, "acespades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(ROYAL_FLUSH == game.score_hand(royal_flush));
}

BOOST_AUTO_TEST_CASE(score_straight_flush)
{
    Game game;
    std::vector<Card> straight_flush = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(STRAIGHT_FLUSH == game.score_hand(straight_flush));
}

BOOST_AUTO_TEST_CASE(score_four_kind)
{
    Game game;
    std::vector<Card> four_kind =
    {
        Card(DIAMONDS, FOUR, "4diamonds"),
        Card(HEARTS, FOUR, "4hearts"),
        Card(CLUBS, FOUR, "4clubs"),
        Card(SPADES, FOUR, "4spades"),
        Card(SPADES, THREE, "3dpades")
    };
    BOOST_CHECK(FOUR_KIND == game.score_hand(four_kind));
}

BOOST_AUTO_TEST_CASE(score_full_house)
{
    Game game;
    std::vector<Card> full_house =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(DIAMONDS, TEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(FULL_HOUSE == game.score_hand(full_house));
}

BOOST_AUTO_TEST_CASE(score_flush)
{
    Game game;
    std::vector<Card> flush = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, TWO, "2spades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(FLUSH == game.score_hand(flush));
}

BOOST_AUTO_TEST_CASE(score_straight)
{
    Game game;
    std::vector<Card> straight =
    {
        Card(SPADES, TWO, "2spades"),
        Card(DIAMONDS, THREE, "3diamonds"),
        Card(HEARTS, FOUR, "4hearts"),
        Card(CLUBS, FIVE, "5clubs"),
        Card(CLUBS, SIX, "6clubs")
    };
    BOOST_CHECK(STRAIGHT == game.score_hand(straight));
}

BOOST_AUTO_TEST_CASE(score_three_kind)
{
    Game game;
    std::vector<Card> three_kind =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(THREE_KIND == game.score_hand(three_kind));
}

BOOST_AUTO_TEST_CASE(score_two_pair)
{
    Game game;
    std::vector<Card> two_pair =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, QUEEN, "queenhearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(TWO_PAIR == game.score_hand(two_pair));
}

BOOST_AUTO_TEST_CASE(score_pair)
{
    Game game;
    std::vector<Card> pair =
    {
        Card(SPADES, JACK, "jackspades"),
        Card(HEARTS, JACK, "jackhearts"),
        Card(HEARTS, TEN, "10hearts"),
        Card(DIAMONDS, THREE, "3diamonds"),
        Card(CLUBS, SIX, "6clubs"),
    };
    BOOST_CHECK(PAIR == game.score_hand(pair));
}

BOOST_AUTO_TEST_CASE(score_high_card)
{
    Game game;
    std::vector<Card> high_card =
    {
        Card(SPADES, JACK, "jackspades"),
        Card(HEARTS, KING, "kinghearts"),
        Card(HEARTS, TEN, "10hearts"),
        Card(DIAMONDS, THREE, "3diamonds"),
        Card(CLUBS, SIX, "6clubs"),
    };
    BOOST_CHECK(HIGH_CARD == game.score_hand(high_card));
}

BOOST_AUTO_TEST_SUITE_END()

