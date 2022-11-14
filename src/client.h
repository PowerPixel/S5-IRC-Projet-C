/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Code du côté client. Le but principal de ce code est de communiquer avec le serveur, 
 * d'envoyer et de recevoir des messages. Ces messages peuvent être du simple texte ou des chiffres.
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

/*
 * port d'ordinateur pour envoyer et recevoir des messages
 */
#define PORT 8089

#define MAX_HOSTNAME_SIZE 256

#define MAX_RANDOM_COLOR 268435456

#define MAX_TAG_SIZE 20

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */
int envoie_recois_message(int socketfd);

/*
* Fonction d'envoi du hostname
*/
int envoie_recois_hostname(int socketfd);


int envoie_balises(int socketfd, char* arg);

/*
 * Fonction d'envoi des balises
 */
int envoie_balises_socket(int socketfd, int nb_balises, char** balises);

/*
 * Fonction qui génére un entier aléatoire entre min et max
 */
int generer_entier_aleatoire(int min, int max);

int generation_couleurs_aleatoire(char* data);


char* get_hostname();

#endif
