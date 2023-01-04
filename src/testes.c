
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * L'objectif principal de ce code est d'effectuer des tests unitaires et
 *  fonctionnels sur les différents composants créés dans ce projet.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "testes.h"
#include "client.h"

void test_hostname( char* hostname )
    {
      if (hostname[0] == '\0') {
        printf("test failed \n");
      }
      else
        printf("test succeded \n");
    }

void test_message( int message_ecrit )
    {
      if ( message_ecrit < 0 ) {
        printf("test failed \n");
      }
      else
        printf("test succeded \n");
    }

void test_calcul( char *  resultat_recu_char, char * operator, char *  operand1_char, char * operand2_char)
    {
      double tmp, operand1 , operand2, resultat_recu;
      

      char *delim = " ";
      char *token = strtok(resultat_recu_char,delim);
      while(token != NULL)
        {
            resultat_recu = atof(token);
            token = strtok(NULL,delim);
        }
      operand1 = atof(operand1_char);
      operand2 = atof(operand2_char);

      if (strcmp(operator, "+") == 0) {
        tmp = operand1 + operand2;
      } else if (strcmp(operator, "-") == 0) {
        tmp = operand1 - operand2;
      } else if (strcmp(operator, "/") == 0) {
        tmp = operand1 / operand2;
      } else if (strcmp(operator, "*") == 0) {
        tmp = operand1 * operand2;
      }

      if ( tmp == resultat_recu ) {
        printf("test succeded \n");
      }
      else {
        printf("test failed \n");
      }
    }