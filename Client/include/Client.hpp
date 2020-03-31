#include <iostream>
#include <gtkmm.h>
#include <string>
#include <vector>
#include <deque>
#include <thread>

#include "asio.hpp"
//#include "Player.hpp"
//#include "Connection.hpp"
//#include "chat_message.hpp"

class CLIENT{
    private:
        bool inGame;
        //PLAYER player;
        //CONNECTION connection;
        Glib::RefPtr<Gtk::Application> app;
        Glib::RefPtr<Gtk::Builder> builder;
    public:
        Gtk::Stack *stack;
        Gtk::Window *window;
        Gtk::Entry *entry_name;
        Gtk::Entry *entry_ip;
        Gtk::Entry *entry_port;
        Gtk::Button *btn_join;
        Gtk::Button *btn_spectate;
        Gtk::Label *lbl_connection_error;
        Gtk::Label *lbl_player_one;
        Gtk::Button *btn_leave_game;


        CLIENT(){
            app = Gtk::Application::create("PokerProject.Team11");
            builder = Gtk::Builder::create_from_file("glade/MainWindow.glade");

            window = nullptr;
            stack = nullptr;
            
            builder->get_widget("Window", window);
            builder->get_widget("Stack", stack);

            entry_name = nullptr;
            entry_ip = nullptr;
            entry_port = nullptr;
            lbl_connection_error = nullptr;
            btn_join = nullptr;
            btn_spectate = nullptr;

            lbl_player_one = nullptr;
            btn_leave_game = nullptr;

            builder->get_widget("entry_name", entry_name);
            builder->get_widget("entry_ip", entry_ip);
            builder->get_widget("entry_port", entry_port);
            builder->get_widget("btn_join", btn_join);
            builder->get_widget("btn_spectate", btn_spectate);
            builder->get_widget("lbl_connection_error", lbl_connection_error);

            builder->get_widget("lbl_player_one", lbl_player_one);
            builder->get_widget("btn_leave_game", btn_leave_game);

            btn_join->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&enter_game), this));
            btn_spectate->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&spectate_game), this));
            btn_leave_game->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));

            stack->set_visible_child("login_screen");

        }
        Glib::RefPtr<Gtk::Application> get_app(){
            return this->app;
        }
        Gtk::Window *get_window(){
            return this->window;
        }
        static void enter_game(void *client){
            CLIENT *tempClient = static_cast<CLIENT *>(client);
            tempClient->stack->set_visible_child("game_screen");
        }
        static void spectate_game(void *client){
            CLIENT *tempClient = static_cast<CLIENT *>(client);
            tempClient->stack->set_visible_child("spectate_screen");
        }
        static void leave_game(void *client){
            CLIENT *tempClient = static_cast<CLIENT *>(client);
            
            tempClient->stack->set_visible_child("login_screen");
        }
};