/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
/*
 * Code pour le traitement des messages au format JSON
 */

#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JSONObject * parse_json(char *input) {
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
        JSONField *field = parse_field(&input);
        object->nb_field = object->nb_field + 1;
        object->fields[object->nb_field - 1] = field;
        break;

      case '}':
        if (in_object) {
          perror(
              "Object ended without starting (missing opening brace for "
              "object)");
          exit(1);
        }
        in_object = 1;
        break;
      case ' ':
      case '\t':
      case '\n':
      default:
        break;
    };
    input++;
  }
  if (in_object == 0) {
    perror("Object was not terminated with a closing curly bracket. Exiting.");
    exit(EXIT_FAILURE);
  }
  return object;
}

JSONField *parse_field(char **input) {
  JSONField *result = malloc(sizeof(JSONField));
  // Parsing field key
  char *_input = *input;
  char *key = malloc(sizeof(char) * MAX_KEY_SIZE);
  int size_key = 0;
  while (*_input != '\"') {
    memcpy(&(key[size_key++]), _input++, 1);
  }
  _input++;
  key[size_key + 1] = '\0';
  result->key = key;
  while (*_input != ':') {
    if ((*_input != ' ') && (*_input != '\t') && (*_input != '\n')) {
      printf("Unexepected char during field value parsing (key is %s) -> %d", result->key, *_input);
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
      result->type = String;
      result->value.string = parse_string(&_input);
      break;
    case '[':
      // Parsing array
      _input++;
      result->type = Array;
      result->value.array = parse_array(&_input);
      break;
    default:
      perror("Unrecognized char in JSON Object");
      exit(1);
  };
  *input = _input;
  return result;
}

char *parse_string(char **input) {
  char *_input = *input;
  char *result = malloc(sizeof(char) * MAX_STRING_SIZE);
  int string_size = 0;
  // We keep copying chars until we hit the closing bracket
  while (*_input != '\"') {
    memcpy(&(result[string_size++]), _input++, 1);
  }
  result[string_size + 1] = '\0';
  *input = _input;
  return result;
}

JSONArray *parse_array(char **input) {
  char *_input = *input;
  JSONArray *result = malloc(sizeof(JSONArray));
  result->array = malloc(sizeof(void *) * MAX_ARRAY_SIZE);
  result->array_type = malloc(sizeof(JSONType) * MAX_ARRAY_SIZE);
  result->array_size = 0;
  while (*_input != ']') {
    if (*_input == ' ' || *_input == '\t' || *_input == '\n') {
      _input++;
      continue;
    }

    if (*_input >= '0' && *_input <= '9') {
      int *parsed_integer = malloc(sizeof(int));
      *parsed_integer = 0;
      while ((*_input >= '0') && (*_input <= '9')) {
        *parsed_integer = *parsed_integer * 10 + (*_input - '0');
        _input++;
      }
      result->array_size = result->array_size + 1;
      result->array[result->array_size - 1] = parsed_integer;
      result->array_type[result->array_size - 1] = Integer;
    }

    if (*_input == '\"') {
      char *parsed_string = malloc(sizeof(char) * MAX_STRING_SIZE);
      int string_size = 0;
      _input++;
      while (*_input != '\"') {
        memcpy(&(parsed_string[string_size++]), _input, 1);
        _input++;
      }
      result->array_size = result->array_size + 1;
      result->array[result->array_size - 1] = parsed_string;
      result->array_type[result->array_size - 1] = String;
    }
    if (*_input == ']') {
      break;
    }
    _input++;
  }
  *input = _input;
  return result;
}

void print_json_object(JSONObject *object) {
  printf("{\n");
  for (int i = 0; i < object->nb_field; i++) {
    JSONField *field = object->fields[i];
    printf("\t \"%s\": ", field->key);
    switch (field->type) {
        // We don't use other types in this project, and we can be lazy
        // So we don't implement boolean or doubles or int print
      case String:
        printf("\"%s\"", field->value.string);
        break;
      case Array:
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

void print_json_array(JSONArray *array) {
  printf("[ ");
  if (array->array_size != 0) {
    for (int i = 0; i < array->array_size; i++) {
      switch (array->array_type[i]) {
        case Integer:
          printf("%d", *((int *)array->array[i]));
          break;
        case String:
          printf("\"%s\"", (char *)array->array[i]);
          break;
        default:
          break;
      }
      if (i != (array->array_size - 1)) {
        printf(", ");
      }
    }
  }
  printf(" ]");
}

JSONField *get_value(JSONObject *object, char *key) {
  for (int i = 0; i < object->nb_field; i++) {
    if (strcmp(key, object->fields[i]->key) == 0) {
      return object->fields[i];
    }
  }
  return NULL;
}

JSONArray *create_array() {
  JSONArray *result = malloc(sizeof(JSONArray));
  result->array = malloc(sizeof(void *) * MAX_ARRAY_SIZE);
  result->array_type = malloc(sizeof(JSONType) * MAX_ARRAY_SIZE);
  result->array_size = 0;
  return result;
}

JSONObject *create_json_object(char *code, JSONArray *arguments) {
  JSONObject *object = malloc(sizeof(JSONObject));

  JSONField *_code = malloc(sizeof(JSONField));
  JSONField *_arguments = malloc(sizeof(JSONField));

  object->fields = malloc(sizeof(JSONField) * MAX_JSON_FIELDS);

  _code->key = "code";
  _code->type = String;
  _code->value.string = code;

  _arguments->key = "valeurs";
  _arguments->type = Array;
  _arguments->value.array = arguments;

  object->nb_field = 2;
  object->fields[0] = _code;
  object->fields[1] = _arguments;

  return object;
}

void insert_str_into_array(char *str, JSONArray *array) {
  array->array_size += 1;
  array->array[array->array_size - 1] = str;
  array->array_type[array->array_size - 1] = String;
}

void insert_int_into_array(int *integer, JSONArray *array) {
  array->array_size += 1;
  array->array[array->array_size - 1] = integer;
  array->array_type[array->array_size - 1] = Integer;
}

void convert_to_data(char *data, JSONObject *object) {
  sprintf(data + strlen(data), "{");
  for (int i = 0; i < object->nb_field; i++) {
    JSONField *field = object->fields[i];
    sprintf(data + strlen(data), "\"%s\":", field->key);
    switch (field->type) {
        // We don't use other types in this project, and we can be lazy
        // So we don't implement boolean or doubles or int print
      case String:
        sprintf(data + strlen(data), "\"%s\"", field->value.string);
        break;
      case Array:
        convert_json_array_to_data(data, field->value.array);
        break;
      default:
        break;
    }
    if (i != (object->nb_field - 1)) {
      sprintf(data + strlen(data), ",");
    }
  }
  sprintf(data + strlen(data), "}");
}

void convert_json_array_to_data(char *data, JSONArray *array) {
  sprintf(data + strlen(data), "[");
  for (int i = 0; i < array->array_size; i++) {
    switch (array->array_type[i]) {
      case Integer:
        sprintf(data + strlen(data), "%d", *((int *)array->array[i]));
        break;
      case String:
        sprintf(data + strlen(data), "\"%s\"", (char *)array->array[i]);
        break;
      default:
        break;
    }
    if (i != (array->array_size - 1)) {
      sprintf(data + strlen(data), ",");
    }
  }
  sprintf(data + strlen(data), "]");
}

int json_to_text(JSONObject *object, char *data) {
  if (object->nb_field != 2) {
    return -1;
  }
  // The first field of the object isn't a code, go away
  if (strcmp(object->fields[0]->key, "code") != 0) {
    return -1;
  }
  if (strcmp(object->fields[1]->key, "valeurs") != 0) {
    return -1;
  }

  sprintf(data, "%s: ", object->fields[0]->value.string);
  int *integer;
  for (int i = 0; i < object->fields[1]->value.array->array_size; i++) {
    switch (object->fields[1]->value.array->array_type[i]) {
      case String:
        sprintf(data + strlen(data), "%s",
                (char *)object->fields[1]->value.array->array[i]);
        break;
      case Integer:
        integer = (int *)object->fields[1]->value.array->array[i];
        sprintf(data + strlen(data), "%d", *integer);
        break;
      case Array:
      default:
        break;
    }
    if ((i + 1) != object->fields[1]->value.array->array_size) {
      sprintf(data + strlen(data), ", ");
    }
  }

  return 0;
}

// TODO: yuck !! code dup, fix that later
int json_to_text_calcul(JSONObject *object, char *data) {
  if (object->nb_field != 2) {
    return -1;
  }
  // The first field of the object isn't a code, go away
  if (strcmp(object->fields[0]->key, "code") != 0) {
    return -1;
  }
  if (strcmp(object->fields[1]->key, "valeurs") != 0) {
    return -1;
  }

  sprintf(data, "%s: ", object->fields[0]->value.string);
  int *integer;
  for (int i = 0; i < object->fields[1]->value.array->array_size; i++) {
    switch (object->fields[1]->value.array->array_type[i]) {
      case String:
        sprintf(data + strlen(data), "%s",
                (char *)object->fields[1]->value.array->array[i]);
        break;
      case Integer:
        integer = (int *)object->fields[1]->value.array->array[i];
        sprintf(data + strlen(data), "%d", *integer);
        break;
      case Array:
      default:
        break;
    }
    if ((i + 1) != object->fields[1]->value.array->array_size) {
      sprintf(data + strlen(data), " ");
    }
  }

  return 0;
}
