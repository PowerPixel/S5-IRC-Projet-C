/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
/*
 * Code pour le traitement des messages au format JSON
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

JSONObject* parse_json(char* input) {
    JSONObject *object = malloc(sizeof(JSONObject));
    object->fields = malloc(sizeof(JSONField) * MAX_JSON_FIELDS);
    object->nb_field = 0;
    int in_object = 1;
    while (*input != '\0') {
        switch (*input) {
            case '{':
                in_object = 0;
                break;
            case '"':
                input++;
                JSONField* field = parse_field(&input);
                object->nb_field = object->nb_field + 1;
                object->fields[object->nb_field - 1] = field;
                break;

            case '}':
                if (in_object) {
                    perror("Object ended without starting (missing opening brace for object)");
                    exit(1);
                }
                break;
            case ' ':
            case '\t':
            case '\n':
            default:
                break;
        };
        input++;
    }
    return object;
}

JSONField* parse_field(char** input) {
    JSONField *result = malloc(sizeof(JSONField));
    // Parsing field key
    char* _input = *input;
    char* key = malloc(sizeof(char) * MAX_KEY_SIZE);
    int size_key = 0;
    while (*_input != '\"') {
        memcpy(&(key[size_key++]), _input++, 1);
    }    
    key[size_key + 1] = '\0';
    result->key = key;
    _input++;
    while (*_input != ':') {
        if (*_input != ' ' || *_input != '\t' || *_input != '\n') {
            printf("%c", *_input);
            perror("Unexepected char during field value parsing");
            exit(1);
        }
        _input++;
    }
    _input++;
    // Parsing field
    while (*_input == ' ' || *_input == '\t' || *_input == '\n') {
        _input++;
    }
    switch (*_input) {
        case '"':
            // Parsing string
            _input++;
            result->type = 0;
            result->value.string = parse_string(&_input);
            break;
        case '[':
            // Parsing array
            _input++;
            result->type = 2;
            result->value.array = parse_array(&_input);
            break;
        default:
            perror("Unrecognized char in JSON Object");
            exit(1);            
    };
    *input = _input;
    return result;
}

char* parse_string(char** input) {
    char* _input = *input;
    char* result = malloc(sizeof(char) * MAX_STRING_SIZE);
    int string_size = 0;
    while (*_input != '\"') {
        memcpy(&(result[string_size++]), _input++, 1);
    }
    result[string_size + 1] = '\0';
    *input = _input;
    return result;
}

JSONArray* parse_array(char** input) {
    char* _input = *input;
    JSONArray *result = malloc(sizeof(JSONArray));
    result->array = malloc(sizeof(long) * MAX_ARRAY_SIZE);
    result->array_type = malloc(sizeof(long) * MAX_ARRAY_SIZE);
    result->array_size = 0;
    while (*_input != ']') {
        if (*_input == ' ' || *_input == '\t' || *_input == '\n') {
            _input++;
            continue;
        }

        if (*_input >= '0' && *_input <= '9') {
            int* parsed_integer = malloc(sizeof(int));
            *parsed_integer = 0;
            while ((*_input >= '0') && (*_input <= '9')) {
                *parsed_integer = *parsed_integer * 10 + (*_input - '0');
                _input++;
            }
            result->array_size = result->array_size + 1;
            result->array[result->array_size - 1] = parsed_integer;
            result->array_type[result->array_size - 1] = 0;
        }

        if (*_input == '\"') {
            char* parsed_string = malloc(sizeof(char) * MAX_STRING_SIZE);
            int string_size = 0;
            _input++;
            while (*_input != '\"') {              
                memcpy(&(parsed_string[string_size++]), _input, 1);
                _input++;
            }
            result->array_size = result->array_size + 1;
            result->array[result->array_size - 1] = parsed_string;
            result->array_type[result->array_size - 1] = 1;
        }
        if (*_input == ']') {
            break;
        }
        _input++;
    }
    *input = _input;
    return result;
}

void print_json_object(JSONObject* object) {
    printf("{\n");
    for (int i = 0; i < object->nb_field; i++) {
        JSONField *field = object->fields[i];
        printf("\t \"%s\": ", field->key);
        switch (field->type) {
            // We don't use other types in this project, and we can be lazy
            // So we don't implement boolean or doubles or int print
            case 0:
                printf("\"%s\"", field->value.string);
                break;
            case 2:
                print_json_array(field->value.array);
                break;
            default:
                break;
        }
        if (i != (object->nb_field - 1)) {
            printf(", \n");
        }
    }
    printf("\n}\n");
}

void print_json_array(JSONArray* array) {
    printf("[ ");
    for (int i = 0; i < array->array_size; i++) {
        switch(array->array_type[i]) {
            case 0:
                printf("%d", *((int *) array->array[i]));
                break;
            case 1:
                printf("\"%s\"", (char *) array->array[i]);
                break;
            default:
                break;
        }
        if (i != (array->array_size - 1)) {
            printf(", ");
        }
    }
    printf(" ]");
}