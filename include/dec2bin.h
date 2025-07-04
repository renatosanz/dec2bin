#include <gtk/gtk.h>
#include <regex.h>

typedef struct {
  GtkWidget *entry;
  GtkWidget *dropdown;
  char last_state[256];
  regex_t last_regex;
} InputData;

extern InputData *input_left, *input_right;
extern GtkApplication *app;
extern GtkWindow *win;
extern GtkWidget *clean_button, *close_button, *switch_button, *about_btn;

void compile_regex(InputData *data, char *base);
char *convert(char *from_base, char *to_base, char *value, regex_t regex, int eval_regex_flag);

void switch_bases(GtkButton *switch_button, gpointer *user_data) ;
