# Bibliothèques

<math.h> permet de d'utiliser la fonction racine carré

<thread.h> permet de creer des thread a chaque nouvel utilisateur connecté

# Références

* https://json.org

# Difficulté

* parsé le json 
* faire les conversions de json ⇆ data 
* maintenir une strucutre de code propre
* comprendre comment fonctionne le plot

# Commentaires

## Ajout du champ protocole dans les messages

En prévision de la tâche 4 et dans un soucis d'efficacité, tous les messages que le client envoie au serveur sont désormais débutés par une séquence `%d\n`. Cette
séquence permet au serveur d'identifier le protocole de communication du client et de lui répondre en fonction de celui-ci

## <u>Comment utiliser le code</u>

Lancer le serveur dans un teminale à l'aide de la commande ```./serveur```

Ensuite dans un second terminal il faut lancer la commande ```./client` <type_d'echange> <fonction_requise> <potentielle_arguments>```

Vous pouvez retrouver la syntaxe en tapant simplement `./client`

### Type d'échange
* Si l'on souhaite que la communication client <-> serveur se fasse en json il suffit de passer `--json` en premier paramètre depuis le client. Dans le cas contraire, ne rien écire.

### Fonctions et arguments

 * Fonction **message** : ```./client msg```, ensuite le terminal vous demandera le message.

 * Fonction **hostname** : ```./client hostname```

 * Fonction **calcul** : ```./client calcul <operateur> <opérande> <opérande>```

 * Fonction **calcul complexe** : ```./client calcul <nom_operation> <opérande> <opérande>``` <nom_operation> peut etre minimum, maximum, moyenne, ecart-type

 * Fonction **balises** : ```./client balises random```, random permet de générer directement aleatoirement entre 10 et 15 balises sans avoir à les écrires.

 * Fonction **bmp** : ```./client bmp <chemin_vers_l'image> <nombre_de_couleur_à_analysé>```

### Le serveur peut gerer plusieurs client
