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

#define BOOST_TEST_MODULE wintest
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(test_win)

BOOST_AUTO_TEST_CASE(correct_winner_is_determined)
{
    Game game;
    game.player_uuids = {"homer","marge","bart","lisa","maggie"};
    
    for(auto player_uuid: game.player_uuids)
    {
        game.player_lookup_umap.insert({player_uuid, Player(player_uuid, player_uuid)});
    }
    Player* homer = &game.player_lookup_umap.at("homer");
    Player* marge = &game.player_lookup_umap.at("marge");
    Player* bart = &game.player_lookup_umap.at("bart");
    Player* lisa = &game.player_lookup_umap.at("lisa");
    Player* maggie = &game.player_lookup_umap.at("maggie");

    homer->is_active = true;
    homer->hand = 
    {
        Card(SPADES, ACE, "acespades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // royal flush, homer should win

    marge->is_active = true;
    marge->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // straight flush 

    bart->is_active = true;
    bart->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, QUEEN, "queenhearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // two pair 
    
    lisa->is_active = true;
    lisa->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // three kind 
    
    maggie->is_active = true;
    maggie->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, TWO, "2spades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // flush
    Rank high_rank;
    std::vector<std::string> winners = game.determine_winner(game.player_uuids, high_rank);
    BOOST_CHECK( winners.size() == 1 );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), homer->uuid_str ) != winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), marge->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), bart->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), lisa->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), maggie->uuid_str ) == winners.end() );
}

BOOST_AUTO_TEST_CASE(winnings_are_distributed_properly)
{
    Game game;
    game.player_uuids = {"homer","marge","bart","lisa","maggie"};
    
    for(auto player_uuid: game.player_uuids)
    {
        game.player_lookup_umap.insert({player_uuid, Player(player_uuid, player_uuid)});
    }
    Player* homer = &game.player_lookup_umap.at("homer");
    Player* marge = &game.player_lookup_umap.at("marge");
    Player* bart = &game.player_lookup_umap.at("bart");
    Player* lisa = &game.player_lookup_umap.at("lisa");
    Player* maggie = &game.player_lookup_umap.at("maggie");


    homer->bet = 200;
    homer->bank = 0;
    homer->is_active = true;
    homer->hand = 
    {
        Card(SPADES, ACE, "acespades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // royal flush, homer should win

    marge->bet = 150;
    marge->bank = 0;
    marge->is_active = true;
    marge->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // straight flush 

    bart->bet = 100;
    bart->bank = 0;
    bart->is_active = true;
    bart->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, QUEEN, "queenhearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // two pair 
    
    lisa->bet = 100;
    lisa->bank = 0;
    lisa->is_active = true;
    lisa->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // three kind 
    
    maggie->bet = 1;
    maggie->bank = 99;
    maggie->is_active = true;
    maggie->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, TWO, "2spades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // flush

    Rank high_rank;
    std::vector<std::string> winners = game.determine_winner(game.player_uuids, high_rank);
    BOOST_CHECK( winners.size() == 1 );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), homer->uuid_str ) != winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), marge->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), bart->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), lisa->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), maggie->uuid_str ) == winners.end() );
    BOOST_CHECK( homer->bank == 551 );
    BOOST_CHECK( (homer->bet + marge->bet + bart->bet + lisa->bet + maggie->bet) == 0 );
    BOOST_CHECK( marge->bank == 0 );
    BOOST_CHECK( bart->bank == 0 );
    BOOST_CHECK( lisa->bank == 0 );
    BOOST_CHECK( maggie->bank == 99 );
}

