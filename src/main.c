#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include <dec2bin.h>
#include <gtk/gtk.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

// app & window
GtkApplication *app;
GtkWindow *win;

// buttons
GtkWidget *clean_button, *close_button, *switch_button, *about_btn;

// inputs data
InputData *input_left, *input_right, *active_input;

void *exit_app(GtkWidget *button, gpointer user_data) {
  g_application_quit(G_APPLICATION(app));
  return 0;
}

void display_about_window(GtkWidget *about_button, gpointer *user_data) {
  GFile *logo_file = g_file_new_for_path("data/icons/logo.png");
  GdkTexture *example_logo = gdk_texture_new_from_file(logo_file, NULL);
  g_object_unref(logo_file);

  gtk_show_about_dialog(GTK_WINDOW(win),           // Ventana padre
                        "program-name", "dec2bin", // Nombre del programa
                        "version", "1.0",          // Versión
                        "copyright", "2025 Renato Sanchez Loeza", "website",
                        "https://riprtx.netlify.app/", "website-label",
                        "my blog", "license-type",
                        GTK_LICENSE_GPL_3_0, // Derechos de autor
                        "comments",
                        "Base Converter GTK app.", // Comentarios
                        "logo",
                        example_logo, // Logo
                        NULL          // Fin de los argumentos
  );
}

void execute_base_convert(GtkEditable *editable_from,
                          GtkEditable *editable_to) {

  char *value = gtk_editable_get_text(GTK_EDITABLE(editable_from));
  if (strlen(value) > 0) {
    char *from_base = gtk_string_object_get_string(GTK_STRING_OBJECT(
        gtk_drop_down_get_selected_item(GTK_DROP_DOWN(input_left->dropdown))));
    char *to_base = gtk_string_object_get_string(GTK_STRING_OBJECT(
        gtk_drop_down_get_selected_item(GTK_DROP_DOWN(input_right->dropdown))));
    char *converted_value =
        convert(from_base, to_base, value, active_input->last_regex, 1);
    g_signal_handlers_block_matched(G_OBJECT(editable_to), G_SIGNAL_MATCH_ID,
                                    g_signal_lookup("changed", GTK_TYPE_ENTRY),
                                    0, NULL, NULL, NULL);

    // block & unblock the signals of editable_to, otherwise it will loop
    // forever
    gtk_editable_set_text(GTK_EDITABLE(editable_to), converted_value);

    g_signal_handlers_unblock_matched(
        G_OBJECT(editable_to), G_SIGNAL_MATCH_ID,
        g_signal_lookup("changed", GTK_TYPE_ENTRY), 0, NULL, NULL, NULL);
  } else {
    gtk_editable_set_text(GTK_EDITABLE(input_right->entry), "");
  }
}

void set_entry_placeholder(GtkDropDown *dropdown, GtkEntry *entry) {
  gtk_entry_set_placeholder_text(
      entry, gtk_string_object_get_string(GTK_STRING_OBJECT(
                 gtk_drop_down_get_selected_item(GTK_DROP_DOWN(dropdown)))));
}

void update_base_converter(GtkDropDown *dropdown, GParamSpec *pspec,
                           InputData *input) {
  char *value = gtk_editable_get_text(GTK_EDITABLE(input->entry));
  char *new_base = gtk_string_object_get_string(GTK_STRING_OBJECT(
      gtk_drop_down_get_selected_item(GTK_DROP_DOWN(dropdown))));
  compile_regex(input, new_base);

  if (strlen(value) > 0) {

    char *converted_value =
        convert(input->last_state, new_base, value, input->last_regex, 0);
    g_signal_handlers_block_matched(G_OBJECT(input->entry), G_SIGNAL_MATCH_ID,
                                    g_signal_lookup("changed", GTK_TYPE_ENTRY),
                                    0, NULL, NULL, NULL);
    gtk_editable_set_text(GTK_EDITABLE(input->entry), converted_value);
    g_signal_handlers_unblock_matched(
        G_OBJECT(input->entry), G_SIGNAL_MATCH_ID,
        g_signal_lookup("changed", GTK_TYPE_ENTRY), 0, NULL, NULL, NULL);
    strcpy(input->last_state, new_base);
  }
  set_entry_placeholder(dropdown, GTK_ENTRY(input->entry));
}

void set_last_state(GtkDropDown *dropdown, GParamSpec *pspec,
                    char *last_state) {
  strcpy(last_state,
         gtk_string_object_get_string(GTK_STRING_OBJECT(
             gtk_drop_down_get_selected_item(GTK_DROP_DOWN(dropdown)))));
}

void clean_entries(GtkButton *clean_button, gpointer *user_data) {
  gtk_editable_set_text(GTK_EDITABLE(input_left->entry), "");
  gtk_editable_set_text(GTK_EDITABLE(input_right->entry), "");
}

