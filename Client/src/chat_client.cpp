//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <gtkmm.h>
#include "json.hpp"
#include "UI.hpp"
//#include "asio.hpp"

int main(int argc, char* argv[])
{
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create("PokerProject.Team11");
    UI gui;

    app->run(*gui.get_window());

    // char line[chat_message::max_body_length + 1];
    // while (std::cin.getline(line, chat_message::max_body_length + 1))
    // {
    //   chat_message msg;
    //   msg.body_length(std::strlen(line));
    //   std::memcpy(msg.body(), line, msg.body_length());
    //   msg.encode_header();
    //   c.write(msg);
    // }

    delete gui.get_window();

  return 0;
}
