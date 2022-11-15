
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code permettant de stocker différents types de couleurs : format 24 bits (RGB) ou format 32 bits (32). 
 */


#define MAX_JSON_FIELDS 1024
#define MAX_KEY_SIZE 1024
#define MAX_STRING_SIZE 1024
#define MAX_ARRAY_SIZE 1024


// String, Boolean, Tableau, Object, Entier, Entier double
// [ 2, "#blc", "#cishard"]
// [ 0, 1, 1 ]
// We define two arrays : one to know the type of the array at index i and the other which is the array itself.
typedef struct {
    void** array;
    int* array_type;
    int array_size;
} JSONArray;

typedef struct 
{

    short type; // 0 => String, 1 => Boolean, 2 => Tableau, 3 => Integer, 4 => Double
    char* key;
    union {
        char* string; // 0
        int i_value; // 1, 3
        JSONArray* array; // 2
        double d_value; // 4
    } value;
} JSONField;

typedef struct 
{
    int nb_field;
    JSONField** fields;
} JSONObject;


JSONObject* parse_json(char* input);
JSONArray* parse_array(char** input);
JSONField* parse_field(char** input);
char* parse_string(char** input);
void print_json_object(JSONObject* object);
void print_json_array(JSONArray* array);