void switch_bases(GtkButton *switch_button, gpointer *user_data) {
  int dropdown_one_val =
      gtk_drop_down_get_selected(GTK_DROP_DOWN(input_left->dropdown));
  int dropdown_two_val =
      gtk_drop_down_get_selected(GTK_DROP_DOWN(input_right->dropdown));
  gtk_drop_down_set_selected(GTK_DROP_DOWN(input_left->dropdown),
                             dropdown_two_val);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(input_right->dropdown),
                             dropdown_one_val);
}

void select_active_input(GtkEntry *entry, InputData *recived_input_data) {
  active_input = recived_input_data;
}

static void *on_activate(GtkApplication *app, gpointer user_data) {
  input_right = malloc(sizeof(InputData));
  input_left = malloc(sizeof(InputData));

  GtkIconTheme *icon_theme;
  icon_theme = gtk_icon_theme_get_for_display(gdk_display_get_default());
  gtk_icon_theme_add_search_path(icon_theme, "/org/dec2bin/data/icons");
  if (!gtk_icon_theme_has_icon(icon_theme, "logo")) {
    printf("icon error.\n");
  }

  GtkBuilder *builder = gtk_builder_new();
  gtk_builder_add_from_resource(builder, "/org/dec2bin/data/ui/dec2bin.ui",
                                NULL);

  // bind widgets to variables
  win = GTK_WINDOW(gtk_builder_get_object(builder, "dec2bin_win"));

  // inputs
  input_left->entry = GTK_WIDGET(gtk_builder_get_object(builder, "entry_left"));
  input_right->entry =
      GTK_WIDGET(gtk_builder_get_object(builder, "entry_right"));
  input_left->dropdown =
      GTK_WIDGET(gtk_builder_get_object(builder, "base_one"));
  input_right->dropdown =
      GTK_WIDGET(gtk_builder_get_object(builder, "base_two"));

  // set last states
  strcpy(input_left->last_state,
         gtk_string_object_get_string(
             GTK_STRING_OBJECT(gtk_drop_down_get_selected_item(
                 GTK_DROP_DOWN(input_left->dropdown)))));
  strcpy(input_right->last_state,
         gtk_string_object_get_string(
             GTK_STRING_OBJECT(gtk_drop_down_get_selected_item(
                 GTK_DROP_DOWN(input_right->dropdown)))));

  gtk_window_set_default_icon_name("logo");

  // compile regex for decimal values
  compile_regex(input_right, input_right->last_state);
  compile_regex(input_left, input_left->last_state);

  close_button = GTK_WIDGET(gtk_builder_get_object(builder, "close_btn"));
  about_btn = GTK_WIDGET(gtk_builder_get_object(builder, "about_display_btn"));
  switch_button =
      GTK_WIDGET(gtk_builder_get_object(builder, "switch_base_btn"));
  clean_button = GTK_WIDGET(gtk_builder_get_object(builder, "clean_btn"));

  // setup ui
  gtk_drop_down_set_selected(GTK_DROP_DOWN(input_left->dropdown), 0);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(input_right->dropdown), 0);
  set_entry_placeholder(GTK_DROP_DOWN(input_left->dropdown),
                        GTK_ENTRY(input_left->entry));
  set_entry_placeholder(GTK_DROP_DOWN(input_right->dropdown),
                        GTK_ENTRY(input_right->entry));

  // connect close button
  g_signal_connect(GTK_WIDGET(close_button), "clicked", G_CALLBACK(exit_app),
                   NULL);
  g_signal_connect(GTK_WIDGET(about_btn), "clicked",
                   G_CALLBACK(display_about_window), NULL);
  g_signal_connect(GTK_WIDGET(switch_button), "clicked",
                   G_CALLBACK(switch_bases), NULL);
  g_signal_connect(GTK_WIDGET(clean_button), "clicked",
                   G_CALLBACK(clean_entries), NULL);
  // connect entrys to converters
  g_signal_connect(GTK_EDITABLE(input_left->entry), "changed",
                   G_CALLBACK(select_active_input), input_left);
  g_signal_connect(GTK_EDITABLE(input_right->entry), "changed",
                   G_CALLBACK(select_active_input), input_right);

  g_signal_connect(GTK_EDITABLE(input_left->entry), "changed",
                   G_CALLBACK(execute_base_convert),
                   GTK_EDITABLE(input_right->entry));
  g_signal_connect(GTK_EDITABLE(input_right->entry), "changed",
                   G_CALLBACK(execute_base_convert),
                   GTK_EDITABLE(input_left->entry));

  g_signal_connect(GTK_WIDGET(input_left->dropdown), "notify::selected",
                   G_CALLBACK(update_base_converter), input_left);
  g_signal_connect(GTK_WIDGET(input_right->dropdown), "notify::selected",
                   G_CALLBACK(update_base_converter), input_right);
  g_object_unref(builder);

  gtk_application_add_window(GTK_APPLICATION(app), GTK_WINDOW(win));
  gtk_window_present(GTK_WINDOW(win));
  return 0;
}

int main(int argc, char **argv) {
  gtk_init();
  app = gtk_application_new("org.riprtx.dec2Bin", G_APPLICATION_DEFAULT_FLAGS);
  gtk_window_set_default_icon_name("dec2bin");
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  free(input_left);
  free(input_right);
  return status;
}
