Aletrut Thibaut  
Vaillant Thomas  
# B.A.L.
Projet B.A.L. : Création de fonctions tsocks pour communiquuer en C dans un réseau et stocker les messages dans une boite aux lettres.
## Commandes utilisables
Compilation gcc (nom_du_fichier).c -o (nom_de_l'éxécutable)  
```bash
  ./(nom_de_executable) -options (adresse destinataire si nécessaire) (numéro de port)
```
## Options
```bash
  usage: cmd [-b|-r|([-p|-s|-e][-n ##][-l ##])]
```

### Tsocks
Activer tcp/udp  
TCP par défaut, sinon option  
```bash
  ./(nom_de_executable) -u
```  
Cette option s'accompagne du type : Source ou puits  
Source
```bash
  ./(nom_de_executable) -s
```
Puits
```bash
  ./(nom_de_executable) -p
```
Pour gérer le nombre de message envoyé ou lu  
```bash
  ./(nom_de_executable) -n (nombre de messages)
```
(défaut : 10)    
Pour gérer la taille d'un message envoyé ou lu  
```bash
  ./(nom_de_executable) -l (Taille d un message)
```
(défaut : 30)  
### B.A.L.
B.A.L.
```bash
  ./(nom_de_executable) -b
```
Emetteur
```bash
  ./(nom_de_executable) -e (id recepteur)
```
Récepteur
```bash
  ./(nom_de_executable) -r (id recepteur)
```
Pour gérer le nombre de lettres envoyées  
```bash
  ./(nom_de_executable) -n (nombre de lettres)
```
(défaut : 1)    
Pour gérer la taille d'une lettre envoyée 
```bash
  ./(nom_de_executable) -l (Taille d une lettre)
```
## Fonctionnement
La b.a.l. se lance et se met en attente de connexion. A une demande de connection, celle-ci attend une tramme renseignant le type : emmetteur 0 ou recepteur 1 et si emetteur le nombre et la taille des lettres.  
Lorsqu'un emetteur envoie sa trame, puis ses lettres, la b.A.L. enregistre dans une double liste de cellules les lettres selon l'id du récepteur.  
Lorsqu'un recepteur envoie sa trame, il reçoit succéssivement la taille des lettres puis la lettre, avec 0 pour arreter et -1 s'il n'y a pas de lettres à son id.

Le seul détail est qu'un fork ne permet pas facilement de modifier un processus antérieur et donc la gestion des clients se fait un par un.  
