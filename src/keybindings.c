#include "gdk/gdkkeysyms.h"
#include <dec2bin.h>
#include <keybindings.h>

static gboolean on_shortcut_quit_app(GtkWidget *widget, GVariant *args,
                                     gpointer user_data) {
  g_application_quit(G_APPLICATION(app));
  return TRUE;
}

static gboolean on_shortcut_clear_entries(GtkWidget *widget, GVariant *args,
                                          gpointer user_data) {
  gtk_editable_set_text(GTK_EDITABLE(input_left->entry), "");
  gtk_editable_set_text(GTK_EDITABLE(input_right->entry), "");

  return TRUE;
}

static gboolean on_shortcut_switch_bases(GtkWidget *widget, GVariant *args,
                                         gpointer user_data) {
  int dropdown_one_val =
      gtk_drop_down_get_selected(GTK_DROP_DOWN(input_left->dropdown));
  int dropdown_two_val =
      gtk_drop_down_get_selected(GTK_DROP_DOWN(input_right->dropdown));
  gtk_drop_down_set_selected(GTK_DROP_DOWN(input_left->dropdown),
                             dropdown_two_val);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(input_right->dropdown),
                             dropdown_one_val);

  return TRUE;
}

/**
 * @brief Adds a keyboard shortcut to a widget
 *
 * @param widget The widget to which the shortcut will be added
 * @param key The GDK key value (e.g., GDK_KEY_s)
 * @param modifiers The modifier mask (e.g., GDK_CONTROL_MASK)
 * @param callback The function to call when the shortcut is activated
 * @param user_data User data to pass to the callback (can be NULL)
 * @param destroy_notify Function to call when user_data is no longer needed
 * (can be NULL)
 * @return GtkEventController* The created controller (can be used to remove the
 * shortcut later)
 */

GtkEventController *add_keyboard_shortcut(GtkWidget *widget, guint key,
                                          GdkModifierType modifiers,
                                          GtkShortcutFunc callback,
                                          gpointer user_data,
                                          GDestroyNotify destroy_notify) {
  GtkEventController *controller = gtk_shortcut_controller_new();
  GtkShortcutTrigger *trigger = gtk_keyval_trigger_new(key, modifiers);
  GtkShortcutAction *action =
      gtk_callback_action_new(callback, user_data, destroy_notify);
  GtkShortcut *shortcut = gtk_shortcut_new(trigger, action);

  gtk_shortcut_controller_add_shortcut(GTK_SHORTCUT_CONTROLLER(controller),
                                       shortcut);
  gtk_widget_add_controller(widget, GTK_EVENT_CONTROLLER(controller));

  return controller;
}

void loadKeyBindings(GtkWidget *window) {
  // quit app shortcut
  add_keyboard_shortcut(GTK_WIDGET(win), GDK_KEY_q, GDK_CONTROL_MASK,
                        on_shortcut_quit_app, NULL, NULL);
  // clear both entries
  add_keyboard_shortcut(GTK_WIDGET(win), GDK_KEY_BackSpace, GDK_CONTROL_MASK,
                        on_shortcut_clear_entries, NULL, NULL);
  // switch between bases
  add_keyboard_shortcut(GTK_WIDGET(win), GDK_KEY_x, GDK_CONTROL_MASK,
                        on_shortcut_switch_bases, NULL, NULL);
}
