#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <vector>
#include <gtkmm.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <unordered_map>
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

    builder->get_widget("entry_raise", entry_raise);
    builder->get_widget("lbl_raise", lbl_raise);
    builder->get_widget("btn_confirm_raise", btn_confirm_raise);
    playersGame.push_back(bx_player_one);
    playersGame.push_back(bx_player_two);
    playersGame.push_back(bx_player_three);
    playersGame.push_back(bx_player_four);
    playersGame.push_back(bx_player_five);
    playersNameGame.push_back(lbl_player_one);
    playersNameGame.push_back(lbl_player_two);
    playersNameGame.push_back(lbl_player_three);
    playersNameGame.push_back(lbl_player_four);
    playersNameGame.push_back(lbl_player_five);
    playersCards.push_back(img_card_one);
    playersCards.push_back(img_card_two);
    playersCards.push_back(img_card_three);
    playersCards.push_back(img_card_four);
    playersCards.push_back(img_card_five);
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
    btn_raise->set_sensitive(false);
    btn_bet->set_sensitive(false);
    btn_allin->set_sensitive(false);
    btn_call->set_sensitive(false);
    btn_check->set_sensitive(false);
    btn_fold->set_sensitive(false);

    entry_raise->hide();
    lbl_raise->hide();
    btn_confirm_raise->hide();
    
    btn_leave_game->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));
    
    btn_raise->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&raise_), this));
    btn_confirm_raise->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&confirm_raise), this));
    
    btn_bet->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&bet), this));
    btn_allin->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&allin), this));
    btn_call->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&call), this));
    btn_check->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&check), this));
    btn_fold->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&fold), this));
    btn_ready->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&ready_up), this));
    
    //Spectate Screen
    btn_leave_spectate->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));
}
UI::~UI(){
    for(unsigned int i = 0; i < playersCards.size(); i++){
        playersCards.at(i)->clear();
    }
}
Gtk::Window* UI::get_window(){
    return this->window;
}
//UI Functionality Methods
void UI::ready_up(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->btn_ready->set_sensitive(false);
    tempUI->send_move("join");
}
// void UI::select_card_one(void *ui){
//     UI *tempUI = static_cast<UI *>(ui);
    
// }
// void UI::select_card_two(void *ui){
//     UI *tempUI = static_cast<UI *>(ui);
    
// }
// void UI::select_card_three(void *ui){
//     UI *tempUI = static_cast<UI *>(ui);
    
// }
// void UI::select_card_four(void *ui){
//     UI *tempUI = static_cast<UI *>(ui);
    
// }
// void UI::select_card_five(void *ui){
//     UI *tempUI = static_cast<UI *>(ui);
    
// }
void UI::raise_(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    
    tempUI->btn_raise->hide();
    tempUI->btn_bet->hide();
    tempUI->btn_allin->hide();
    tempUI->btn_call->hide();
    tempUI->btn_check->hide();
    tempUI->btn_fold->hide();
    
    tempUI->entry_raise->show();
    tempUI->lbl_raise->show();
    tempUI->btn_confirm_raise->show();
}

void UI::confirm_raise(void *ui)
{
    UI *tempUI = static_cast<UI *>(ui);
    
    std::string raise_str = tempUI->entry_raise->get_text();
    int current_bet = std::atoi(raise_str.c_str());
    tempUI->entry_raise->set_text("");

    // hide raise interface
    tempUI->entry_raise->hide();
    tempUI->lbl_raise->hide();
    tempUI->btn_confirm_raise->hide();
    // show bet phase interface
    tempUI->btn_raise->show();
    tempUI->btn_bet->show();
    tempUI->btn_allin->show();
    tempUI->btn_call->show();
    tempUI->btn_check->show();
    tempUI->btn_fold->show();
    
    tempUI->send_move("raise", current_bet);
}

