
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code pour traiter différents types d'opérations mathématiques
 */
#include "operations.h"
#include <math.h>

double moyenne(double tab[], int size)
{
    double tmp = 0.0;
    for (int i = 0; i < size; i++)
    {
        tmp = tmp + tab[i];
    }
    tmp = (double)tmp / (double)(size);
    return tmp;
}

double ecart_type(double tab[], int size)
{

    double moyenne_des_carre = 0.0;
    double carre_de_moyenne = 0.0;
    double tmp = 0.0;

    carre_de_moyenne = moyenne(tab, size) * moyenne(tab, size);

    for (int i = 0; i < size; i++)
    {
        moyenne_des_carre = moyenne_des_carre + tab[i] * tab[i];
    }
    moyenne_des_carre = (double)moyenne_des_carre / (double)(size);

    tmp = moyenne_des_carre - carre_de_moyenne;
    tmp = sqrt(tmp);
    return tmp;
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