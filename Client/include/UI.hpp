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
        //Spectate Screen
        Gtk::Button *btn_leave_spectate;
        //Game Variables

        UI(){
            builder = Gtk::Builder::create_from_file("glade/MainWindow.glade");
            //Connection Variables
            connection = NULL;
            thread = NULL;
            io_context = NULL;
            //Data Variables
            uuid = boost::uuids::to_string(boost::uuids::random_generator()());
            //Login Screen
            builder->get_widget("Window", window);
            builder->get_widget("Stack", stack);
            builder->get_widget("entry_name", entry_name);
            builder->get_widget("entry_ip", entry_ip);
            builder->get_widget("entry_port", entry_port);
            builder->get_widget("btn_join", btn_join);
            builder->get_widget("btn_spectate", btn_spectate);
            builder->get_widget("lbl_connection_error", lbl_connection_error);
            //Game Screen
            builder->get_widget("btn_leave_game", btn_leave_game);
            builder->get_widget("lbl_player_one", lbl_player_one);
            builder->get_widget("lbl_player_two", lbl_player_two);
            builder->get_widget("lbl_player_three", lbl_player_three);
            builder->get_widget("lbl_player_four", lbl_player_four);
            builder->get_widget("lbl_player_five", lbl_player_five);
            builder->get_widget("img_card_one", img_card_one);
            builder->get_widget("img_card_two", img_card_two);
            builder->get_widget("img_card_three", img_card_three);
            builder->get_widget("img_card_four", img_card_four);
            builder->get_widget("img_card_five", img_card_five);
            builder->get_widget("btn_raise", btn_raise);
            builder->get_widget("btn_bet", btn_bet);
            builder->get_widget("btn_allin", btn_allin);
            builder->get_widget("btn_call", btn_call);
            builder->get_widget("btn_check", btn_check);
            builder->get_widget("btn_fold", btn_fold);
            builder->get_widget("lbl_player_bank", lbl_player_bank);
            builder->get_widget("list_chat", list_chat);
            //Spectate Screen
            builder->get_widget("btn_leave_spectate", btn_leave_spectate);

            //Login Screen
            btn_join->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&join_game), this));
            btn_spectate->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&spectate_game), this));
            //Game Screen
            btn_leave_game->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));
            btn_raise->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&raise_), this));
            btn_bet->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&bet), this));
            btn_allin->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&allin), this));
            btn_call->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&call), this));
            btn_check->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&check), this));
            btn_fold->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&fold), this));
            //Spectate Screen
            btn_leave_spectate->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));
        }
        Gtk::Window *get_window(){
            return this->window;
        }
        void send_info(){
            toServer["from"]["name"] = name;
            toServer["from"]["uuid"] = uuid;
            chat_message msg;
            std::string temp = toServer.dump();
            msg.body_length(std::strlen(temp.c_str()));
            std::memcpy(msg.body(), temp.c_str(), msg.body_length());
            msg.encode_header();
            connection->write(msg);
        }
        void send_move(std::string move){
            toServer["from"]["name"] = name;
            toServer["from"]["uuid"] = uuid;
            toServer["chat"] = "";
            toServer["event"] = move;
            chat_message msg;
            std::string temp = toServer.dump();
            msg.body_length(std::strlen(temp.c_str()));
            std::memcpy(msg.body(), temp.c_str(), msg.body_length());
            msg.encode_header();
            connection->write(msg);
        }
        static void update_fromServer(){

        }
        static void raise_(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            tempUI->send_move("raise");
        }
        static void bet(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            tempUI->send_move("bet");
        }
        static void allin(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            tempUI->send_move("allin");
        }
        static void call(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            tempUI->send_move("call");
        }
        static void check(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            tempUI->send_move("check");
        }
        static void fold(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            tempUI->send_move("fold");
            tempUI->inGame = false;
        }
        static void join_game(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            try{
                tempUI->connect(tempUI->entry_ip->get_text(), tempUI->entry_port->get_text());
                tempUI->name = tempUI->entry_name->get_text().raw();
                tempUI->stack->set_visible_child("game_screen");
                tempUI->send_info();
                tempUI->inGame = true;
            } catch(std::exception &e){
                tempUI->lbl_connection_error->set_visible(true);
            }
        }
        void connect(std::string ip, std::string port){
            io_context = new asio::io_context();
            tcp::resolver resolver(*io_context);
            auto endpoints = resolver.resolve(ip, port);
            this->connection = new CONNECTION(*io_context, endpoints);
            this->thread = new std::thread([this](){io_context->run(); update_fromServer();});
        }
        void update_game_screen(){
            
        }
        void update_spectate_screen(){

        }
        void update_login_screen(){
            entry_name->set_text("");
            entry_ip->set_text("");
            entry_port->set_text("");
            lbl_connection_error->set_visible(false);
        }
        static void spectate_game(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            try{
                tempUI->connect(tempUI->entry_ip->get_text(), tempUI->entry_port->get_text());
                tempUI->name = tempUI->entry_name->get_text().raw();
                tempUI->uuid = boost::uuids::to_string(boost::uuids::random_generator()());
                tempUI->toServer["from"]["name"] = tempUI->name;
                tempUI->toServer["from"]["uuid"] = tempUI->uuid;
                tempUI->update_game_screen();
                tempUI->stack->set_visible_child("spectate_screen");
            } catch(std::exception &e){
                tempUI->lbl_connection_error->set_visible(true);
            }
        }
        static void leave_game(void *ui){
            UI *tempUI = static_cast<UI *>(ui);
            tempUI->connection->close();
            tempUI->thread->join();
            tempUI->update_login_screen();
            tempUI->stack->set_visible_child("login_screen");
        }
};