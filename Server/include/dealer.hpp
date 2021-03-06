#ifndef DEALER_H
#define DEALER_H

#include <unordered_set>
#include <functional>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <cassert>
#include <sstream>

#include "json.hpp"

using json = nlohmann::json;

//----------------------------------------------------------------------

enum Suit { SPADES, DIAMONDS, HEARTS, CLUBS, SUIT_MAX };

enum Card_rank { TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE, CARD_RANK_MAX };

enum Rank { ROYAL_FLUSH, STRAIGHT_FLUSH, FOUR_KIND, FULL_HOUSE, FLUSH, STRAIGHT, THREE_KIND, TWO_PAIR, PAIR, HIGH_CARD, HAND_RANK_MAX };

enum Hand { HAND_SIZE_MAX = 5 };

enum Game_phase { PRE, ANTE_DEAL, BET_ONE, DRAW, BET_TWO, SHOWDOWN, PHASE_MAX };

enum Bet_options { CHECK, CALL, RAISE, FOLD, ALL_IN, BET_OPTION_MAX };

enum Player_count { PLAYER_COUNT_MIN = 2, PLAYER_COUNT_MAX = 5 };

//----------------------------------------------------------------------

struct Card
{
    Card( Suit suit_, Card_rank rank_, std::string str_ ) : suit(suit_), rank(rank_), str(str_)
    {
    }
    Card( const Card &card ) : suit(card.suit), rank(card.rank), str(card.str)
    {
    }
    Suit suit;
    Card_rank rank;
    std::string str;
    bool operator==(const Card& rhs)
    {
        return ((suit == rhs.suit) && (rank == rhs.rank));
    }
};

class Player
{
public:
    Player( std::string uuid_str_, std::string name_ ) 
        : uuid_str(uuid_str_), name(name_)
        {
            bank = 1000;
            is_active = false;
            is_all_in = false;
            bet = 0;
            current_bet = 0;
        }
    std::string uuid_str;
    std::string name;
    int bank;
    bool is_active;
    bool is_all_in;
    int bet;
    int current_bet;
    std::vector<Card> hand;
};

//----------------------------------------------------------------------

class Game
{
public:
    void deal_to_hand(std::vector<Card>& deck, std::vector<Card>& hand)
    {
        while(hand.size() < HAND_SIZE_MAX) // deal to hand
        {
            hand.push_back(deck.back());
            deck.pop_back();
        }
        return;
    }
    
    std::vector<Card> get_new_deck()
    {
        std::vector<Card> deck;
        for(int i = 0; i < SUIT_MAX; i++) // fill deck in order
        {
            for(int j = 0; j < CARD_RANK_MAX; j++)
            {
                std::stringstream ss;
                Suit suit = static_cast<Suit>(i);
                Card_rank card_rank = static_cast<Card_rank>(j);
                ss << card_rank_to_str.at(card_rank) << suit_to_str.at(suit);
                Card card = { suit, card_rank, ss.str() };
                deck.push_back(card);
            }
        }
        std::mt19937 random_gen(static_cast<uint32_t>(time(0))); // random generator
        std::shuffle( deck.begin(), deck.end(), random_gen ); // shuffle deck using random generator
        return deck; 
    }

    void replace_cards_from_hand(std::vector<Card>& deck, std::vector<Card>& hand, std::vector<Card>& cards_to_replace)
    {
        for(auto& card: cards_to_replace)
        {
            hand.erase(std::remove(hand.begin(), hand.end(), card), hand.end()); // deletes matching card from hand
        }
        deal_to_hand(deck, hand); // fills hand back to HAND_SIZE_MAX
        return;
    }

