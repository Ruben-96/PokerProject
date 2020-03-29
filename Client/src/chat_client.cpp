#include <gtk/gtk.h>
#include <iostream>

void enter_game();

GtkWidget *entry_ip;
GtkWidget *entry_port;
GtkWidget *btn_join;
GtkWidget *btn_spectate;
GtkWidget *window2;
GtkWidget *window;
int main(int argc, char *argv[]){

   GtkBuilder *builder;
   
   gtk_init(&argc, &argv);

   builder = gtk_builder_new();
   gtk_builder_add_from_file(builder, "glade/MainWindow.glade", NULL);

   window = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
   window2 = GTK_WIDGET(gtk_builder_get_object(builder, "GameWindow"));
   gtk_builder_connect_signals(builder, NULL);

   entry_ip = GTK_WIDGET(gtk_builder_get_object(builder, "entry_ip"));
   entry_port = GTK_WIDGET(gtk_builder_get_object(builder, "entry_port"));
   btn_join = GTK_WIDGET(gtk_builder_get_object(builder, "btn_join"));
   btn_spectate = GTK_WIDGET(gtk_builder_get_object(builder, "btn_spectate"));

   g_object_unref(builder);

   gtk_widget_show(window);
   gtk_main();

   return 0;

}

void enter_game(){
   gtk_widget_hide(window);
   gtk_widget_hide(btn_join);
   std::cout << "enter_game" << std::endl;
   gtk_widget_show(window2);
}

void on_window_main_destroy(){
   gtk_main_quit();
}
