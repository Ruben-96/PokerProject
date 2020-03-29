#include <iostream>
#include <string>
#include <vector>

#include "Player.hpp"
#include "Connection.hpp"

class Client{
    private:
        bool inGame;
        PLAYER player;
        CONNECTION connection;
    public:
        Client(){
            inGame = false;
        }
        void create_player(std::string name, int id){
            player = PLAYER(name, id);
        }
        void start_game(){
            inGame = true;
        }
        void end_game(){
            inGame = false;
        }
};