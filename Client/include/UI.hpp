#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <gtkmm.h>
#include "Connection.hpp"
#include "json.hpp"
#include "asio.hpp"
#include "chat_message.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

using asio::ip::tcp;
typedef nlohmann::json json;

class CONNECTION;

class UI{
    private:
        
    public:
        Glib::RefPtr<Gtk::Builder> builder;
        //Connection Variables
        CONNECTION *connection;
        asio::io_context *io_context;
        std::thread *thread;
        //Data Variables
        bool inGame = false;
        std::string name;
        std::string uuid;
        json fromServer;
        json toServer;
        //Login Screen
        Gtk::Window *window;
        Gtk::Stack *stack;
        Gtk::Entry *entry_name;
        Gtk::Entry *entry_ip;
        Gtk::Entry *entry_port;
        Gtk::Button *btn_join;
        Gtk::Button *btn_spectate;
        Gtk::Label *lbl_connection_error;
        //Game Screen
        Gtk::Button *btn_leave_game;
        Gtk::Label *lbl_player_one;
        Gtk::Label *lbl_player_two;
        Gtk::Label *lbl_player_three;
        Gtk::Label *lbl_player_four;
        Gtk::Label *lbl_player_five;
        Gtk::Image *img_card_one;
        Gtk::Image *img_card_two;
        Gtk::Image *img_card_three;
        Gtk::Image *img_card_four;
        Gtk::Image *img_card_five;
        Gtk::Button *btn_raise;
        Gtk::Button *btn_bet;
        Gtk::Button *btn_allin;
        Gtk::Button *btn_call;
        Gtk::Button *btn_check;
        Gtk::Button *btn_fold;
        Gtk::Label *lbl_player_bank;
        Gtk::ListBox *list_chat;
        Gtk::Button *btn_ready;
        //Spectate Screen
        Gtk::Button *btn_leave_spectate;
        //Game Variables

        UI();
        Gtk::Window* get_window();
        void send_info();
        void send_move(std::string move);
        void update_fromServer(chat_message msg);
        static void raise_(void *ui);
        static void bet(void *ui);
        static void allin(void *ui);
        static void call(void *ui);
        static void check(void *ui);
        static void fold(void *ui);
        static void join_game(void *ui);
        void connect(std::string ip, std::string port);
        void update_game_screen(json);
        void update_spectate_screen(json);
        void update_login_screen();
        static void spectate_game(void *ui);
        static void leave_game(void *ui);
        static void ready_up(void *ui);
};