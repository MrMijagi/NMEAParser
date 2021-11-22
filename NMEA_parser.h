/**
 * @file NMEA_parser.h
 * @author author <email@example.com>
 * @brief Created with oxygen
 * @version 0.1
 * @date 2021-11-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */


/**
 * 
 * Arrays for printing enums
 * 
 */


extern const char *error_t_strings[];
extern const char *direction_strings[];
extern const char *unit_strings[];


/**
 * 
 * Enums
 * 
 */


typedef enum {
  NO_ERROR,
  INCORRECT_COORDINATE,
  INCORRECT_ALTITUDE,
  INCORRECT_NUMBER_OF_SATELLITES,
  INCORRECT_MESSAGE,
  INCORRECT_CHECKSUM,
  INCORRECT_FORMAT
} error_t;

// Direction for Coordinate structure
// North/South for latitude
// East/West for longitude
typedef enum {
  North,
  South,
  West,
  East
} Direction;

// Unit for Altitude structure
// Meter for antenna altitude
// TODO: check for other Units
typedef enum {
  Meters
} Unit;


/**
 * 
 * Structs
 * 
 */


// Coordinate structure
typedef struct {
  int degrees;
  float minutes;
  Direction direction;
} Coordinate;

// Altitude structure
typedef struct {
  float value;
  Unit unit;
} Altitude;

// Message structure
typedef struct {
  Coordinate latitude;
  Coordinate longitude;
  int satellites;
  Altitude altitude;
} Message;


/**
 * 
 * Functions
 * 
 */


/**
 * @brief convert strings to Coordinate struct
 * 
 * @param value format: ddmm.mm (dd - degrees, mm.mm - minutes)
 * @param dir char (N | S | W | E)
 * @param error 
 * 
 * @return Coordinate
 */
Coordinate parse_coordinate(const char *value, const char *dir, error_t *error);

/**
 * @brief convert strings to Altitude struct
 * 
 * @param value format: x.x
 * @param unit char (M)
 * @param error
 * 
 * @return Altitude 
 */
Altitude parse_altitude(const char *value, const char *unit, error_t *error);

/**
 * @brief convert string into Message struct
 * 
 * @param message GGA message from NMEA standard
 * @param error
 * 
 * @return Message 
 */
Message parse_message(const char *message, error_t *error);

/**
 * @brief nicely print Message structure
 * 
 * @param message 
 */
void print_message_struct(const Message *message);