    enum Rank score_hand(std::vector<Card>& hand) // returns rank of hand
    {
        std::unordered_map< Rank, std::function< bool(std::vector<Card>&) >, std::hash<int> > hand_score_table
        {
            {ROYAL_FLUSH, [](std::vector<Card>& hand)->bool // royal flush
                {
                    std::unordered_set<int> suit_set, rank_set;
                    std::unordered_set<int> royal_flush_ranks {8, 9, 10, 11, 12}; // checks for the specific ranks
                    for(auto card: hand){ suit_set.insert(card.suit); rank_set.insert(card.rank);};
                    return ( ( suit_set.size() == 1 ) && ( rank_set == royal_flush_ranks ) ); // and that only one suit is present
                }
            },
            {STRAIGHT_FLUSH, [](std::vector<Card>& hand)->bool // straight flush
                {
                    std::unordered_set<int> suit_set;
                    for(auto card: hand){ suit_set.insert(card.suit); }
                    for(unsigned int i = 0; i < (hand.size() - 1); i++) { if ((hand[i].rank - hand[i+1].rank) != 1) { return false; } };
                    return suit_set.size() == 1;
                }
            },
            {FOUR_KIND, [](std::vector<Card>& hand)->bool // four of a kind
                {
                    std::unordered_map<Card_rank, int, std::hash<int> > card_count;
                    for(auto card: hand) { card_count[card.rank]++; }; // counts the number of cards in each rank in the hand
                    for(auto& kv: card_count) { if(kv.second == 4) return true; }; // if one rank has four cards found, return true
                    return false;
                }
            }, 
            {FULL_HOUSE, [](std::vector<Card>& hand)->bool // full house
                {
                    std::unordered_set<int> rank_set;
                    for(auto card: hand) { rank_set.insert(card.rank); };
                    return rank_set.size() == 2; // returns true/false whether or not only two ranks are found
                    // four of a kind would pass this test, but will be filtered
                }
            }, 
            {FLUSH, [](std::vector<Card>& hand)->bool // flush
                {
                    std::unordered_set<int> suit_set;
                    for(auto card: hand) { suit_set.insert(card.suit); };
                    return suit_set.size() == 1; // returns whether or not all cards are of the same suit
                    // like similar tests above and below, other hands can satisfy this condition but they will not reach this test
                }
            }, 
            {STRAIGHT, [](std::vector<Card>& hand)->bool // straight
                {
                    for(unsigned int i = 0; i < (hand.size() - 1); i++) { if ((hand[i].rank - hand[i+1].rank) != 1) { return false; }; };
                    return true; // returns false if card ranks are found to be non-consecutive in order
                }
            },
            {THREE_KIND, [](std::vector<Card>& hand)->bool // three of a kind
                {
                    std::unordered_map<Card_rank, int, std::hash<int> > card_count;
                    for(auto card: hand) { card_count[card.rank]++; }; // counts the card of each rank
                    for(auto& kv: card_count) { if(kv.second == 3) return true; }; // returns true if found any rank found 3 times
                    return false;
                }
            }, 
            {TWO_PAIR, [](std::vector<Card>& hand)->bool // two pair
                {
                    std::unordered_set<int> rank_set;
                    for(auto card: hand) { rank_set.insert(card.rank); }; // looks for rank
                    return rank_set.size() == 3; // returns true if only three unique ranks present in hand
                }
            },
            {PAIR, [](std::vector<Card>& hand)->bool
                {
                    std::unordered_set<int> rank_set;
                    for(auto card: hand) { rank_set.insert(card.rank); };
                    return rank_set.size() == 4; // returns true if only four unique ranks present in hand
                }
            },
            {HIGH_CARD, [](std::vector<Card>& hand)->bool{return true;}}, 
        };
        
        std::sort(hand.begin(), hand.end(), [](const Card &a, const Card &b)->bool{return a.rank > b.rank;});
        
        for(int i = 0; i < HAND_RANK_MAX; i++)
        {
            if(hand_score_table[static_cast<Rank>(i)](hand)==true)
            {
                return static_cast<Rank>(i);
            }
        }

    return HAND_RANK_MAX;
    }

