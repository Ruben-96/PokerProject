#pragma once

#include <string>

enum VALUE {
    ACE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, MAX
};

enum SUITE{
    HEARTS, SPADES, CLUBS, DIAMONDS, MAX
};

std::string valueStr[13] = {"ace", "2", "3", "4", "5", "6", "7", "9", "10", "jack", "queen", "king"};
std::string suiteStr[4] = {"hearts", "spades", "clubs", "diamonds"};

class CARD{
    private:
        int value;
        int suite;
        std::string img;
    public:
        CARD(int value, int suite){
            this->value = value;
            this->suite = suite;
            this->img = valueStr[this->value] + suiteStr[this->suite] + ".jpg";
        }
        int get_value(){
            return this->value;
        }
        int get_suite(){
            return this->value;
        }
        std::string get_image(){
            return this->img;
        }
};