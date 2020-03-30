#include <iostream>
#include <gtkmm.h>
#include <string>
#include <vector>
#include <deque>
#include <thread>

//#include "asio.hpp"
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
        Gtk::Window *window;
    public:
        CLIENT(){
            app = Gtk::Application::create("PokerProject.Team11");
            builder = Gtk::Builder::create_from_file("glade/MainWindow.glade");

            window = nullptr;
            Gtk::Stack *stack = nullptr;
            
            builder->get_widget("Window", window);
            builder->get_widget("Stack", stack);

            Gtk::Entry *entry_name = nullptr;
            Gtk::Entry *entry_ip = nullptr;
            Gtk::Entry *entry_port = nullptr;
            Gtk::Button *btn_join = nullptr;
            Gtk::Button *btn_spectate = nullptr;

            Gtk::Label *lbl_player_one = nullptr;
            Gtk::Button *btn_leave_game = nullptr;

            builder->get_widget("entry_name", entry_name);
            builder->get_widget("entry_ip", entry_ip);
            builder->get_widget("entry_port", entry_port);
            builder->get_widget("btn_join", btn_join);
            builder->get_widget("btn_spectate", btn_spectate);

            btn_join->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&enter_game), stack));


            builder->get_widget("lbl_player_one", lbl_player_one);
            builder->get_widget("btn_leave_game", btn_leave_game);

            stack->set_visible_child("login_screen");

        }
        Glib::RefPtr<Gtk::Application> get_app(){
            return this->app;
        }
        Gtk::Window * get_window(){
            return this->window;
        }
        static void enter_game(void *stack){
            Gtk::Stack *tempStack = static_cast<Gtk::Stack *>(stack);
            tempStack->set_visible_child("game_screen");
        }
};