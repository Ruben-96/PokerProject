#include <gtkmm.h>
#include <iostream>

#include "../include/Client.hpp"

int main(int argc, char *argv[]){

   CLIENT client;

   client.get_app()->run(*client.get_window());

   delete client.get_window();

   return 0;

}
