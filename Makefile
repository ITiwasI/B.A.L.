# Fichier source
SRC = tsock-v3.c

# Nom de l'exécutable
EXEC = tsock-v3

# Règle par défaut (cible principale)
all: $(EXEC)

# Règle pour créer l'exécutable
$(EXEC): $(SRC)
	gcc  $(SRC) -o $(EXEC)