    std::vector<std::string> determine_winner(std::vector<std::string> player_uuids, Rank& high_hand)
    {
        std::unordered_map< Rank, std::vector<std::string>, std::hash<int> > rank_to_player_map;
        for(int i = 0; i < HAND_RANK_MAX; i++)
        {
            rank_to_player_map.insert({static_cast<Rank>(i), {}});
        }

        std::unordered_map<std::string, Card> player_to_high_card_map;
        for(auto player_uuid: player_uuids)
        {
            Player* player = &player_lookup_umap.at(player_uuid);
            if(player->is_active)
            {
                enum Rank hand_score = score_hand( player->hand ); // scores player hand
                Card high_card = player->hand.at(0); // cards will have been sorted during scoring; highest card will be first
                rank_to_player_map.at(hand_score).push_back(player_uuid); // create a map of hand_scores to player uuids
                player_to_high_card_map.insert({ player_uuid, high_card }); // create a map of player uuids to their hands' high cards
            }
        }
        
        for(int i = 0; i < HAND_RANK_MAX; i++) // checks all hands from highest to lowest
        {
            high_hand = static_cast<Rank>(i);
            if( rank_to_player_map.at(high_hand).size() > 0 ) break;
        }

        std::vector<std::string> winners; // create a vector of winner uuids, it is a vector to account for the rare case of a tie

        if(rank_to_player_map.at(high_hand).size() > 1) // if there is more than one player with the highest rank, attempt to tie break
        {
            std::unordered_map<Card_rank, std::vector<std::string>, std::hash<int> > high_card_vec_map; // maps high cards to vector of uuids
            
            for(int i = 0; i < CARD_RANK_MAX; i++)
            {
                high_card_vec_map.insert({static_cast<Card_rank>(i), {}});
            }

            for(auto player_uuid: rank_to_player_map.at(high_hand))
            {
                Card_rank player_high_card_rank;
                std::vector<Card> hand_copy = player_lookup_umap.at(player_uuid).hand;
                if(high_hand != HIGH_CARD)
                {
                    while(score_hand(hand_copy) == high_hand)
                    {
                        player_high_card_rank = hand_copy.at(0).rank;
                        hand_copy.erase(hand_copy.begin());
                    }
                }
                else
                {
                    player_high_card_rank = hand_copy.at(0).rank;
                }
                high_card_vec_map.at(player_high_card_rank).push_back(player_uuid);
            }

            for(int i = CARD_RANK_MAX - 1; i >= 0; i--) // iterates from highest card down
            {
                if(high_card_vec_map.at(static_cast<Card_rank>(i)).size() > 0) // when it finds the highest card
                {
                    winners = high_card_vec_map.at(static_cast<Card_rank>(i)); // winners vector assigned the list of players there
                    break;
                }
            }
        }
        else if(rank_to_player_map.at(high_hand).size() == 1) // if there's only one winner
        {
            winners = rank_to_player_map.at(high_hand); // no tie break necessary, add them to the end
        }

        int sum_of_bets = 0;
        for(auto player_uuid: player_uuids)
        {
            Player* player = &player_lookup_umap.at(player_uuid);
            sum_of_bets+=player->bet;
            player->bet = 0;
            player->current_bet = 0;
        }

        int winnings_per_player = sum_of_bets / static_cast<int>(winners.size());
        for(auto player_uuid: winners)
        {
            Player* player = &player_lookup_umap.at(player_uuid);
            player->bank += winnings_per_player;
        }
        
        return winners;
    }

