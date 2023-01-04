CC ?= gcc

# Modifiez les deux lignes suivantes et ajoutez de nouveaux fichiers selon vos besoins
COBJS ?= client.o bmp.o couleur.o json.o validation.o testes.o
SOBJS ?= serveur.o json.o validation.o testes.o 

# Avertissements
CFLAGS ?= -Wall -Wextra
# Bibliothèques
LIBS ?= -lm -lpthread

# Déboggage
DEBUG ?= -ggdb3

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client

all: $(SERVER) $(CLIENT)

# Création d'un exécutable pour le côté serveur
$(SERVER): $(SOBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(SERVER) $(SOBJS) $(LIBS)

# Création d'un exécutable pour le côté client
$(CLIENT): $(COBJS)
	$(CC) $(DEBUG) $(CFLAGS) -o $(CLIENT) $(COBJS) $(LIBS)

# Compilation du code sans création d'un exécutable
.c.o: 
	$(CC) $(DEBUG) $(CFLAGS) -c $*.c


clean:
	rm -rf *.o $(SERVER) $(CLIENT) colors tags
