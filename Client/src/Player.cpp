#include "Player.hpp"

PLAYER::PLAYER(std::string name, int bank){
    this->name = name;
    this->bank = bank;
}

std::string PLAYER::get_name(){
    return this->name;
}

int PLAYER::get_bank(){
    return this->bank;
}

void PLAYER::set_bank(int bank){
    this->bank = bank;
}