void UI::bet(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("bet");
}
void UI::allin(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    tempUI->send_move("all in");
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
}
void UI::join_game(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    try{
        tempUI->connect(tempUI->entry_ip->get_text(), tempUI->entry_port->get_text());
        tempUI->name = tempUI->entry_name->get_text().raw();
        tempUI->send_info();
        tempUI->stack->set_visible_child("game_screen");
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
    std::string toServer_str = toServer.dump();
    
    char msg_body[chat_message::max_body_length];
    strcpy(msg_body, toServer_str.c_str());
    
    msg.body_length(std::strlen(toServer_str.c_str()) + 1);
    std::memcpy(msg.body(), msg_body, msg.body_length());
    msg.encode_header();
    
    connection->write(msg);
    
}

void UI::send_move(std::string move_str, int current_bet)
{
    toServer["from"]["name"] = name;
    toServer["from"]["uuid"] = uuid;
    toServer["event"] = move_str;
    toServer["current_bet"] = current_bet;

    chat_message msg;
    std::string toServer_str = toServer.dump();

    char msg_body[chat_message::max_body_length + 1];
    strcpy(msg_body, toServer_str.c_str());

    msg.body_length(std::strlen(toServer_str.c_str()) + 1);
    std::memcpy(msg.body(), msg_body, msg.body_length());
    msg.encode_header();
    
    connection->write(msg);
}

void UI::send_move(std::string move_str){
    toServer["from"]["name"] = name;
    toServer["from"]["uuid"] = uuid;
    toServer["event"] = move_str;
    /*
    std::unordered_map<std::string, Moves> moves_from_str
    {
        {"check", CHECK}, {"call", CALL}, {"raise", RAISE}, {"fold", FOLD}, {"all in", ALL_IN}, {"request_cards", REQUEST_CARDS}, {"join", READY}
    };
    Moves move = moves_from_str.at(move_str);
    switch(move)
    {
        case READY:
            break;
        case RAISE:
            toServer["current_bet"] = current_bet;
            break;
        case CALL:
            break;
        case CHECK:
            break;
        case FOLD:
            break;
        case ALL_IN:
            break;
        case REQUEST_CARDS:
            break;
        default:
            break;
    }
    */
    chat_message msg;
    std::string toServer_str = toServer.dump();

    char msg_body[chat_message::max_body_length + 1];
    strcpy(msg_body, toServer_str.c_str());

    msg.body_length(std::strlen(toServer_str.c_str()) + 1);
    std::memcpy(msg.body(), msg_body, msg.body_length());
    msg.encode_header();
    
    connection->write(msg);
}
void UI::update_fromServer(std::string message){
    std::cout << message << std::endl;
    //message.erase(0, message.find("{"));
    //std::cout << "??? " << message << std::endl;
    json fromServer = json::parse(message);
    std::cout << fromServer.dump(2) << std::endl;
    int i = 0;
    for(auto& element : fromServer["hand"]){
        playersNameGame.at(i)->set_label(element.value("name", "No Name"));
        playersGame.at(i)->show();
        if(element.value("uuid", "NULL") == uuid){
            lbl_player_bank->set_label(std::to_string(element.value("final_bank", 0000)));
            int j = 0;
            for(auto& card : element["cards"]){
                std::string url = "./img/cards_150/" + card.get<std::string>() + ".jpg";
                std::cout << url << std::endl; // card didn't load, trying to replicate, DEBUG
                playersCards.at(j)->set(url);
                // if(fromServer.value("phase", "NULL") == "draw phase"){
                //     playersCards.at(j)->set_sensitive(true);
                // } else{
                //     playersCards.at(j)->set_sensitive(false);
                // }
                j++;
            }
        }
        i++;
    }
    if(fromServer.value("turn", "NULL") == uuid){
        btn_raise->set_sensitive(true);
        btn_bet->set_sensitive(true);
        btn_allin->set_sensitive(true);
        btn_call->set_sensitive(true);
        btn_check->set_sensitive(true);
        btn_fold->set_sensitive(true);
    } else{
        btn_raise->set_sensitive(false);
        btn_bet->set_sensitive(false);
        btn_allin->set_sensitive(false);
        btn_call->set_sensitive(false);
        btn_check->set_sensitive(false);
        btn_fold->set_sensitive(false);
    }
    Gtk::Label label(fromServer.value("dealer_comment", "No comment."));
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
