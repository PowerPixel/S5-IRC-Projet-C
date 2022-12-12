
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Le but principal de ce code est de s'assurer que les messages envoyés par le
 * client sont valides, c'est-à-dire que les messages respectent le format JSON
 * et respectent le protocole spécifié par le côté serveur.
 */

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "validation.h"
#include "json.h"

int validate_json_string_is_expected_format(char* json_string) {
    JSONObject *obj = parse_json(json_string);
    int has_code = -1;
    int has_values = -1;
    for (int i = 0; i < obj->nb_field; i++) {
        JSONField* field = obj->fields[i];
        if (strcmp(field->key, "code") == 0) {
            validate_code_field(field);
            has_code = 0;
        } else if (strcmp(field->key, "valeurs") == 0) {
            validate_valeurs_field(field);
            has_values = 0;
        }
    }
    return has_code && has_values;
}

void validate_code_field(JSONField* field) {
    if (field->type != String) {
        printf("Validation failed : field code is not of type string.\n");
        exit(EXIT_FAILURE);
    }
}

void validate_valeurs_field(JSONField* field) {
    if (field->type != Array) {
        printf("Validation failed : valeurs field is of type %d instead of %d.\n", field->type, Array);
        exit(EXIT_FAILURE);
    }
}