#include <stdio.h>
#include <locale.h>

#include "NMEA_parser.h"

void test_parse_message(const char *message) {
  error_t error;
  Message message_struct = parse_message(message, &error);

  print_message_struct(&message_struct);
  printf("error: %s\n", error_t_strings[error]);
}

int main() {
  char *message_array[] = {
    "$GNGGA,001043.00,4404.14036,S,12118.85961,E,1,12,0.98,1113.0,M,-21.3,M,,*47\r\n",
    "$GPGGA,172814.0,3723.46587704,N,12202.26957864,W,2,6,1.2,18.893,M,-25.669,M,2.0,0031*4F\r\n"
  };

  for (size_t i = 0; i < 2; i++) {
    printf("\n\nTesting message: %s\n", message_array[i]);
    test_parse_message(message_array[i]);
  }

  return 0;
}
