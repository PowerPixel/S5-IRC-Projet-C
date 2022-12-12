/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Le code côté serveur. L'objectif principal est de recevoir des messages des
 * clients, de traiter ces messages et de répondre aux clients.
 */

#include "serveur.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <math.h>

#include "commons.h"
#include "json.h"

void plot(char *data) {
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
  while (1) {
    char *token = strtok_r(str, ",", &saveptr);
    if (token == NULL) {
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
      fprintf(p, "0 0 10 %d %d 0x%s\n", (count - 1) * (360 / n),
              count * (360 / n), token + 1);
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
int renvoie_message(int client_socket_fd, char *data) {
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  if (data_size < 0) {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}


void tri_a_bulle(double tab[],int size)
{
  int i, j;
  double tmp;
  for (i=0 ; i < size-1; i++)
  {
    for (j=0 ; j < size-i-1; j++)
    {
      /* Pour un ordre décroissant utiliser < */
      if (tab[j] > tab[j+1]) 
      {
        tmp = tab[j];
        tab[j] = tab[j+1];
        tab[j+1] = tmp;
      }
    }
  }
}

double moyenne(double tab[],int size)
{
  double tmp = 0.0;
  for (int i=0 ; i < size; i++)
  {
    tmp = tmp + tab[i];
  }
  tmp = (double)tmp/(double)(size);
  return tmp;
}

double ecart_type (double tab[],int size) {

double moyenne_des_carre = 0.0 ;
double carre_de_moyenne = 0.0;
double tmp = 0.0;

  carre_de_moyenne = moyenne(tab,size) * moyenne(tab,size);

  for (int i=0 ; i < size; i++)
  {
    moyenne_des_carre = moyenne_des_carre + tab[i] * tab[i] ;
  }
  moyenne_des_carre = (double)moyenne_des_carre/(double)(size);

  tmp = moyenne_des_carre - carre_de_moyenne;
  tmp = sqrt(tmp);
  return tmp;

}

/*
 * renvoyer un message (*data) au client (client_socket_fd)
 */

// TODO: We are aware that this command is not 100% compliant to the spec, we'll
// be working on it :)
int renvoie_calcul(char *data, double *result) {
  char operator[10];
  double operand1;
  double operand2;
  char * copy = malloc(strlen(data) + 1); 
  double tab[MAX_ARRAY_SIZE];
  double moy;
  double ecart_type_result;
  char* tmpTab[MAX_ARRAY_SIZE];

  strcpy(copy, data);

  char *delim = " ";
  unsigned count = 0;
  /* First call to strtok should be done with string and delimiter as first and second parameter*/
  char *token = strtok(copy,delim);
  count++;

  /* Consecutive calls to the strtok should be with first parameter as NULL and second parameter as delimiter
    * * return value of the strtok will be the split string based on delimiter*/
  while(token != NULL)
          {
            // if(strcmp(operator, "minimum") == 0|| strcmp(operator, "maximum") == 0 || strcmp(operator, "moyenne") == 0 || strcmp(operator, "ecart-type") ){
            //   if(count > 3 ){
            //     tmpTab[0] = token;
            //   }
            // }
            /* else*/ if (count > 2 ){
                tab[count-3] = atof(token); 
                // printf("tableau[%d] => %lf \n", count-3,tab[count-3]);
              }
            // printf("Token no. %d : %s \n", count,token);
            token = strtok(NULL,delim);
            count++;
          }
  free(copy);

  // if(strcmp(operator, "minimum") || strcmp(operator, "maximum") || strcmp(operator, "moyenne") || strcmp(operator, "ecart-type") ){
  //   char *delim2 = ",";
  //   unsigned count2 = 0;
  //   /* First call to strtok should be done with string and delimiter as first and second parameter*/
  //   char *token2 = strtok(tmpTab[0],delim2);
  //   count2++;
  //   while(token2 != NULL)
  //           {
  //             // printf("Token no. %d : %s \n", count2,token2);
  //             tab[count2-1] = atof(token2);
  //             token2 = strtok(NULL,delim2);
  //             count2++;
  //           }
  //   tri_a_bulle(tab,count2-1); 
  // }
  /*else { */tri_a_bulle(tab,count-3); /*}*/

  // Reading and interpreting operators and operands
  sscanf(data, "%*s %s %lf %lf", operator, & operand1, & operand2);

  // Simple verification of operand
  if (strcmp(operator, "+") == 0) {
    *result = operand1 + operand2;
  } else if (strcmp(operator, "-") == 0) {
    *result = operand1 - operand2;
  } else if (strcmp(operator, "/") == 0) {
    *result = operand1 / operand2;
  } else if (strcmp(operator, "*") == 0) {
    *result = operand1 * operand2;
  } else if (strcmp(operator, "minimum") == 0) {
    *result = tab[0];
  } else if (strcmp(operator, "maximum") == 0) {
    *result = tab[count-4];
  } else if (strcmp(operator, "moyenne") == 0) {
    moy = moyenne(tab,count-3);
    *result = moy;
  } else if (strcmp(operator, "ecart-type") == 0) {
    ecart_type_result = ecart_type(tab,count-3);
    *result = ecart_type_result;
  } else {
    return (EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd) {
  struct sockaddr_in client_addr;
  char data[1024];
  Protocol protocol = Text;
  JSONObject *object;

  unsigned int client_addr_len = sizeof(client_addr);

  // nouvelle connection de client
  int client_socket_fd =
      accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket_fd < 0) {
    perror("accept");
    return (EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, sizeof(data));

  if (data_size < 0) {
    perror("erreur lecture");
    return (EXIT_FAILURE);
  }

  // Separate protocol and data
  sscanf(data, "%d\n%[^\n]", (int *)&protocol, data);

  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message: "
   * ou un autre mot.
   */
  printf("Message recu: %s\n", data);

  if (protocol == JSON) {
    object = parse_json(data);
    // TODO: because of protocol specifications inconcistencies, we need a
    // special conversion to Text format for calcul operations... calcul
    // operation doesn't have commas separated values ¯\_(ツ)_/¯ (see
    // https://github.com/johnsamuelwrites/AlgoC/blob/master/Projet/partie1.md#c-calcul
    // vs
    // https://github.com/johnsamuelwrites/AlgoC/blob/master/Projet/partie1.md#e-balises)
    if (strcmp((char *)object->fields[0]->value.string, "calcul") == 0) {
      json_to_text_calcul(object, data);
    } else {
      json_to_text(object, data);
    }
  }
  char code[10];
  sscanf(data, "%s", code);
  int isMessage = strcmp(code, "message:") == 0;
  int isNom = strcmp(code, "nom:") == 0;

  // If the command is message or name, we just need to echo back to the client
  if (isMessage || isNom) {
    // Protocol based data processing, need a protocol check
    if (protocol == JSON) {
      JSONObject *_object = NULL;
      JSONArray *args = create_array();
      char resultat[MAX_STRING_SIZE];
      sscanf(data, "%*s %s", resultat);
      memset(data, 0, sizeof(data));
      insert_str_into_array(resultat, args);

      // We need to put a code based on the command
      if (isNom) {
        _object = create_json_object("nom", args);
      } else if (isMessage) {
        _object = create_json_object("message", args);
      }

      // Turn the JSON Object into a string
      convert_to_data(data, _object);
    }
    // sends the message back to the client
    renvoie_message(client_socket_fd, data);
  } else if (strcmp(code, "calcul:") == 0) {
    double result = 0.0;
    char resultat[MAX_STRING_SIZE];
    renvoie_calcul(data, &result);
    memset(data, 0, sizeof(data));


    if (protocol == JSON) {
      // printf("bonjour %lf", result);
      JSONArray *args = create_array();
      sprintf(resultat, "%lf", result);
      insert_str_into_array(resultat, args);
      JSONObject *_object = create_json_object("calcul", args);
      convert_to_data(data, _object);
    } else {
      sprintf(data, "calcul: %lf", result);
    }

    renvoie_message(client_socket_fd, data);
  } else if (strcmp(code, "balises:") == 0) {
    recois_balises(data);
    memset(data, 0, sizeof(data));
    if (protocol == JSON) {
      JSONArray *args = create_array();
      insert_str_into_array("enregistrées", args);
      JSONObject *_object = create_json_object("balises", args);
      convert_to_data(data, _object);
      renvoie_message(client_socket_fd, data);
    } else {
      renvoie_message(client_socket_fd, "balises: enregistrées");
    }

  } else {
    plot(data);
    memset(data, 0, sizeof(data));

    if (protocol == JSON) {
      JSONArray *args = create_array();
      insert_str_into_array("enregistrées", args);
      JSONObject *_object = create_json_object("couleurs", args);
      convert_to_data(data, _object);
    }
    renvoie_message(client_socket_fd, data);
  }

  // fermer le socket
  close(socketfd);
  return (EXIT_SUCCESS);
}

int recois_balises(char *data) {
  if (access("tags", F_OK) == 0) {
    remove("tags");
  }
  FILE *save_file_for_tags = fopen("tags", "w");
  if (save_file_for_tags == NULL) {
    perror("Cannot open save tags file");
    exit(EXIT_FAILURE);
  }

  int count = 0;
  int n;
  char *saveptr = NULL;
  char *str = data;
  while (1) {
    char *token = strtok_r(str, ",", &saveptr);
    if (token == NULL) {
      break;
    }
    str = NULL;
    printf("%d: %s\n", count, token);
    if (count >= 1) {
      // Sauvegarde des couleurs
      fprintf(save_file_for_tags, "%s\n", token);
    }
    if (count == 1) {
      n = atoi(token);
      printf("n = %d\n", n);
    }
    count++;
  }
  return 0;
}

int main() {
  int socketfd;
  int bind_status;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0) {
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
  bind_status =
      bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0) {
    perror("bind");
    return (EXIT_FAILURE);
  }

  // Écouter les messages envoyés par le client
  listen(socketfd, 10);

  // Lire et répondre au client
  recois_envoie_message(socketfd);

  return 0;
}
