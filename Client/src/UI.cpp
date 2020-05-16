#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <vector>
#include <gtkmm.h>
#include <algorithm>
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
    window->set_title("Poker++");
    builder->get_widget("Stack", stack);
    builder->get_widget("entry_name", entry_name);
    builder->get_widget("entry_ip", entry_ip);
    builder->get_widget("entry_port", entry_port);
    builder->get_widget("btn_join", btn_join);
    builder->get_widget("btn_spectate", btn_spectate);
    builder->get_widget("lbl_connection_error", lbl_connection_error);
    //Game Screen
    builder->get_widget("lbl_game_phase", lbl_game_phase);
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
    
    builder->get_widget("lbl_draw", lbl_draw);
    builder->get_widget("check_card_1", check_card_1);
    builder->get_widget("check_card_2", check_card_2);
    builder->get_widget("check_card_3", check_card_3);
    builder->get_widget("check_card_4", check_card_4);
    builder->get_widget("check_card_5", check_card_5);
    builder->get_widget("btn_draw", btn_draw);
    
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

    //Winning Screen
    builder->get_widget("btn_leave_winning", btn_leave_winning);
    builder->get_widget("lbl_winner1", lbl_winner1);
    builder->get_widget("lbl_winner2", lbl_winner2);
    builder->get_widget("lbl_winner3", lbl_winner3);
    lbl_winners.push_back(lbl_winner1);
    lbl_winners.push_back(lbl_winner2);
    lbl_winners.push_back(lbl_winner3);
    lbl_winner1->hide();
    lbl_winner2->hide();
    lbl_winner3->hide();


    btn_leave_winning->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));

    //Login Screen
    btn_join->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&join_game), this));
    btn_spectate->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&spectate_game), this));
    //Game Screen
    bet_buttons = {btn_raise, btn_allin, btn_call, btn_check, btn_fold}; 
    for(auto button: bet_buttons) {button->set_sensitive(false);}
    entry_raise->hide();
    lbl_raise->hide();
    btn_confirm_raise->hide();
    

    draw_checkbuttons = {check_card_1, check_card_2, check_card_3, check_card_4, check_card_5};
    draw_buttons = {lbl_draw, check_card_1, check_card_2, check_card_3, check_card_4, check_card_5, btn_draw};
    for(auto button: draw_buttons) {button->hide();}

    btn_leave_game->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&leave_game), this));
    
    btn_raise->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&raise_), this));
    btn_confirm_raise->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&confirm_raise), this));
    
    btn_allin->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&allin), this));
    btn_call->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&call), this));
    btn_check->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&check), this));
    btn_fold->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&fold), this));
    btn_ready->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&ready_up), this));
    
    btn_draw->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&select_discards), this));
    
    //rank_turn_button

    str_to_phase =
    {
        {"pre", PRE}, {"ante & deal", ANTE_DEAL}, {"bet phase #1", BET_ONE}, {"draw phase", DRAW}, 
        {"bet phase #2", BET_TWO}, {"showdown", SHOWDOWN},
    };
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

void UI::select_discards(void *ui)
{
    UI *tempUI = static_cast<UI *>(ui);
    std::vector<bool> card_bool_vec;
    std::vector<std::string> discard_vec;
    for(auto checkbutton: tempUI->draw_checkbuttons)
    {
        card_bool_vec.push_back(checkbutton->get_active());
    }
    for(int i = 0; i < 5; i++)
    {
        if(card_bool_vec.at(i))
        {
            discard_vec.push_back(tempUI->player_hand_vec.at(i));
        }
    }
    tempUI->send_move("request_cards", discard_vec);
}

void UI::raise_(void *ui){
    UI *tempUI = static_cast<UI *>(ui);
    // hide bet phase interface
    for(auto button: tempUI->bet_buttons) { button->hide(); }
    // show raise interface
    tempUI->entry_raise->show();
    tempUI->lbl_raise->show();
    tempUI->btn_confirm_raise->show();
}

