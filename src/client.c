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
#include <sys/mman.h>
#include <fcntl.h>


#include "client.h"
#include "bmp.h"
#include "json.h"


void parse_json_file(char* json_filename) {
  int file = open(json_filename, O_RDONLY);
  int len = lseek(file, 0, SEEK_END);
  void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, file, 0);
  JSONObject *object = parse_json(data);
  print_json_object(object);
}

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

int envoie_balises(int socketfd, char* arg) {
    int nb_balises;
    char** tags;

    if (strcmp(arg, "random") == 0) {
        nb_balises = generer_entier_aleatoire(10, 15);

        // On génére un tableau de nb_balises en taille
        tags = malloc(sizeof(char*) * nb_balises);
        for (int i = 0; i < nb_balises; i++) {
            int length = generer_entier_aleatoire(3, 10);
            tags[i] = malloc(sizeof(char) * length + 1);
            for (int j = 0; j < length; j++) {
                // On génére une lettre aléatoire pour chaque emplacement dans le mot
                tags[i][j] = generer_entier_aleatoire(97, 122);
            }
            // Pour terminer la chaîne de caractères
           tags[i][length] = 0;
        }
    } else {
        nb_balises = atoi(arg);
        if (nb_balises == 0) {
            printf("Nombre de balises invalide");
            exit(EXIT_FAILURE);
        }

        tags = malloc(sizeof(char*) * nb_balises);

        for (int i = 0; i < nb_balises; i++) {
            tags[i] = malloc(MAX_TAG_SIZE * sizeof(char));
            printf("Veuillez rentrer le tag numéro %d -> ", i + 1);
            fgets(tags[i], MAX_TAG_SIZE, stdin);
            printf("\n");
        }
    }
    return envoie_balises_socket(socketfd, nb_balises, tags);
}


/*
 * Surcharge de la méthode envoie balise (pour le formatage et envoi sur socket)
 */
int envoie_balises_socket(int socketfd, int nb_balises, char** balises) {
    char data[1024];
    char buffer[256];
    memset(data, 0, sizeof(data));

    strcpy(data, "balises: ");
    sprintf(buffer, "%d,", nb_balises);
    strcat(data, buffer);

    for (int i = 0; i < nb_balises; i++) {
        if (i != nb_balises - 1) {
            sprintf(buffer, "#%s,", balises[i]);
            strcat(data, buffer);
        } else {
            sprintf(buffer, "#%s", balises[i]);
            strcat(data, buffer);
        }
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

void analyse(char *pathname, char *data, int nb_colors_to_plot)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  strcpy(data, "couleurs: ");
  char temp_string[10];
  sprintf(temp_string, "%d,", nb_colors_to_plot);
  strcat(data, temp_string);

  // choisir n couleurs
  for (count = 1; count < nb_colors_to_plot + 1 && cc->size - count > 0; count++)
  {
    if (cc->compte_bit == BITS32)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    if (cc->compte_bit == BITS24)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc24[cc->size - count].c.vert, cc->cc.cc24[cc->size - count].c.bleu);
    }
    strcat(data, temp_string);
  }

  // enlever le dernier virgule
  data[strlen(data) - 1] = '\0';
}

int envoie_couleurs(int socketfd, char *pathname, char* nb_colors_to_plot)
{

  char data[1024];
  memset(data, 0, sizeof(data));

  if (strcmp(pathname, "random") == 0) {
    // Générer 10 couleurs random
    generation_couleurs_aleatoire(data);
  } else {
    int _nb_colors_to_plot = atoi(nb_colors_to_plot);
    if (_nb_colors_to_plot <= 0 || _nb_colors_to_plot > 30) {
      printf("Nombre de couleurs à plotter invalide.\n");
      exit(EXIT_FAILURE);
    }
    analyse(pathname, data, _nb_colors_to_plot);
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
      sprintf(buffer, "#%x, ", generer_entier_aleatoire(0, MAX_RANDOM_COLOR));
    } else {
      sprintf(buffer, "#%x", generer_entier_aleatoire(0, MAX_RANDOM_COLOR));
    }
    strcat(data, buffer);
  } 

  return 0;
}

int generer_entier_aleatoire(int min, int max) {
    return (rand() % (max - min + 1)) + min;
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
    printf("\n\t bmp: [<chemin vers l'image en bmp>|random (pour les tests de fonctionnalité)] [nb couleurs dominantes, <=30]");
    printf("\n\t hostname: pas d'arguments");
    printf("\n\t calcul: <operateur> <première opérande> <deuxième opérande>");
    printf("\n\t balises: random (génére un nombre aléatoire de balises entre 10 et 15 et les envoie)");
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

  if (strcmp(argv[1], "balises") == 0) {
      envoie_balises(socketfd, argv[2]);
  }
  if (strcmp(argv[1], "bmp") == 0)
  {
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, argv[2], argv[3]);
    
  }

  if (strcmp(argv[1], "json") == 0)
  {
    parse_json_file(argv[2]);
  }

  close(socketfd);
}