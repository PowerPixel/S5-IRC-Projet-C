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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "bmp.h"

char* get_hostname() {
    int ret;
    char* hostname;
    hostname = malloc(MAX_HOSTNAME_SIZE);

    ret = gethostname(&hostname[0], MAX_HOSTNAME_SIZE);
    if (ret == -1) {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }

    return hostname;
}

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd)
{

  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);
  strcpy(data, "message: ");
  strcat(data, message);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_recois_hostname(int socketfd) {
  char data[MAX_HOSTNAME_SIZE];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  strcpy(data, "nom: ");
  strcat(data, get_hostname());

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_calcul_recois_resultat(int socketfd, char* operator, char* operand1, char* operand2) {
  char data[MAX_HOSTNAME_SIZE];
  double resultat;
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  strcpy(data, "calcul: ");
  strcat(data, operator);
  strcat(data, " ");
  strcat(data, operand1);
  strcat(data, " ");
  strcat(data, operand2);
  


  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur calcul!!!!!");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, (void *) &resultat, sizeof(resultat));
  if (read_status < 0)
  {
    perror("erreur lecture resultat");
    return -1;
  }

  printf("calcul: %lf\n", resultat);

  return 0;
}

void analyse(char *pathname, char *data)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  strcpy(data, "couleurs: ");
  char temp_string[10] = "10,";
  if (cc->size < 10)
  {
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);

  // choisir 10 couleurs
  for (count = 1; count < 11 && cc->size - count > 0; count++)
  {
    if (cc->compte_bit == BITS32)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    if (cc->compte_bit == BITS24)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    strcat(data, temp_string);
  }

  // enlever le dernier virgule
  data[strlen(data) - 1] = '\0';
}

int envoie_couleurs(int socketfd, char *pathname)
{

  char data[1024];
  memset(data, 0, sizeof(data));

  if (strcmp(pathname, "random") == 0) {
    // Générer 10 couleurs random
    generation_couleurs_aleatoire(data);
  } else {
    analyse(pathname, data);
  }
  

  int write_status = write(socketfd, data, strlen(data));
  memset(data, 0, sizeof(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("%s\n", data);

  return 0;
}

int generation_couleurs_aleatoire(char* data) {

  int nb_colors = rand() % (20 + 1 - 10) + 10;
  char buffer[1024];
  // Ecriture des données (donc nb couleurs puis chaque couleur individuellement)
  strcpy(data, "couleurs: ");
  sprintf(buffer, "%d, ", nb_colors);
  strcat(data, buffer);
  for (int i = 0; i < nb_colors; i++) {
    if (i != nb_colors - 1) {
      sprintf(buffer, "#%x, ", (rand() % (MAX_RANDOM_COLOR - 0 + 1)) + 0);
    } else {
      sprintf(buffer, "#%x", (rand() % (MAX_RANDOM_COLOR - 0 + 1)) + 0);
    }
    strcat(data, buffer);
  } 

  

  return 0;
}

int main(int argc, char **argv)
{
  int socketfd;

  struct sockaddr_in server_addr;

  if (argc < 2)
  {
    printf("usage: ./client [msg|hostname|bmp|calcul] <arguments>\n");
    printf("Arguments: ");
    printf("\n\t msg: pas d'arguments");
    printf("\n\t bmp: [<chemin vers l'image en bmp>|random (pour les tests de fonctionnalité)]");
    printf("\n\thostname: pas d'arguments");
    printf("\n\tcalcul: <operateur> <première opérande> <deuxième opérande>");
    printf("\n");
    return (EXIT_FAILURE);
  }

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0)
  {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "msg") == 0) {
    // envoyer et recevoir un message
    envoie_recois_message(socketfd);
  }

  if (strcmp(argv[1], "hostname") == 0) {
    envoie_recois_hostname(socketfd);
  }

  if (strcmp(argv[1], "calcul") == 0) {
    envoie_calcul_recois_resultat(socketfd, argv[2], argv[3], argv[4]);
  }
  else
  {
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, argv[1]);
  }

  close(socketfd);
}
