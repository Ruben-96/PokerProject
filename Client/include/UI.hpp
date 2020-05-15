#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <gtkmm.h>
#include <vector>
#include <unordered_map>
#include <algorithm>
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

enum Moves { READY, RAISE, CALL, CHECK, ALL_IN, FOLD, REQUEST_CARDS }; 

enum Game_phase { PRE, ANTE_DEAL, BET_ONE, DRAW, BET_TWO, SHOWDOWN, PHASE_MAX };

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
        std::string name;
        std::string uuid;
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
        std::vector<Gtk::Box *> playersGame;
        std::vector<Gtk::Label *> playersNameGame;
        std::vector<Gtk::Image *> playersCards;
        std::vector<std::string> player_hand_vec;
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
        
        std::vector<Gtk::Button *> bet_buttons;
        Gtk::Button *btn_raise;
        Gtk::Button *btn_allin;
        Gtk::Button *btn_call;
        Gtk::Button *btn_check;
        Gtk::Button *btn_fold;
        
        Gtk::Entry *entry_raise;
        Gtk::Label *lbl_raise;
        Gtk::Button *btn_confirm_raise;

        Gtk::Label *lbl_draw;
        std::vector<Gtk::CheckButton *> draw_checkbuttons;
        std::vector<Gtk::Widget *> draw_buttons;
        Gtk::CheckButton *check_card_1;
        Gtk::CheckButton *check_card_2;
        Gtk::CheckButton *check_card_3;
        Gtk::CheckButton *check_card_4;
        Gtk::CheckButton *check_card_5;
        Gtk::Button *btn_draw;

        
        Gtk::Label *lbl_player_bank;
        Gtk::ListBox *list_chat;
        Gtk::Button *btn_ready;
        Gtk::Box *bx_player_one;
        Gtk::Box *bx_player_two;
        Gtk::Box *bx_player_three;
        Gtk::Box *bx_player_four;
        Gtk::Box *bx_player_five;
        //Spectate Screen
        Gtk::Button *btn_leave_spectate;
        //Game Variables
        std::unordered_map<std::string, Game_phase> str_to_phase;
        int bank;
        UI();
        ~UI();
        Gtk::Window* get_window();
        void send_info();
        void send_move(std::string move);
        void send_move(std::string move, int current_bet);
        void send_move(std::string move, std::vector<std::string> discard_vec);
        void update_fromServer(std::string msg);
        static void raise_(void *ui);
        static void confirm_raise(void *ui);
        
        static void bet(void *ui);
        static void allin(void *ui);
        static void call(void *ui);
        static void check(void *ui);
        static void fold(void *ui);
        static void join_game(void *ui);
        static void select_discards(void *ui);
        void connect(std::string ip, std::string port);
        void update_game_screen(json);
        void update_spectate_screen(json);
        void update_login_screen();
        static void spectate_game(void *ui);
        static void leave_game(void *ui);
        static void ready_up(void *ui);
        static void select_card_one(void *ui);
        static void select_card_two(void *ui);
        static void select_card_three(void *ui);
        static void select_card_four(void *ui);
        static void select_card_five(void *ui);
};
