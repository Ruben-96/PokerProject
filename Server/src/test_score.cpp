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

BOOST_AUTO_TEST_CASE(hands_get_expected_score)
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

    std::vector<Card> straight_flush = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(STRAIGHT_FLUSH == game.score_hand(straight_flush));

    std::vector<Card> two_pair =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, QUEEN, "queenhearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(TWO_PAIR == game.score_hand(two_pair));
    
    std::vector<Card> three_kind =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(THREE_KIND == game.score_hand(three_kind));
    
    std::vector<Card> flush = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, TWO, "2spades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    };
    BOOST_CHECK(FLUSH == game.score_hand(flush));

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

BOOST_AUTO_TEST_SUITE_END()

/*
int main(int argc, char* argv[])
{
    std::unordered_map<Rank, std::string> hand_rank_names 
    {
        {ROYAL_FLUSH, "Royal Flush"}, {STRAIGHT_FLUSH, "Straight Flush"}, {FOUR_KIND, "Four of a Kind"}, 
        {FULL_HOUSE, "Full House"}, {FLUSH, "Flush"}, {STRAIGHT, "Straight"}, 
        {THREE_KIND, "Three of a Kind"}, {TWO_PAIR, "Two Pairs"}, {PAIR, "Pair"}, {HIGH_CARD, "High Card"}
    };
    Game game;
    std::vector<std::vector<Card>> hands;
    std::cout << "HAND SCORING TEST" << std::endl;
    
    for(unsigned int i = 0; i < 5; i++)
    {
        std::vector<Card> hand;
        game.deal_to_hand(game.deck, hand);
        hands.push_back(hand);
    }

    for(unsigned int i = 0; i < hands.size(); i++)
    {
        std::cout << "Hand " << i << " contents: "; 
        for(auto card: hands.at(i))
        {
            std::cout << card.str << " ";
        }
        std::cout << std::endl;
        
        Rank hand_score = game.score_hand(hands.at(i));
        std::cout << "Hand " << i << " score: " << hand_rank_names.at(hand_score) << std::endl;
    }
}
*/
