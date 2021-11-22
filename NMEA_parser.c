#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "NMEA_parser.h"


const char *error_t_strings[] = {
  "NO_ERROR",
  "INCORRECT_COORDINATE",
  "INCORRECT_ALTITUDE",
  "INCORRECT_NUMBER_OF_SATELLITES",
  "INCORRECT_MESSAGE",
  "INCORRECT_CHECKSUM",
  "INCORRECT_FORMAT"
};

const char *direction_strings[] = {
  "North",
  "South",
  "West",
  "East"
};

const char *unit_strings[] = {
  "Meters"
};


Coordinate parse_coordinate(const char *value, const char *dir, error_t *error) {
  Coordinate coordinate;

  int digits;
  // first find out how many digits are used for degrees
  for (size_t i = 3; i <= 5; i++) {
    if (*(value + i) == '.') {
      digits = i - 2;
    }
  }

  // convert value to degrees
  errno = 0;
          
  char *end;
  char degree[digits + 1];

  // take first two characters from value
  strncpy(degree, value, digits);
  coordinate.degrees = (int) strtol(degree, &end, 10);
  
  // check for error while parsing to int
  if (errno != 0) {
    *error = INCORRECT_COORDINATE;
  }

  // convert rest of the value to minutes
  errno = 0;

  coordinate.minutes = strtof(value + digits, &end);
  // check for error while parsing to float
  if (errno != 0) {
    *error = INCORRECT_COORDINATE;
  }
  
  // read direction
  switch (*dir) {
    case 'N':
      coordinate.direction = North;
      break;
    case 'S':
      coordinate.direction = South;
      break;
    case 'W':
      coordinate.direction = West;
      break;
    case 'E':
      coordinate.direction = East;
      break;
    default:
      *error = INCORRECT_ALTITUDE;
  }

  return coordinate;
}

Altitude parse_altitude(const char *value, const char *unit, error_t *error) {
  Altitude altitude;

  // convert string to float
  errno = 0;
          
  char *end;
  altitude.value = strtof(value, &end);
  
  // check for error while parsing to float
  if (errno != 0) {
    *error = INCORRECT_ALTITUDE;
  }
  
  // read unit
  switch (*unit) {
    case 'M':
      altitude.unit = Meters;
      break;
    default:
      *error = INCORRECT_ALTITUDE;
  }

  return altitude;
}

Message parse_message(const char *message, error_t *error) {
  int checksum = 0;
  char tmp[82] = "";      // array of char to remember previous strings
  char buffer[82] = "";   // buffer that takes all the chars before next delimeter
                          // 82 because thats the maximum message length
  int counter = 0;        // keeps track of which value is buffer reading

  Message message_struct;

  // check if message starts with '$'
  if (strncmp(message, "$", 1) != 0) {
    *error = INCORRECT_FORMAT;
  }

  // this loop will run until it meets either '*' or '\r'
  // if it ends because of null terminator the format is incorrect
  while (*message) {
    // it's not obligatory to put checksum at the end
    // so I also check for carriage return
    if (*message == '*' || *message == '\r') {
      break;         // I think it's bad to use break in order to leave loop but well
    } else if (*message != '$') {
      checksum ^= *message;
    }

    if (*message != ',') {
      // add character to buffer
      int length = strlen(buffer);
      buffer[length] = *message;
      buffer[length + 1] = '\0';
    } else {
      // save to struct depending on counter
      switch (counter) {
        case 0:
          // check if type of message is correct
          if (strncmp(buffer + 3, "GGA", 3) != 0) {
            *error = INCORRECT_MESSAGE;
          }
          break;
        case 2:
        case 4:
        case 9:
          // remember latitude / longitude / altitude
          strncpy(tmp, buffer, 82);
          break;
        case 3:
          // save direction of latitude and tmp into struct
          message_struct.latitude = parse_coordinate(tmp, buffer, error);
          break;
        case 5:
          // save direction of longtitude and tmp into struct
          message_struct.longitude = parse_coordinate(tmp, buffer, error);
          break;
        case 7:
          // save number of satellites
          errno = 0;
          
          char *end;
          message_struct.satellites = (int) strtol(buffer, &end, 10);
          
          // check for error while parsing to int
          if (errno != 0) {
            *error = INCORRECT_NUMBER_OF_SATELLITES;
          } else if (message_struct.satellites < 0 ||
                     message_struct.satellites > 12)
          {
            *error = INCORRECT_NUMBER_OF_SATELLITES;
          }

          break;
        case 10:
          // save altitude unit and tmp into struct
          message_struct.altitude = parse_altitude(tmp, buffer, error);
          break;
      }

      // clear buffer
      memset(buffer, 0, 82);
      // increase counter
      counter++;
    }

    message++;
  }

  // check if there are other characters left
  if (*message == '\0') {
    *error = INCORRECT_FORMAT;
  } else {
    // first check checksum (if it was provided in first place)
    if (*message == '*') {
      // checksum should be exactly two characters
      if (*(message + 3) != '\r') {
        *error = INCORRECT_CHECKSUM;
      }

      // convert int into hex string
      char hex[3];
      snprintf(hex, 2, "%X", checksum);

      // compare both characters
      message++;
      if (strncmp(message, hex, 2) != 0) {
        *error = INCORRECT_CHECKSUM;
      }

      // skip past checksum section
      message += 2;
    }

    // check if message ends with <CR><LF>
    if (strncmp(message, "\r\n", 2) != 0) {
      *error = INCORRECT_FORMAT;
    }

    // check if there were enough commas
    if (counter < 10) {
      *error = INCORRECT_FORMAT;
    }
  }

  return message_struct;
}

void print_message_struct(const Message *message) {
  printf("latitude:\n\tdegrees: %d\n\tminutes: %f\n\tdirection: %s\n",
    message->latitude.degrees,
    message->latitude.minutes,
    direction_strings[message->latitude.direction]);
  
  printf("longitude:\n\tdegrees: %d\n\tminutes: %f\n\tdirection: %s\n",
    message->longitude.degrees,
    message->longitude.minutes,
    direction_strings[message->longitude.direction]);

  printf("altitude:\n\tvalue: %f\n\tunit: %s\n",
    message->altitude.value,
    unit_strings[message->altitude.unit]);

  printf("number of satellites: %d\n", message->satellites);
}
