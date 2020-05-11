#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>

#include "chat_message.hpp"
#include "Connection.hpp"
#include "UI.hpp"
#include "asio.hpp"

typedef std::deque<chat_message> chat_message_queue;

CONNECTION::CONNECTION(asio::io_context& io_context, const tcp::resolver::results_type& endpoints) : io_context_(io_context), socket_(io_context){
    do_connect(endpoints);
}

void CONNECTION::write(const chat_message& msg){
    asio::post(io_context_, [this, msg](){
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress){
            do_write();
        }
    });
}

void CONNECTION::close(){
    asio::post(io_context_, [this]() { socket_.close(); });
}

void CONNECTION::do_connect(const tcp::resolver::results_type& endpoints){
    asio::async_connect(socket_, endpoints, [this](std::error_code ec, tcp::endpoint){
        if(!ec){
            do_read_header();
        }
    });
}

void CONNECTION::do_read_header(){
    asio::async_read(socket_, asio::buffer(read_msg_.data(), chat_message::header_length), [this](std::error_code ec, std::size_t /*length*/){
        if(!ec && read_msg_.decode_header()){
            do_read_body();
        } else{
            socket_.close();
        }
    });
}

void CONNECTION::do_read_body(){
    asio::async_read(socket_, asio::buffer(read_msg_.body(), read_msg_.body_length()), [this](std::error_code ec, std::size_t /*length*/){
        if(!ec){
            gui->update_fromServer(read_msg_.body());
            // std::cout.write(read_msg_.body(), read_msg_.body_length());
            // std::cout << "\n";
            // do_read_header();
        } else{
            socket_.close();
        }
    });
}

void CONNECTION::do_write(){
    asio::async_write(socket_, asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()), [this](std::error_code ec, std::size_t /*length*/){
        if (!ec){
            write_msgs_.pop_front();
            if (!write_msgs_.empty()){
                do_write();
            }
        } else{
            socket_.close();
        }
    });
}