void UI::confirm_raise(void *ui)
{
    UI *tempUI = static_cast<UI *>(ui);
    
    std::string raise_str = tempUI->entry_raise->get_text(); // gets text from field
    int current_bet = std::atoi(raise_str.c_str()); // converts to int
    tempUI->entry_raise->set_text(""); // resets field
    if(current_bet > 0 && current_bet < tempUI->bank)
    {
        // hide raise interface
        tempUI->entry_raise->hide();
        tempUI->lbl_raise->hide();
        tempUI->btn_confirm_raise->hide();
        // show bet phase interface
        for(auto button: tempUI->bet_buttons) { button->show(); }
        
        tempUI->send_move("raise", current_bet); // sends entered data to sending function
    }
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

void UI::send_move(std::string move_str, std::vector<std::string> discard_vec)
{
    toServer["from"]["name"] = name;
    toServer["from"]["uuid"] = uuid;
    toServer["event"] = move_str;
    json cards_requested(json::value_t::array);
    for(auto card: discard_vec) {cards_requested.push_back(card);}
    toServer["cards_requested"] = cards_requested;

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
    json fromServer = json::parse(message);
    std::cout << fromServer.dump(2) << std::endl;
    int i = 0;
    lbl_game_phase->set_label(fromServer.value("phase", "PreGame"));
    for(auto& element : fromServer["hand"]){
        playersNameGame.at(i)->set_label(element.value("name", "No Name"));
        playersGame.at(i)->show();
        if(element.value("uuid", "NULL") == uuid){
            lbl_player_bank->set_label(std::to_string(element.value("final_bank", 0000)));
            player_hand_vec = element["cards"].get<std::vector<std::string>>();
            std::reverse(player_hand_vec.begin(), player_hand_vec.end());
            bank = element.value("final_bank",0);
            int j = 0;
            for(auto& card : element["cards"]){
                std::string url = "./img/cards_150/" + card.get<std::string>() + ".jpg";
                playersCards.at(j)->set(url);
                j++;
            }
        }
        i++;
    }
    Game_phase phase = str_to_phase.at(fromServer.value("phase", "pre"));
    bool is_turn = (fromServer.value("turn", "NULL") == uuid);
    int k = 0;
    switch(phase)
    {
        case BET_TWO:
            btn_check->hide();
        case BET_ONE:
            for(auto button: draw_buttons) { button->hide(); }
            for(auto button: bet_buttons)
            {
                button->show();
                button->set_sensitive(is_turn);
            }
            break;
        case DRAW:
            for(auto button: draw_buttons) { button->show(); }
            for(auto button: draw_checkbuttons) { button->set_sensitive(is_turn); }
            btn_draw->set_sensitive(is_turn);
            for(auto button: bet_buttons) { button->hide(); }
            for(auto card: player_hand_vec) {std::cout << card << std::endl;}
            break;
        case SHOWDOWN:
            for(auto &winner: fromServer["winners"]){
                lbl_winners.at(k)->set_label(winner.value("name", "NULL"));
                lbl_winners.at(k)->show();
                k++;
                if(k > 2) break;
            }
            stack->set_visible_child("winning_screen");
            break;
        default:
            break;
    }
    std::string commentText = fromServer.value("dealer_comment", "NULL");
    if(commentText != "NULL"){
        Gtk::Label *label = Gtk::manage(new Gtk::Label());
        label->set_text(commentText);
        list_chat->insert(*label, -1);
        list_chat->show_all();
    }
}
void UI::connect(std::string ip, std::string port){
    io_context = new asio::io_context();
    tcp::resolver resolver(*io_context);
    auto endpoints = resolver.resolve(ip, port);
    this->connection = new CONNECTION(*io_context, endpoints);
    this->thread = new std::thread([this](){io_context->run();});
    this->connection->gui = this;
}