    bool advance_turn() // advances turn to next eligible player
    {
        bool end_passed = false;
        bool last_player_folded = !player_lookup_umap.at(*turn_iter).is_active;
        
        if(turn_iter != player_uuids.end())
        {
            advance(turn_iter, 1);
        }
        else
        {
            end_passed = true;
            turn_iter = player_uuids.begin();
        }
        
        if(phase == BET_ONE || phase == BET_TWO) // skips over players who are folded or all in
        {
            while( turn_iter == player_uuids.end() || (!player_lookup_umap.at(*turn_iter).is_active || player_lookup_umap.at(*turn_iter).is_all_in))
            {
                if(turn_iter == player_uuids.end())
                {
                    end_passed = true;
                    turn_iter = player_uuids.begin();
                }
                else
                {
                    advance(turn_iter, 1);
                }
            }
        }
        else // skips over players who are folded
        {
            while(turn_iter == player_uuids.end() || !player_lookup_umap.at(*turn_iter).is_active)
            {
                if(turn_iter == player_uuids.end())
                {
                    end_passed = true;
                    turn_iter = player_uuids.begin();
                }
                else
                {
                    advance(turn_iter, 1);
                }
            }
        }
        
        if(last_player_folded)
        {
            int active = 0;
            for(auto player_uuid: player_uuids)
            {
                active += player_lookup_umap.at(player_uuid).is_active;
            }
            if(active == 1) // if only one active player remains, ends the betting round
            {
                end_passed = true;
            }
        }
        
        return end_passed;
    }

    void bet_transactions(Player* player, Bet_options bet_option, json &to_dealer, json &to_players)
    {
        switch(bet_option)
        {
            case RAISE:
                {
                    int current_bet = to_dealer["current_bet"].get<int>();
                    if(current_bet >= player->bank)
                    {
                        current_bet = player->bank;
                        player->is_all_in = true;
                    }
                    player->current_bet = current_bet;
                    player->bet += current_bet;
                    player->bank -= current_bet;
                    if(player->bet > max_bet)
                    {
                        max_bet = player->bet;
                    }
                    std::stringstream comment;
                    comment << "Player " << player->name << " has raised " << current_bet << " for a total of " << max_bet << ".";
                    to_players["dealer_comment"] = comment.str();
                }
                break;
            case ALL_IN:
                {
                    player->current_bet = player->bank;
                    player->bank = 0;
                    player->bet += player->current_bet;
                    player->is_all_in = true;
                    
                    std::stringstream comment;
                    comment << "Player " << player->name << " has gone all-in.";
                    to_players["dealer_comment"] = comment.str();
                    break;
                }
            case CHECK:
                if(phase == BET_ONE)
                {
                    std::stringstream comment;
                    comment << "Player " << player->name << " has checked." << std::endl;
                    break;
                }
                else if(phase == BET_TWO)
                {
                    player->is_active = false;
                    to_players["dealer_comment"] = "Player folded";
                    break;
                }
            case CALL:
                {
                    int needed_bet = max_bet - player->bet;
                    if(needed_bet < player->bank)
                    {
                        std::stringstream comment;
                        comment << "Player " << player->name << " has called.";
                        to_players["dealer_comment"] = comment.str();

                        player->current_bet = needed_bet;
                    }
                    else
                    {
                        std::stringstream comment;
                        comment << "Player " << player->name << " has gone all in.";
                        to_players["dealer_comment"] = comment.str();

                        player->is_all_in = true;
                        player->current_bet = player->bank;
                    }
                    player->bank -= player->current_bet;
                    player->bet += player->current_bet;
                }
                break;
            case FOLD:
                {
                    player->is_active = false;

                    std::stringstream comment;
                    comment << "Player " << player-> name << " has folded.";
                    to_players["dealer_comment"] = comment.str();
                }
                break;
            default:
                break;
        }
        return;
    }

