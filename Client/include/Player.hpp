#include <vector>
#include <string>

#include "Card.hpp"

class PLAYER{
    private:
        std::string name;
        int playerId;
        std::vector<CARD> hand;
        std::string move;
    public:
        PLAYER(){

        }
        PLAYER(std::string n, int id){
            name = n;
            playerId = id;
            hand = std::vector<CARD>();
        }
        void add_card(CARD card){
            hand.push_back(card);
        }
        CARD remove_card(int card){
            return hand.erase(card);
        }
        void bet(){
            move = "bet";
        }
        void raise(){
            move = "raise";
        }
        void fold(){
            move = "fold";
        }
        void call(){
            move = "call";
        }
};