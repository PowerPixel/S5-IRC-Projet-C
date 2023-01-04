
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


#ifndef __TESTES_H__
#define __TESTES_H__

void test_hostname( char * hostname);

void test_message( int message_ecrit );

void test_calcul( char * resultat_recu, char * operator, char *  operand1, char *  operand2);

#endif