    chat_message event_handler(std::string input_string)
    {
        // parses input string, updates
        json to_dealer = json::parse(input_string);
        std::cout << to_dealer.dump(2) << std::endl;

        json to_players;
        // according to the game phase, modify the game state, and record the new game state in a json object
        Game_phase phase_now = phase;
        if(phase == BET_ONE || phase == DRAW || phase == BET_TWO)
        {
            if(*turn_iter != to_dealer["from"]["uuid"])
            {
                phase_now = PHASE_MAX;
            }
        }
        switch(phase_now)
        {

            case PRE:
                {
                    std::string uuid_str = to_dealer["from"]["uuid"].get<std::string>(); // read name and uuid from json
                    std::string name = to_dealer["from"]["name"].get<std::string>();
                    
                    if(std::find(player_uuids.begin(), player_uuids.end(), uuid_str) == player_uuids.end()) // if uuid not recognized
                    {
                        if(player_uuids.size() < PLAYER_COUNT_MAX)
                        {
                            player_uuids.push_back( uuid_str ); // add player id to id list
                            player_lookup_umap.insert({uuid_str, Player(uuid_str, name)}); // create player in player list

                            std::stringstream comment;
                            comment << "Player " << name << " has joined the lobby.";
                            to_players["dealer_comment"] = comment.str();
                        }
                        else
                        {
                            std::stringstream comment;
                            comment << "Player " << name
                                << " attempted to join after the maximum number of players had been reached.";
                            to_players["dealer_comment"] = comment.str();
                        }
                    }
                    if(to_dealer.find("event") != to_dealer.end())
                    {
                        std::string player_event = to_dealer["event"].get<std::string>();
                        if(player_event == "join")
                        {
                            player_lookup_umap.at(uuid_str).is_active = true; // join = ready flag
                            std::stringstream comment;
                            comment << "Player " << name << " has readied up.";
                            to_players["dealer_comment"] = comment.str();
                        }
                    }
                    if(player_uuids.size() >= PLAYER_COUNT_MIN && player_uuids.size() <= PLAYER_COUNT_MAX)
                    {
                        bool all_ready = true; // checks if all recognized players are ready
                        for(auto player_uuid: player_uuids)
                        {
                            if (!player_lookup_umap.at(player_uuid).is_active) all_ready = false; 
                        };
                        if(all_ready) 
                        {
                            turn_iter = player_uuids.begin();
                            std::string comment_b = "\nThe game has begun.";
                            std::string comment_a = to_players["dealer_comment"].get<std::string>() + comment_b;
                            to_players["dealer_comment"] = comment_a;
                            phase = ANTE_DEAL;
                        }
                    }
                }
                if(phase != ANTE_DEAL) {break;} // if the phase changes, proceed to execute the ante_deal case

            case ANTE_DEAL:
                {
                    for(auto& player_uuid: player_uuids)
                    {
                        Player* player = &player_lookup_umap.at(player_uuid);
                        player->bank = player->bank - ante;
                        player->bet = player->bet + ante;
                        deal_to_hand( deck, player->hand );
                    }
                    phase = BET_ONE;
                    max_bet = ante;
                }
                break;

            case BET_ONE:
                {
                    std::string current_player_uuid = to_dealer["from"]["uuid"].get<std::string>();
                    
                    Player* player = &player_lookup_umap.at(current_player_uuid);

                    std::string player_event = to_dealer["event"].get<std::string>();
                    Bet_options bet_option = bet_option_from_str.at(player_event);
                    bet_transactions(player, bet_option, to_dealer, to_players);
                    
                    if(bet_option == RAISE) { was_raise_in_round = true; };
                    
                    bool bet_round_ended = advance_turn();
                    

                    if(bet_round_ended)
                    {
                        int active_count = 0;
                        for(auto player_uuid: player_uuids)
                        {
                            active_count += player_lookup_umap.at(player_uuid).is_active;
                        }
                        // assess whether all active players have the same bet; if so, proceed to next phase DRAW
                        if(!was_raise_in_round && active_count > 1)
                        {
                            phase = DRAW;
                        }
                        else if(active_count == 1) // if only one player remains active, go to showdown
                        {
                            phase = SHOWDOWN;
                        }
                        was_raise_in_round = false;
                    }
                }
                if(phase != SHOWDOWN)
                {
                    break;
                }

            case DRAW: // players send a list of cards to be replaced; those are removed from their hands and new cards are dealt; one round
                if (phase!= SHOWDOWN)
                {
                    Player* player = &player_lookup_umap.at(*turn_iter);
                    // parse cards requested
                    json discard_array = to_dealer["cards_requested"];
                    std::vector<Card> discard_pile;
                    for(auto discard_json: discard_array) // since there's no char like '_' to split the string
                    {
                        std::string card_str = discard_json.get<std::string>();
                        for(auto& kv1: card_rank_from_str)
                        {
                            std::string rank_str = kv1.first;
                            Card_rank card_rank = kv1.second;

                            if(card_str.find(rank_str) != std::string::npos)
                            {
                                for(auto& kv2: suit_from_str)
                                {
                                    std::string suit_str = kv2.first;
                                    Suit suit = kv2.second;
                                    if(card_str.find(suit_str) != std::string::npos)
                                    {
                                        discard_pile.push_back(Card( suit, card_rank, card_str ));
                                    }
                                }
                            }
                        }
                    }
                    replace_cards_from_hand(deck, player->hand, discard_pile); // execute card replacement

                    bool round_ended = advance_turn();
                    if(round_ended) phase = BET_TWO;
                    break;
                }

            case BET_TWO: // same as above but no 0 bets are allowed
                if(phase != SHOWDOWN)
                {
                    std::string current_player_uuid = to_dealer["from"]["uuid"].get<std::string>();
                    
                    Player* player = &player_lookup_umap.at(current_player_uuid);

                    std::string player_event = to_dealer["event"].get<std::string>();
                    Bet_options bet_option = bet_option_from_str.at(player_event);
                    bet_transactions(player, bet_option, to_dealer, to_players);

                    if(bet_option == RAISE || bet_option == ALL_IN) { was_raise_in_round = true; };
                    
                    bool bet_round_ended = advance_turn();
                    if(bet_round_ended)
                    {
                        int active_count = 0;
                        for(auto player_uuid: player_uuids)
                        {
                            active_count += player_lookup_umap.at(player_uuid).is_active;
                        }
                        // assess whether all active players have the same bet; if so, proceed to next phase DRAW
                        if((!was_raise_in_round && active_count > 1) || active_count == 1)
                        {
                            phase = SHOWDOWN;
                        }
                        was_raise_in_round = false;
                    }
                }
                if(phase != SHOWDOWN)
                {
                    break;
                }

            case SHOWDOWN: 
                {
                    Rank winning_rank;
                    std::vector<std::string> winner_uuids = determine_winner(player_uuids, winning_rank);
                    std::vector<std::string> winner_names;
                    json winner_uuids_json(json::value_t::array);
                    for(auto winner_uuid: winner_uuids)
                    {
                        std::string winner_name = player_lookup_umap.at(winner_uuid).name;
                        json winner;
                        winner["uuid"] = winner_uuid;
                        winner["name"] = winner_name;
                        winner_uuids_json.push_back(winner);
                        winner_names.push_back(winner_name);
                    }
                    to_players["winners"] = winner_uuids_json;
                    std::stringstream comment;
                    comment << "Winner(s): ";
                    for(auto name: winner_names) { comment << name; }
                    comment << ", with a " << rank_to_str.at(winning_rank); 
                    to_players["dealer_comment"] = comment.str();
                    break;
                }
                break;

            default:
                break;
        }
        if(phase != PRE)
        {
            to_players["turn"] = *turn_iter;
            to_players["phase"] = phase_to_str.at(phase);
            // update general state information
            for(auto player_uuid: player_uuids)
            {
                Player* player = &player_lookup_umap.at(player_uuid);
                json dealt_hand;
                dealt_hand["bet"] = player->bet;
                dealt_hand["current_bet"] = player->current_bet;
                dealt_hand["uuid"] = player->uuid_str;
                dealt_hand["name"] = player->name;
                dealt_hand["final_bank"] = player->bank;
                dealt_hand["is_active"] = player->is_active;
                dealt_hand["is_all_in"] = player->is_all_in;
                json hand_json(json::value_t::array);
                for(auto card: player->hand)
                {
                    hand_json.push_back(card.str);
                }
                dealt_hand["cards"] = hand_json;
                to_players["hand"].push_back(dealt_hand);
            }
        }

        std::cout << to_players.dump(4) << std::endl; // pretty print to_players
        
        // take the json dump of the game state, copy it, and return it to be delivered by the server
        std::string to_players_str = to_players.dump();
        
        // create & encode message to be sent back by server containing the game state json string
        chat_message r_msg;
        char msg_body[chat_message::max_body_length + 1];
        strcpy(msg_body, to_players_str.c_str());
        r_msg.body_length(std::strlen(msg_body)+1);
        std::memcpy(r_msg.body(), msg_body, r_msg.body_length());
        r_msg.encode_header();
        return r_msg;
    }
    void showdown_end_game();
    Game()
    {
        phase = PRE;
        deck = get_new_deck();
    }