BOOST_AUTO_TEST_CASE(tie_is_broken)
{
    Game game;
    game.player_uuids = {"homer","marge","bart","lisa","maggie"};
    
    for(auto player_uuid: game.player_uuids)
    {
        game.player_lookup_umap.insert({player_uuid, Player(player_uuid, player_uuid)});
    }
    Player* homer = &game.player_lookup_umap.at("homer");
    Player* marge = &game.player_lookup_umap.at("marge");
    Player* bart = &game.player_lookup_umap.at("bart");
    Player* lisa = &game.player_lookup_umap.at("lisa");
    Player* maggie = &game.player_lookup_umap.at("maggie");

    homer->is_active = true;
    homer->hand = 
    {
        Card(HEARTS, NINE, "9spades"),
        Card(HEARTS, EIGHT, "8spades"),
        Card(HEARTS, QUEEN, "queenspades"),
        Card(HEARTS, JACK, "jackspades"),
        Card(HEARTS, TEN, "10spades")
    }; // straight flush 

    marge->is_active = true;
    marge->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // straight flush, marge should win
    
    bart->is_active = true;
    bart->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, QUEEN, "queenhearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // two pair 
    
    lisa->is_active = true;
    lisa->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // three kind 

    maggie->is_active = true;
    maggie->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, TWO, "2spades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // flush
    BOOST_TEST_CHECKPOINT( "Preparing " << game.player_uuids.size() << " players to be determined" );
    Rank high_rank;
    std::vector<std::string> winners = game.determine_winner(game.player_uuids, high_rank);
    BOOST_TEST_CHECKPOINT( "" << winners.size() << " player(s) win" );
    BOOST_CHECK( winners.size() == 1 );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), homer->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), marge->uuid_str ) != winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), bart->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), lisa->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), maggie->uuid_str ) == winners.end() );
}

BOOST_AUTO_TEST_CASE(tie_is_broken_low)
{
    Game game;
    game.player_uuids = {"homer","marge"};
    
    for(auto player_uuid: game.player_uuids)
    {
        game.player_lookup_umap.insert({player_uuid, Player(player_uuid, player_uuid)});
    }
    Player* homer = &game.player_lookup_umap.at("homer");
    Player* marge = &game.player_lookup_umap.at("marge");

    homer->is_active = true;
    homer->hand = 
    {
        Card(DIAMONDS, KING, "kingdiamonds"),
        Card(SPADES, KING, "kingspades"),
        Card(CLUBS, QUEEN, "queenclubs"),
        Card(SPADES, FOUR, "4spades"),
        Card(DIAMONDS, TWO, "2diamonds")
    }; // high pair 

    marge->is_active = true;
    marge->hand = 
    {
        Card(CLUBS, TWO, "aceclubs"),
        Card(SPADES, TEN, "10spades"),
        Card(HEARTS, EIGHT, "8hearts"),
        Card(SPADES, SEVEN, "7spades"),
        Card(HEARTS, SEVEN, "7hearts")
    }; // low pair
    
    BOOST_TEST_CHECKPOINT( "Preparing " << game.player_uuids.size() << " players to be determined" );
    Rank high_rank;
    std::vector<std::string> winners = game.determine_winner(game.player_uuids, high_rank);
    BOOST_TEST_CHECKPOINT( "" << winners.size() << " player(s) win" );
    BOOST_CHECK( winners.size() == 1 );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), homer->uuid_str ) != winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), marge->uuid_str ) == winners.end() );
}

BOOST_AUTO_TEST_CASE(tie_is_handled)
{
    Game game;
    game.player_uuids = {"homer","marge","bart","lisa","maggie"};
    
    for(auto player_uuid: game.player_uuids)
    {
        game.player_lookup_umap.insert({player_uuid, Player(player_uuid, player_uuid)});
    }
    Player* homer = &game.player_lookup_umap.at("homer");
    Player* marge = &game.player_lookup_umap.at("marge");
    Player* bart = &game.player_lookup_umap.at("bart");
    Player* lisa = &game.player_lookup_umap.at("lisa");
    Player* maggie = &game.player_lookup_umap.at("maggie");

    homer->is_active = true;
    homer->bank = 0;
    homer->bet = 100;
    homer->hand = 
    {
        Card(SPADES, ACE, "acespades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // royal flush, homer should win 

    marge->is_active = true;
    marge->bank = 0;
    marge->bet = 100;
    marge->hand = 
    {
        Card(HEARTS, ACE, "acespades"),
        Card(HEARTS, KING, "kingspades"),
        Card(HEARTS, QUEEN, "queenspades"),
        Card(HEARTS, JACK, "jackspades"),
        Card(HEARTS, TEN, "10spades")
    }; // royal flush, marge should also win

    bart->is_active = true;
    bart->bank = 0;
    bart->bet = 100;
    bart->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, QUEEN, "queenhearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // two pair 
    
    lisa->is_active = true;
    lisa->bank = 0;
    lisa->bet = 100;
    lisa->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // three kind 
    
    maggie->is_active = true;
    maggie->bank = 0;
    maggie->bet = 100;
    maggie->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, TWO, "2spades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // flush
    BOOST_TEST_CHECKPOINT( "Preparing " << game.player_uuids.size() << " players to be determined" );
    Rank high_rank;
    std::vector<std::string> winners = game.determine_winner(game.player_uuids, high_rank);
    BOOST_TEST_CHECKPOINT( "" << winners.size() << " player(s) win" );
    BOOST_CHECK( winners.size() == 2 );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), homer->uuid_str ) != winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), marge->uuid_str ) != winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), bart->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), lisa->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), maggie->uuid_str ) == winners.end() );
    BOOST_CHECK( homer->bank == 250 );
    BOOST_CHECK( marge->bank == 250 );
    BOOST_CHECK( bart->bank == 0 );
    BOOST_CHECK( lisa->bank == 0 );
    BOOST_CHECK( maggie->bank == 0 );
}

