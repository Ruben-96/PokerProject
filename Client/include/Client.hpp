#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <thread>

#include "asio.hpp"
#include "Player.hpp"
#include "Connection.hpp"
#include "chat_message.hpp"

class CLIENT{
    private:
        bool inGame;
        PLAYER player;
        int id;
        CONNECTION connection;
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints;
    public:
        CLIENT(){
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
        void resolve_endpoints(){

        }
        void connect(){
            connection = CONNECTION(io_context, endpoints);
        }
};