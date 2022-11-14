/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Le code côté serveur. L'objectif principal est de recevoir des messages des clients,
 * de traiter ces messages et de répondre aux clients.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serveur.h"

void plot(char *data)
{

  // Extraire le compteur et les couleurs RGB
  FILE *p = popen("gnuplot -persist", "w");

  if (access("colors", F_OK) == 0) {
    remove("colors");
  }  
  FILE *save_file_for_colors = fopen("colors", "w");
  if (save_file_for_colors == NULL) {
    perror("Cannot open save colors file");
    exit(EXIT_FAILURE);
  }
  printf("Plot\n");
  int count = 0;
  int n;
  char *saveptr = NULL;
  char *str = data;
  fprintf(p, "set xrange [-15:15]\n");
  fprintf(p, "set yrange [-15:15]\n");
  fprintf(p, "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "set title 'Top 10 colors'\n");
  fprintf(p, "plot '-' with circles lc rgbcolor variable\n");
  while (1)
  {
    char *token = strtok_r(str, ",", &saveptr);
    if (token == NULL)
    {
      break;
    }
    str = NULL;
    if (count == 0) {
      int *nb;
      nb = malloc(sizeof(int));
      printf("n token = %s\n", token);
      // couleurs: n
      sscanf(token, "%*s%d\n", nb);
      printf("nb = %d\n", *nb);
      n = *nb;
    } else {
          printf("%d: %s\n", count, token);
    }

    if (count >= 1) {
      // Sauvegarde des couleurs
      fprintf(save_file_for_colors, "%s\n", token);
      // Le numéro 36, parceque 360° (cercle) / 10 couleurs = 36
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count - 1) * (360/n), count * (360/n), token + 1);
    }
    count++;
  }
  
  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);
  fclose(save_file_for_colors);
}

/* renvoyer un message (*data) au client (cli ent_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_calcul(int client_socket_fd, char *data)
{

  double result;
  // Lecture et interprétation de l'opérande

  char operator;
  double operand1;
  double operand2;

  sscanf(data, "%*s %s %lf %lf", &operator, &operand1, &operand2);
  if (strcmp(&operator, "+") == 0) {
    result = operand1 + operand2;
  } else if (strcmp(&operator, "-") == 0) {
    result = operand1 - operand2;
  } else if (strcmp(&operator, "/") == 0) {
    result = operand1 / operand2;
  } else if (strcmp(&operator, "*") == 0) {
    result = operand1 * operand2;
  } else {
    return (EXIT_FAILURE);
  }

  int data_size = write(client_socket_fd, (void *)&result, sizeof(double));

  if (data_size < 0) {
    perror("erreur calcul !!!!!!");
    return (EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd)
{
  struct sockaddr_in client_addr;
  char data[1024];

  unsigned int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket_fd < 0)
  {
    perror("accept");
    return (EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, sizeof(data));

  if (data_size < 0)
  {
    perror("erreur lecture");
    return (EXIT_FAILURE);
  }

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Message recu: %s\n", data);
  char code[10];
  sscanf(data, "%s", code);

  // Si le message commence par le mot: 'message:'
  if (strcmp(code, "message:") == 0 || strcmp(code, "nom:") == 0)
  {
    renvoie_message(client_socket_fd, data);
  }

  if (strcmp(code, "calcul:") == 0) {
    renvoie_calcul(client_socket_fd, data);
  }

  if (strcmp(code, "balises:") == 0) {
      recois_balises( data);
      renvoie_message(client_socket_fd, "balises: enregistré");
  }

  else
  {
    plot(data);
    renvoie_message(client_socket_fd, "couleurs: enregistré");
  }

  // fermer le socket
  close(socketfd);
  return (EXIT_SUCCESS);
}

int recois_balises(char* data) {
    if (access("tags", F_OK) == 0) {
        remove("tags");
    }
    FILE *save_file_for_tags= fopen("tags", "w");
    if (save_file_for_tags == NULL) {
        perror("Cannot open save tags file");
        exit(EXIT_FAILURE);
    }

    int count = 0;
    int n;
    char *saveptr = NULL;
    char *str = data;
    while (1)
    {
        char *token = strtok_r(str, ",", &saveptr);
        if (token == NULL)
        {
            break;
        }
        str = NULL;
        printf("%d: %s\n", count, token);
        if (count >= 1) {
            // Sauvegarde des couleurs
            fprintf(save_file_for_tags, "%s\n", token);
        }
        if (count == 1)
        {
            n = atoi(token);
            printf("n = %d\n", n);
        }
        count++;
    }
    return 0;
}

int main()
{

  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0)
  {
    perror("bind");
    return (EXIT_FAILURE);
  }

  // Écouter les messages envoyés par le client
  listen(socketfd, 10);

  // Lire et répondre au client
  recois_envoie_message(socketfd);

  return 0;
}