BOOST_AUTO_TEST_CASE(skips_inactive_player)
{
    Game game;
    game.player_uuids = {"homer","marge","bart","lisa","maggie"};
    
    for(auto player_uuid: game.player_uuids)
    {
        game.player_lookup_umap.insert({player_uuid, Player(player_uuid, player_uuid)});
    }
    Player* homer = &game.player_lookup_umap.at("homer");
    Player* marge = &game.player_lookup_umap.at("marge");
    Player* bart = &game.player_lookup_umap.at("bart");
    Player* lisa = &game.player_lookup_umap.at("lisa");
    Player* maggie = &game.player_lookup_umap.at("maggie");


    homer->bet = 200;
    homer->bank = 0;
    homer->is_active = false;
    homer->hand = 
    {
        Card(SPADES, ACE, "acespades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // royal flush, homer could win but he folded 

    marge->bet = 150;
    marge->bank = 0;
    marge->is_active = true;
    marge->hand = 
    {
        Card(HEARTS, ACE, "acespades"),
        Card(HEARTS, KING, "kingspades"),
        Card(HEARTS, QUEEN, "queenspades"),
        Card(HEARTS, JACK, "jackspades"),
        Card(HEARTS, TEN, "10spades")
    }; // royal flush, marge should win

    bart->bet = 100;
    bart->bank = 0;
    bart->is_active = true;
    bart->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, QUEEN, "queenhearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // two pair 
    
    lisa->bet = 100;
    lisa->bank = 0;
    lisa->is_active = true;
    lisa->hand =
    {
        Card(DIAMONDS, TWO, "2diamonds"),
        Card(SPADES, TWO, "2spades"),
        Card(HEARTS, TWO, "2hearts"),
        Card(SPADES, QUEEN, "queenspades"),
        Card(SPADES, TEN, "10spades")
    }; // three kind 
    
    maggie->bet = 1;
    maggie->bank = 99;
    maggie->is_active = true;
    maggie->hand = 
    {
        Card(SPADES, NINE, "9spades"),
        Card(SPADES, KING, "kingspades"),
        Card(SPADES, TWO, "2spades"),
        Card(SPADES, JACK, "jackspades"),
        Card(SPADES, TEN, "10spades")
    }; // flush

    Rank high_rank;
    std::vector<std::string> winners = game.determine_winner(game.player_uuids, high_rank);
    BOOST_CHECK( winners.size() == 1 );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), homer->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), marge->uuid_str ) != winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), bart->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), lisa->uuid_str ) == winners.end() );
    BOOST_CHECK( std::find( winners.begin(), winners.end(), maggie->uuid_str ) == winners.end() );
    BOOST_CHECK( marge->bank == 551 );
    BOOST_CHECK( (homer->bet + marge->bet + bart->bet + lisa->bet + maggie->bet) == 0 );
    BOOST_CHECK( homer->bank == 0 );
    BOOST_CHECK( bart->bank == 0 );
    BOOST_CHECK( lisa->bank == 0 );
    BOOST_CHECK( maggie->bank == 99 );
}
BOOST_AUTO_TEST_SUITE_END()
