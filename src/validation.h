
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "json.h"
/*
 * Le but principal de ce code est de s'assurer que les messages envoyés par le
 * client sont valides, c'est-à-dire que les messages respectent le format JSON
 * et respectent le protocole spécifié par le côté serveur.
 */
int validate_json_string_is_expected_format(char* json_string);

void validate_code_field(JSONField* field);

void validate_valeurs_field(JSONField* field);