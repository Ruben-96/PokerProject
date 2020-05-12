#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <gtkmm.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "Connection.hpp"
#include "UI.hpp"
#include "json.hpp"
#include "chat_message.hpp"
#include "asio.hpp"

using json = nlohmann::json;

UI::UI(){
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
    builder->get_widget("btn_ready", btn_ready);
    builder->get_widget("bx_player_one", bx_player_one);
    builder->get_widget("bx_player_two", bx_player_two);
    builder->get_widget("bx_player_three", bx_player_three);
    builder->get_widget("bx_player_four", bx_player_four);
    builder->get_widget("bx_player_five", bx_player_five);
    playersGame.push_back(bx_player_one);
    playersGame.push_back(bx_player_two);
    playersGame.push_back(bx_player_three);
    playersGame.push_back(bx_player_four);
    playersGame.push_back(bx_player_five);
    bx_player_one->hide();
    bx_player_two->hide();
    bx_player_three->hide();
    bx_player_four->hide();
    bx_player_five->hide();
    //Spectate Screen
    builder->get_widget("btn_leave_spectate", btn_leave_spectate);

    //Login Screen
    btn_join->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&join_game), this));
    btn_spectate->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&spectate_game), this));
    //Game Screen
    btn_raise->hide();
    btn_bet->hide();
    btn_allin->hide();
    btn_call->hide();
    btn_check->hide();
    btn_fold->hide();
    btn_leave_game->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));
    btn_raise->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&raise_), this));
    btn_bet->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&bet), this));
    btn_allin->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&allin), this));
    btn_call->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&call), this));
    btn_check->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&check), this));
    btn_fold->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&fold), this));
    btn_ready->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&ready_up), this));
    //Spectate Screen
    btn_leave_spectate->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));
}
Gtk::Window* UI::get_window(){
    return this->window;
}
//UI Functionality methods
void UI::ready_up(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("join");
    tempUI->btn_raise->show();
    tempUI->btn_bet->show();
    tempUI->btn_allin->show();
    tempUI->btn_call->show();
    tempUI->btn_check->show();
    tempUI->btn_fold->show();
}
void UI::raise_(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("raise");
}
void UI::bet(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("bet");
}
void UI::allin(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("allin");
}
void UI::call(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("call");
}
void UI::check(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("check");
}
void UI::fold(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("fold");
    tempUI->inGame = false;
}
void UI::join_game(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    try{
        tempUI->connect(tempUI->entry_ip->get_text(), tempUI->entry_port->get_text());
        tempUI->name = tempUI->entry_name->get_text().raw();
        tempUI->stack->set_visible_child("game_screen");
        tempUI->send_info();
        tempUI->inGame = true;
        std::cout << "Connected to Server " << tempUI->entry_ip->get_text().raw() << " : " << tempUI->entry_port->get_text().raw() << std::endl;
    } catch(std::exception &e){
        tempUI->lbl_connection_error->set_visible(true);
    }
}
void UI::spectate_game(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    try{
        tempUI->connect(tempUI->entry_ip->get_text(), tempUI->entry_port->get_text());
        tempUI->name = tempUI->entry_name->get_text().raw();
        tempUI->stack->set_visible_child("spectate_screen");
        tempUI->send_info();
    } catch(std::exception &e){
        tempUI->lbl_connection_error->set_visible(true);
    }
}
void UI::update_login_screen(){
    entry_name->set_text("");
    entry_ip->set_text("");
    entry_port->set_text("");
}
void UI::leave_game(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->connection->close();
    tempUI->thread->join();
    tempUI->update_login_screen();
    tempUI->stack->set_visible_child("login_screen");
}
//Connection methods
void UI::send_info(){
    toServer["from"]["name"] = name;
    toServer["from"]["uuid"] = uuid;
    chat_message msg;
    std::string temp = toServer.dump();
    msg.body_length(std::strlen(temp.c_str()));
    std::memcpy(msg.body(), temp.c_str(), msg.body_length());
    msg.encode_header();
    connection->write(msg);
}
void UI::send_move(std::string move){
    toServer["from"]["name"] = name;
    toServer["from"]["uuid"] = uuid;
    toServer["event"] = move;
    chat_message msg;
    std::string temp = toServer.dump();
    msg.body_length(std::strlen(temp.c_str()));
    std::memcpy(msg.body(), temp.c_str(), msg.body_length());
    msg.encode_header();
    connection->write(msg);
}
void UI::update_fromServer(std::string message){
    std::cout << message << std::endl;
    message.erase(0, message.find("{"));
    json fromServer = json::parse(message);
    Gtk::Label label(fromServer.value("dealer_comment", "No Comment"));
    list_chat->append(label);

}
void UI::connect(std::string ip, std::string port){
    io_context = new asio::io_context();
    tcp::resolver resolver(*io_context);
    auto endpoints = resolver.resolve(ip, port);
    this->connection = new CONNECTION(*io_context, endpoints);
    this->thread = new std::thread([this](){io_context->run();});
    this->connection->gui = this;
}
