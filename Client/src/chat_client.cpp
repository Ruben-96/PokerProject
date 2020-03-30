#include <gtkmm.h>
#include <iostream>

// GtkWidget *entry_ip;
// GtkWidget *entry_port;
// GtkWidget *btn_join;
// GtkWidget *btn_spectate;
// GtkWidget *window2;
// GtkWidget *window;
Gtk::Window *window = nullptr;
Gtk::Window *window2 = nullptr;

static void enter_game(Glib::RefPtr<Gtk::Application> app){
   window->hide();
   app->run(*window2);
}

int main(int argc, char *argv[]){

   auto app = Gtk::Application::create(argc, argv, "PokerProject.Team11");

   Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file("glade/MainWindow.glade");
   builder->get_widget("MainWindow", window);
   builder->get_widget("GameWindow", window2);
   Gtk::Entry *entry_ip = nullptr;
   builder->get_widget("entry_ip", entry_ip);
   Gtk::Entry *entry_port = nullptr;
   builder->get_widget("entry_port", entry_port);
   Gtk::Button *btn_join = nullptr;
   builder->get_widget("btn_join", btn_join);
   Gtk::Button *btn_spectate = nullptr;
   builder->get_widget("btn_spectate", btn_spectate);

   //btn_join->signal_clicked().connect(sigc::bind(sigc::ptr_fun(&enter_game), app));

   app->run(*window);

   delete window;

   return 0;

}
