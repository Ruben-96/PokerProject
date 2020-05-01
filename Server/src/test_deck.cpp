#include <iostream>
#include <thread>
#include <chrono>
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

#define BOOST_TEST_MODULE decktest
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE (deck_test)

BOOST_AUTO_TEST_CASE (number_of_cards_in_deck)
{
    Game game;
    std::vector<Card> deck1 = game.get_new_deck();
    BOOST_CHECK( deck1.size() == 52 );
}

BOOST_AUTO_TEST_CASE (two_decks_not_alike)
{
    Game game;

    std::vector<Card> deck1 = game.get_new_deck();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::vector<Card> deck2 = game.get_new_deck();

    unsigned int matches = 0;
    for(unsigned int i = 0; i < deck1.size(); i++)
    {
        if(deck1.at(i) == deck2.at(i)) matches++;
    }
    BOOST_CHECK( matches < deck1.size() );
    BOOST_CHECK( matches < deck2.size() );
}

BOOST_AUTO_TEST_CASE(deck_cards_unique)
{
    Game game;
    std::vector<Card> deck = game.get_new_deck();
    std::unordered_set<std::string> card_str_set;
    std::unordered_set<Suit> card_suit_set;
    std::unordered_set<Card_rank> card_rank_set;
    for(auto card: deck)
    {
        card_str_set.insert(card.str);
        card_rank_set.insert(card.rank);
        card_suit_set.insert(card.suit);
    }
    BOOST_CHECK( card_str_set.size() == 52 );
    BOOST_CHECK( card_rank_set.size() == 13 );
    BOOST_CHECK( card_suit_set.size() == 4 );
}

BOOST_AUTO_TEST_SUITE_END()