    enum Game_phase phase;
    std::unordered_map<std::string, Player> player_lookup_umap; // container for the player objects identified by uuid
    std::vector<std::string> player_uuids; // contains the string of player uuids, the order in this vector determines the order of turns
    std::vector<std::string>::iterator turn_iter; // keeps track of whose turn it is
    int max_bet; // records current highest bet for betting logic
    std::vector<Card> deck; // deck of cards
    int ante = 100;
    int was_raise_in_round = false;
    // maps to get string representations from enums and vice versa

    std::unordered_map< Rank, std::string, std::hash<int> > rank_to_str
    {
        {ROYAL_FLUSH, "Royal Flush"}, {STRAIGHT_FLUSH, "Straight Flush"}, {FOUR_KIND, "Four of a Kind"}, {FULL_HOUSE, "Full House"}, 
        {FLUSH, "Flush"}, {STRAIGHT, "Straight"}, {THREE_KIND, "Three of a Kind"}, {TWO_PAIR, "Two Pair"}, {PAIR, "Pair"}, {HIGH_CARD, "High Card"}
    };

    std::unordered_map< Suit, std::string, std::hash<int> > suit_to_str
    {
        {SPADES, "spades"}, {DIAMONDS, "diamonds"}, {HEARTS, "hearts"}, {CLUBS, "clubs"},
    };
    std::unordered_map< Card_rank, std::string, std::hash<int> > card_rank_to_str
    {
        {TWO, "2"}, {THREE, "3"}, {FOUR, "4"}, {FIVE, "5"}, {SIX, "6"}, {SEVEN, "7"}, 
        {EIGHT, "8"}, {NINE, "9"}, {TEN, "10"}, {JACK, "jack"}, {QUEEN, "queen"}, {KING, "king"}, {ACE, "ace"},
    };
    std::unordered_map<std::string, Suit> suit_from_str
    {
        {"spades", SPADES}, {"diamonds", DIAMONDS}, {"hearts", HEARTS}, {"clubs", CLUBS},
    };
    std::unordered_map<std::string, Card_rank> card_rank_from_str
    {
        {"2", TWO}, {"3", THREE}, {"4", FOUR}, {"5", FIVE}, {"6", SIX}, {"7", SEVEN}, 
        {"8", EIGHT}, {"9", NINE}, {"10", TEN}, {"jack", JACK}, {"queen", QUEEN}, {"king", KING}, {"ace", ACE},
    };
    std::unordered_map< Bet_options, std::string, std::hash<int> > bet_option_to_str
    {
        {CHECK, "check"}, {CALL, "call"}, {RAISE, "raise"}, {FOLD, "fold"}, {ALL_IN, "all in"},
    };
    std::unordered_map<std::string, Bet_options> bet_option_from_str
    {
        {"check", CHECK}, {"call", CALL}, {"raise", RAISE}, {"fold", FOLD}, {"all in", ALL_IN},
    };
    std::unordered_map< Game_phase, std::string, std::hash<int> > phase_to_str
    {
        {PRE, "pre"}, {ANTE_DEAL, "ante & deal"}, {BET_ONE, "bet phase #1"}, {DRAW, "draw phase"}, 
        {BET_TWO, "bet phase #2"}, {SHOWDOWN, "showdown"},
    };
};

#endif
