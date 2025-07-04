#include "glib.h"
#include <dec2bin.h>
#include <math.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

static const char *const regex_bin = "^[01]+$";
static const char *const regex_hex = "^[0-9A-F]+$";
static const char *const regex_oct = "^[0-7]+$";
static const char *const regex_dec = "^[0-9]+$";

void reverse(char *bin, int left, int right) {
  while (left < right) {
    char temp = bin[left];
    bin[left] = bin[right];
    bin[right] = temp;
    left++;
    right--;
  }
}

// from_base functions
long int from_hex(char *input) {
  long int decimalNumber = 0;
  char hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  int i, j, power = 0;

  for (i = strlen(input) - 1; i >= 0; i--) {
    for (j = 0; j < 16; j++) {
      if (input[i] == hexDigits[j]) {
        decimalNumber += j * pow(16, power);
      }
    }
    power++;
  }
  return decimalNumber;
}

long int from_binary(char *input) {
  int i, power = 0;
  long int decimalNumber = 0;
  for (i = strlen(input) - 1; i >= 0; i--) {
    if (input[i] == '1') {
      decimalNumber += pow(2, power);
    }
    power++;
  }
  return decimalNumber;
}

long int from_decimal(char *input) {
  long int decimalNumber = 0;
  char decimalDigits[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  int i, j, power = 0;

  for (i = strlen(input) - 1; i >= 0; i--) {
    for (j = 0; j < 10; j++) {
      if (input[i] == decimalDigits[j]) {
        decimalNumber += j * pow(10, power);
      }
    }
    power++;
  }
  return decimalNumber;
}

long int from_octal(const char *input) {
  long int decimalNumber = 0;
  char octalDigits[8] = {'0', '1', '2', '3', '4', '5', '6', '7'};
  int i, j, power = 0;

  for (i = strlen(input) - 1; i >= 0; i--) {
    for (j = 0; j < 8; j++) {
      if (input[i] == octalDigits[j]) {
        decimalNumber += j * pow(8, power);
      }
    }
    power++;
  }
  return decimalNumber;
}

// to_base functions
void to_hex(long int input, char *buffer) {
  int index = 0;
  while (input > 0) {
    int bit = input % 16;
    if (bit > 9) {
      buffer[index++] = '@' + (bit - 9);
    } else {
      buffer[index++] = '0' + (bit);
    }
    input /= 16;
  }
  buffer[index] = '\0';
  reverse(buffer, 0, index - 1);
}

void to_binary(long int input, char *buffer) {
  int index = 0;
  while (input > 0) {
    int bit = input % 2;
    buffer[index++] = '0' + bit;
    input /= 2;
  }
  buffer[index] = '\0';
  reverse(buffer, 0, index - 1);
}

void to_decimal(long int input, char *buffer) {
  int index = 0;
  while (input > 0) {
    int bit = input % 10;
    buffer[index++] = '0' + bit;
    input /= 10;
  }
  buffer[index] = '\0';
  reverse(buffer, 0, index - 1);
}

void to_octal(long int input, char *buffer) {
  int index = 0;
  while (input > 0) {
    int bit = input % 8;
    buffer[index++] = '0' + bit;
    input /= 8;
  }
  buffer[index] = '\0';
  reverse(buffer, 0, index - 1);
}

// compile regex for each regex
void compile_regex(InputData *data, char *base) {
  // g_print("regex_compiled %s\n", base);
  if (strcmp(base, "binary") == 0) {
    regcomp(&data->last_regex, regex_bin, REG_EXTENDED);
  } else if (strcmp(base, "octal") == 0) {
    regcomp(&data->last_regex, regex_oct, REG_EXTENDED);
  } else if (strcmp(base, "hex") == 0) {
    regcomp(&data->last_regex, regex_hex, REG_EXTENDED);
  } else if (strcmp(base, "decimal") == 0) {
    regcomp(&data->last_regex, regex_dec, REG_EXTENDED);
  } else {
    printf("Comando desconocido: %s\n", base);
  }
}

// convert function - takes a number on base x and convert it to base y
char *convert(char *from_base, char *to_base, char *value, regex_t regex,
              int eval_regex_flag) {
  if (eval_regex_flag) {
    int regex_val = regexec(&regex, value, 0, NULL, 0);
    if (regex_val == REG_NOMATCH) {
      return "Syntax Error";
    }
  }

  if (!strcmp(from_base, to_base)) {
    return value;
  }

  // from base x to decimal
  long int decimal_val = 0;
  if (strcmp(from_base, "binary") == 0) {
    decimal_val = from_binary(value);
  } else if (strcmp(from_base, "octal") == 0) {
    decimal_val = from_octal(value);
  } else if (strcmp(from_base, "hex") == 0) {
    decimal_val = from_hex(value);
  } else if (strcmp(from_base, "decimal") == 0) {
    decimal_val = from_decimal(value);
  } else {
    decimal_val = 0;
    printf("Comando desconocido: %s\n", from_base);
  }

  long int buffer_size = snprintf(NULL, 0, "%ld", decimal_val) + 1;
  // g_print("Número como cadena: %ld\n", decimal_val);
  char *buffer = malloc(buffer_size);

  // from decimal to base y
  if (strcmp(to_base, "binary") == 0) {
    to_binary(decimal_val, buffer);
  } else if (strcmp(to_base, "octal") == 0) {
    to_octal(decimal_val, buffer);
  } else if (strcmp(to_base, "hex") == 0) {
    to_hex(decimal_val, buffer);
  } else if (strcmp(to_base, "decimal") == 0) {
    to_decimal(decimal_val, buffer);
  } else {
    printf("Comando desconocido: %s\n", from_base);
  }

  // g_print("res: %s\n", buffer);

  return buffer;
}
