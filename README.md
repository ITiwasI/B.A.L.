# B.A.L.
Project B.A.L. : Création de fonctions tsocks pour communiquuer en C dans un réseau et stocker les messages dans une boite aux lettres.
## Commandes utilisables
Compilation gcc (nom_du_fichier).c -o (nom_de_l'éxécutable)  
```bash
  ./(nom_de_executable) -options (adresse destinataire si nécessaire) (numéro de port)
```
## Options
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
