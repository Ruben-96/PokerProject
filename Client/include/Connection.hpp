#pragma once

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "asio.hpp"
#include "chat_message.hpp"
#include "json.hpp"
#include "UI.hpp"

class UI;

typedef std::deque<chat_message> chat_message_queue;
using asio::ip::tcp;

typedef nlohmann::json json;

class CONNECTION{
    private:
        asio::io_context& io_context_;
        tcp::socket socket_;
        chat_message read_msg_;
        chat_message_queue write_msgs_;
        json fromServer;
    public:
        UI *gui;
        CONNECTION(asio::io_context& io_context, const tcp::resolver::results_type& endpoints);
        void write(const chat_message& msg);
        void close();
    private:
        void do_connect(const tcp::resolver::results_type& endpoints);
        void do_read_header();
        void do_read_body();
        void do_write();
};