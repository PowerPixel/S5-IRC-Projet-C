
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code permettant de stocker différents types de couleurs : format 24 bits
 * (RGB) ou format 32 bits (32).
 */

#define MAX_JSON_FIELDS 1024
#define MAX_KEY_SIZE 1024
#define MAX_STRING_SIZE 1024
#define MAX_ARRAY_SIZE 1024

typedef enum { String, Array, Integer } JSONType;

// Structure representing a JSON Array, they can be of various type so we did a
// little bit of engineering : [ 2, "#blc", "#cishard"] [ 0, 1, 1 ] We define
// two arrays : one to know the type of the array at index i and the other which
// is the array itself.
typedef struct {
  void **array;
  JSONType *array_type;
  int array_size;
} JSONArray;

/*
    Structure representing a key-value pair in a JSON pair
*/
typedef struct {
  JSONType type;
  char *key;
  union {
    char *string;      // 0
    int i_value;       // 1, 3
    JSONArray *array;  // 2
    double d_value;    // 4
  } value;
} JSONField;

/*
    Structure representing a JSON Object
*/
typedef struct {
  int nb_field;
  JSONField **fields;
} JSONObject;

/*
    Method to parse a json from a text input
*/
JSONObject *parse_json(char *input);

/*
    Method to parse a json array from a text input
*/
JSONArray *parse_array(char **input);

/*
    Method to parse a key-value pair in a JSON object
*/
JSONField *parse_field(char **input);

/*
    Method to parse a string in a JSON Field
*/
char *parse_string(char **input);

/*
    Method to print a JSON object
*/
void print_json_object(JSONObject *object);

/*
    Method to print a JSON Array
*/
void print_json_array(JSONArray *array);

/*
    Method to find a value in a object
*/

JSONField *get_value(JSONObject *object, char *key);

/*
    Method to create an empty JSON Array
*/

JSONArray *create_array();

/*
    Method to create a JSON Object with code and arguments
*/

JSONObject *create_json_object(char *code, JSONArray *arguments);

/*
    Method to insert a string into an array
*/

void insert_str_into_array(char *string, JSONArray *array);

/*
    Method to insert a integer into an array
*/

void insert_int_into_array(int *integer, JSONArray *array);

/*
    Method to convert JSONObject to data string (for sending over a socket)
*/

void convert_to_data(char *data, JSONObject *object);

/*
    Method to convert JSONArray to data string (for sending it over a socket :°)
   )
*/
void convert_json_array_to_data(char *data, JSONArray *array);

/*
    Method to convert JSON array to text !!!!!! omg!!!
 */
int json_to_text(JSONObject *object, char *data);

/*
    Method to convert JSON array to text !!!!!! omg!!!
 */
int json_to_text_calcul(JSONObject *object, char *data);
