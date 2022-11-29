/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code du côté client. Le but principal de ce code est de communiquer avec le
 * serveur, d'envoyer et de recevoir des messages. Ces messages peuvent être du
 * simple texte ou des chiffres.
 */

#include "client.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "bmp.h"
#include "commons.h"
#include "json.h"

void parse_json_file(char *json_filename) {
  int file = open(json_filename, O_RDONLY);
  int len = lseek(file, 0, SEEK_END);
  void *data = mmap(0, len, PROT_READ, MAP_PRIVATE, file, 0);
  JSONObject *object = parse_json(data);
  print_json_object(object);
}

char *get_hostname() {
  int ret;
  char *hostname;
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

int envoie_recois_message(int socketfd, Protocol protocol) {
  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
  fgets(message, sizeof(message), stdin);
  message[strlen(message) - 1] = '\0';
  sprintf(data, "%d\n", protocol);

  // We check which protocol we're using, and format it accordingly
  if (protocol == Text) {
    sprintf(data + strlen(data), "message: ");
    sprintf(data + strlen(data), "%s", message);
  } else if (protocol == JSON) {
    JSONArray *arguments = create_array();
    insert_str_into_array(message, arguments);
    JSONObject *object = create_json_object("message", arguments);
    print_json_object(object);
    convert_to_data(data, object);
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_recois_hostname(int socketfd, Protocol protocol) {
  JSONObject *object;
  char data[MAX_HOSTNAME_SIZE];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));
  sprintf(data, "%d\n", protocol);
  if (protocol == Text) {
    sprintf(data + strlen(data), "nom: ");
    sprintf(data + strlen(data), "%s", get_hostname());
  } else if (protocol == JSON) {
    JSONArray *arguments = create_array();
    insert_str_into_array(get_hostname(), arguments);
    object = create_json_object("nom", arguments);

    convert_to_data(data, object);
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int envoie_calcul_recois_resultat(int socketfd, Protocol protocol,
                                  char *operator, char * operand1,
                                  char * operand2) {
  char data[MAX_HOSTNAME_SIZE];
  char resultat[MAX_HOSTNAME_SIZE];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  sprintf(data, "%d\n", protocol);
  if (protocol == Text) {
    sprintf(data + strlen(data), "calcul: ");
    sprintf(data + strlen(data), "%s", operator);
    sprintf(data + strlen(data), "%s", " ");
    sprintf(data + strlen(data), "%s", operand1);
    sprintf(data + strlen(data), "%s", " ");
    sprintf(data + strlen(data), "%s", operand2);
  } else if (protocol == JSON) {
    JSONArray *arguments = create_array();
    insert_str_into_array(operator, arguments);
    int *_operand1 = malloc(sizeof(int));
    int *_operand2 = malloc(sizeof(int));

    int tmp = atoi(operand1);
    memcpy(_operand1, &tmp, sizeof(int));
    insert_int_into_array(_operand1, arguments);

    tmp = atoi(operand2);
    memcpy(_operand2, &tmp, sizeof(int));
    insert_int_into_array(_operand2, arguments);

    JSONObject *object = create_json_object("calcul", arguments);

    convert_to_data(data, object);
  }

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0) {
    perror("erreur calcul!!!!!");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, resultat, sizeof(resultat));
  if (read_status < 0) {
    perror("erreur lecture resultat");
    return -1;
  }

  printf("Message recu: %s\n", resultat);

  return 0;
}

int envoie_balises(int socketfd, Protocol protocol, char *arg) {
  int nb_balises;
  char **tags;

  if (strcmp(arg, "random") == 0) {
    nb_balises = generer_entier_aleatoire(10, 15);

    // On génére un tableau de nb_balises en taille
    tags = malloc(sizeof(char *) * nb_balises);
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

    tags = malloc(sizeof(char *) * nb_balises);

    for (int i = 0; i < nb_balises; i++) {
      tags[i] = malloc(MAX_TAG_SIZE * sizeof(char));
      printf("Veuillez rentrer le tag numéro %d -> ", i + 1);
      fgets(tags[i], MAX_TAG_SIZE, stdin);
      tags[i][strlen(tags[i]) - 1] = '\0';
      printf("\n");
    }
  }
  return envoie_balises_socket(socketfd, protocol, nb_balises, tags);
}

/*
 * Surcharge de la méthode envoie balise (pour le formatage et envoi sur socket)
 */
int envoie_balises_socket(int socketfd, Protocol protocol, int nb_balises,
                          char **balises) {
  char data[1024];
  char buffer[256];
  memset(data, 0, sizeof(data));
  sprintf(data, "%d\n", protocol);
  if (protocol == Text) {
    strcat(data, "balises: ");
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
  } else if (protocol == JSON) {
    JSONObject *object;
    JSONArray *arguments = create_array();
    int *_nb_balises = malloc(sizeof(int));
    memcpy(_nb_balises, &nb_balises, sizeof(int));
    insert_int_into_array(_nb_balises, arguments);
    for (int i = 0; i < nb_balises; i++) {
      insert_str_into_array(balises[i], arguments);
    }

    object = create_json_object("balises", arguments);
    convert_to_data(data, object);
  }

  int write_status = write(socketfd, data, strlen(data));
  memset(data, 0, sizeof(data));
  if (write_status < 0) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

void analyse(char *pathname, Protocol protocol, char *data,
             int nb_colors_to_plot) {
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  char temp_string[10];
  JSONArray *array;

  // We give the protocol used to the server
  sprintf(data, "%d\n", protocol);

  // If the protocol is text, we need specific headers
  if (protocol == Text) {
    sprintf(data + strlen(data), "couleurs: ");
    sprintf(temp_string, "%d,", nb_colors_to_plot);
    sprintf(data + strlen(data), "%s", temp_string);
  } else if (protocol == JSON) {
    array = create_array();
    int *nb_colors = malloc(sizeof(int));
    memcpy(nb_colors, &nb_colors_to_plot, sizeof(int));
    insert_int_into_array(nb_colors, array);
  }

  // choisir n couleurs
  for (count = 1; count < nb_colors_to_plot + 1 && cc->size - count > 0;
       count++) {
    if (cc->compte_bit == BITS32) {
      if (protocol == Text) {
        sprintf(temp_string, "#%02x%02x%02x,",
                cc->cc.cc32[cc->size - count].c.rouge,
                cc->cc.cc32[cc->size - count].c.vert,
                cc->cc.cc32[cc->size - count].c.bleu);
        sprintf(data + strlen(data), "%s", temp_string);
      } else if (protocol == JSON) {
        char *string =
            malloc(sizeof(char) *
                   8);  // 8 chars (7 for colors, 1 for the 0 of ending string)
        sprintf(string, "#%02x%02x%02x", cc->cc.cc32[cc->size - count].c.rouge,
                cc->cc.cc32[cc->size - count].c.vert,
                cc->cc.cc32[cc->size - count].c.bleu);
        insert_str_into_array(string, array);
      }
    }
    if (cc->compte_bit == BITS24) {
      if (protocol == Text) {
        sprintf(temp_string, "#%02x%02x%02x,",
                cc->cc.cc24[cc->size - count].c.rouge,
                cc->cc.cc24[cc->size - count].c.vert,
                cc->cc.cc24[cc->size - count].c.bleu);
        sprintf(data + strlen(data), "%s", temp_string);
      } else if (protocol == JSON) {
        char *string =
            malloc(sizeof(char) *
                   8);  // 8 chars (7 for colors, 1 for the 0 of ending string)
        sprintf(string, "#%02x%02x%02x", cc->cc.cc24[cc->size - count].c.rouge,
                cc->cc.cc24[cc->size - count].c.vert,
                cc->cc.cc24[cc->size - count].c.bleu);
        insert_str_into_array(string, array);
      }
    }
  }

  if (protocol == JSON) {
    JSONObject *object = create_json_object("couleurs", array);
    convert_to_data(data, object);
  }
  if (protocol == Text) {
    // enlever le dernier virgule
    data[strlen(data) - 1] = '\0';
  }
}

int envoie_couleurs(int socketfd, Protocol protocol, char *pathname,
                    char *nb_colors_to_plot) {
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
    analyse(pathname, protocol, data, _nb_colors_to_plot);
  }

  int write_status = write(socketfd, data, strlen(data));
  memset(data, 0, sizeof(data));
  if (write_status < 0) {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0) {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}

int generation_couleurs_aleatoire(char *data) {
  int nb_colors = rand() % (20 + 1 - 10) + 10;
  char buffer[1024];
  // Ecriture des données (donc nb couleurs puis chaque couleur
  // individuellement)
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

int main(int argc, char **argv) {
  int socketfd;
  Protocol parse_mode = Text;

  struct sockaddr_in server_addr;

  if (argc < 2) {
    printf("usage: ./client [--json] [msg|hostname|bmp|calcul] <arguments>\n");
    printf("Arguments: ");
    printf("\n\t msg: pas d'arguments");
    printf(
        "\n\t bmp: [<chemin vers l'image en bmp>|random (pour les tests de "
        "fonctionnalité)] [nb couleurs dominantes, <=30]");
    printf("\n\t hostname: pas d'arguments");
    printf("\n\t calcul: <operateur> <première opérande> <deuxième opérande>");
    printf(
        "\n\t balises: random (génére un nombre aléatoire de balises entre 10 "
        "et 15 et les envoie)");
    printf("\n");
    return (EXIT_FAILURE);
  }

  if (strcmp(argv[1], "--json") == 0) {
    parse_mode = JSON;
    argv++;
  }

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status =
      connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0) {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "msg") == 0) {
    // envoyer et recevoir un message
    envoie_recois_message(socketfd, parse_mode);
  }

  if (strcmp(argv[1], "hostname") == 0) {
    envoie_recois_hostname(socketfd, parse_mode);
  }

  if (strcmp(argv[1], "calcul") == 0) {
      // if (strcmp(argv[2], "minimum") == 0 || strcmp(argv[2], "moyenne") == 0 ||
      // strcmp(argv[2], "maximum") == 0 || strcmp(argv[2], "écart-type") == 0){
      //   envoie_calcul_recois_resultat(socketfd, parse_mode, argv[2], argv[3],
      //                             argv[4]);
      // }
      // else {
      envoie_calcul_recois_resultat(socketfd, parse_mode, argv[2], argv[3],
                                  argv[4]);
      // }
    
  }

  if (strcmp(argv[1], "balises") == 0) {
    envoie_balises(socketfd, parse_mode, argv[2]);
  }

  if (strcmp(argv[1], "bmp") == 0) {
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    envoie_couleurs(socketfd, parse_mode, argv[2], argv[3]);
  }

  close(socketfd);
}
