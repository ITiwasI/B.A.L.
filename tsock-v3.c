/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <string.h>

struct hostent* hp;
struct sockaddr_in adr_local;
struct sockaddr_in adr_distant;

typedef struct synchro synchro;
struct synchro {
	int emetteur_ou_recepteur; //0 emetteur ou 1 recepteur
	int identifiant;//identifiant du recepteur
	int nb_lettres;
	int taille_lettre;
};

#define lg_max 10000
typedef struct lettre lettre;
struct lettre {
	char* message;
	int taille_lettre;
	lettre* ptr_next;
};

typedef struct boite_recep boite_recep;
struct boite_recep{
	int identifiant_recep;
	lettre* premiere_lettre;
	boite_recep* ptr_next_boite;
};

void puit_udp(int n_port, int nb_message);
void source_udp(int n_port, char* nom_domaine, int nb_message, int lg_msg);
void puit_tcp(int n_port,int nb_message,int lg_msg);
void source_tcp(int n_port, char* nom_domaine, int nb_message, int lg_msg);
void recepteur_bal(int n_port, char* nom_domaine, synchro* p_trame);
void emetteur_bal(int n_port,char* nom_domaine, synchro* p_trame);
void boite_bal(int n_port);


int main (int argc, char **argv){
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int source = -1 ; /* 0=puits, 1=source */
	int tcp = 1 ; /* O = udp, 1 = tcp */
	int lg_msg = -1;
	int bal=-1;
	synchro* p_trame = malloc(sizeof(synchro));
	p_trame->emetteur_ou_recepteur=-1;
	while ((c = getopt(argc, argv, "bpn:sul:e:r:")) != -1) {
		switch (c) {
		case 'b':
			if (p_trame->emetteur_ou_recepteur==1||source!=-1){
				printf("usage: cmd [-b|-r|([-p|-s|-e][-n ##][-l ##])]\n");
				exit(1);
			}
			bal=1;
			break;
		case 'e':
		    if (p_trame->emetteur_ou_recepteur==1||source!=-1||bal!=-1){
				printf("usage: cmd [-b|-r|([-p|-s|-e][-n ##][-l ##])]\n");
				exit(1);
			}
			p_trame->emetteur_ou_recepteur=0;
			p_trame->identifiant= atoi(optarg);
			break;

		case 'r':
		    if (p_trame->emetteur_ou_recepteur==1||source!=-1||bal!=-1){
				printf("usage: cmd [-b|-r|([-p|-s|-e][-n ##][-l ##])]\n");
				exit(1);
			}
			p_trame->emetteur_ou_recepteur=1;
			p_trame->identifiant= atoi(optarg);
			break;
			
		case 'p':
			if (source == 1||p_trame->emetteur_ou_recepteur!=-1||bal!=-1) {
				printf("usage: cmd [-b|-r|([-p|-s|-e][-n ##][-l ##])]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0||p_trame->emetteur_ou_recepteur!=-1||bal!=-1) {
				printf("usage: cmd [-b|-r|([-p|-s|-e][-n ##][-l ##])]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		case 'u':
			tcp = 0;
			break;

		case 'l':
			lg_msg=atoi(optarg);
			break;

		default:
			printf("usage: cmd [-b|-r|([-p|-s|-e][-n ##][-l ##])]\n");
			break;
		}
	}


	

	
	int n_port=atoi(argv[argc-1]);
	char* nom_domaine = argv[argc-2];
	if (bal==-1&&p_trame->emetteur_ou_recepteur==-1){
		if (nb_message != -1&&source != -1) {
			if (source == 1)
				printf("nb de tampons à envoyer : %d\n", nb_message);
			else
				printf("nb de tampons à recevoir : %d\n", nb_message);
		} else {
			if (source == 1) {
				nb_message = 10 ;
				printf("nb de tampons à envoyer = 10 par défaut\n");
			} else
			printf("nb de tampons à recevoir = infini\n");
	
		}
		if (source == 1)
			printf("Mode : source\n");
		if (source==0 )
			printf("Mode : puits\n");

		if (lg_msg != -1&&source !=-1) {
			if (source == 1)
				printf("Taille de tampons à envoyer : %d\n", lg_msg);
			else
				printf("Taille de tampons à recevoir : %d\n", lg_msg);
			} else {
			lg_msg = 30 ;
			if (source == 1) {
				printf("Taille de tampons à envoyer : 30 par défaut\n");
			} else
			printf("Taille de tampons à recevoir : 30\n");
		}
		
		if (tcp == 0) {
			printf("Type d'envoi : udp\n");
			if (source == 0){
				puit_udp(n_port,nb_message);
			}
			else{
				source_udp(n_port,nom_domaine,nb_message,lg_msg);
			}
		}
		else{
			printf("Type d'envoi : tcp\n");
			if (source == 0){
				puit_tcp(n_port,nb_message,lg_msg);
			}
			else{
				source_tcp(n_port,nom_domaine,nb_message,lg_msg);
			}
		}
	}
	if (p_trame->emetteur_ou_recepteur==0){
		if (nb_message==-1){
			nb_message=1;
			p_trame->nb_lettres=nb_message;
			printf("1 lettre à envoyer\n");
		}else{
			p_trame->nb_lettres=nb_message;
			printf("%d lettre(s) à envoyer\n",nb_message);
		}
		if (lg_msg==-1){
			lg_msg=30;
			p_trame->taille_lettre=lg_msg;
			printf("Taille de lettre fixée à 30\n");
		}else{
			p_trame->taille_lettre=lg_msg;
			printf("Taille de lettre : %d\n",lg_msg);
		}
		emetteur_bal(n_port, nom_domaine, p_trame);
	}	
	if(p_trame->emetteur_ou_recepteur==1){
		recepteur_bal(n_port, nom_domaine, p_trame);
	}	
	if (bal==1){
		boite_bal(n_port);
	}	
	free(p_trame);
	return 0;
}


void puit_udp(int n_port, int nb_message){
	/*creation socket*/
	int sock_local = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
	int lg_adr_local=sizeof(adr_local);
	if((sock_local=socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP))==-1){
		printf("echec de creation du socket\n");
		exit(1);
	}
	/*Constrtuction de l'adresse*/
	memset((char*) &adr_local, 0 , sizeof(adr_local));
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = htons(n_port);
	adr_local.sin_addr.s_addr=INADDR_ANY;

	/*association de l'adresse au socket*/
	if(bind(sock_local, (struct sockaddr*) &adr_local,lg_adr_local)==-1){
		printf("echec du bind\n");
		exit(1);
	}
	printf("Succes du bind\n");

	struct sockaddr* padr_em=malloc(sizeof(struct sockaddr));
	unsigned int* plg_adr_em=malloc(sizeof(int));

	/* Etat de reception*/
	int nmb=0;
	while(nb_message==-1||nmb<nb_message ){
		/* alocation d'espace de reception pour le message et l'adresse destinataire*/
		char * pmesg=malloc(lg_max * sizeof(char));

		int nb_octet_lu = recvfrom(sock_local,pmesg,lg_max,0,padr_em,plg_adr_em);
		printf("Message %d :",nmb+1);
		printf("%s\n", pmesg);
	

		if (nb_octet_lu == -1){
			printf("erreur de reception");
		}
	

		/* liberation d'espace du message */

		free(pmesg);
		nmb+=1;
	}
	free(padr_em);
	free(plg_adr_em); 
}



int construire_messages(char* pmesg, int lg_msg, int nb_message) {
    int lg_mesg = 0;
    char alphabet[28] = {'+', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\0'};
    
    for (int n = 0; n < nb_message; n++) {
        char chiffres[6];
        chiffres[6] = '\0';
        int q = n + 1;
        int r = 0;

        for (int i = 0; i < 5; i++) {
            r = q % 10;
            q = q / 10;
            chiffres[4 - i] = (char)r + 48; // 48 is '0' in ASCII
        }

        int j = 0;
        while (j < 5 && chiffres[j] == '0') {
            chiffres[j] = ' ';
            j += 1;
        }

        for (int i = 0; i < lg_msg; i++) {
            if (i < 5) {
                pmesg[n * lg_msg + i] = chiffres[i];
            } else {
                pmesg[n * lg_msg + i] = alphabet[(n % 26) + 1];
            }
            lg_mesg += 1;
        }

        // Print the individual message
        printf("Message %d : ", n + 1);
        for (int i = 0; i < lg_msg; i++) {
            printf("%c", pmesg[n * lg_msg + i]);
        }
        printf("\n");
    }

    pmesg[nb_message * lg_msg] = '\0';
    return lg_mesg;
}


void source_udp(int n_port, char* nom_domaine, int nb_message, int lg_msg){
	//creation socket
	int sock_local = socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP);
	if((sock_local=socket(AF_INET , SOCK_DGRAM , IPPROTO_UDP))==-1){
		printf("echec de creation du socket\n");
		exit(1);
	}
	//création adresse de socket distant
	memset((char *) &adr_distant, 0 ,sizeof(adr_distant));
	adr_distant.sin_family = AF_INET;
	adr_distant.sin_port = htons(n_port);//port

	if((hp = gethostbyname(nom_domaine))==NULL){
		printf("erreur gethostby name");
		exit(1);
	}
	memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);//ip

	char * pmesg=malloc(lg_msg* nb_message * sizeof(char));
	int lg_mesg = construire_messages(pmesg,lg_msg,nb_message);
	for(int n=0;n<nb_message;n++){
		int lg_emis = sendto(sock_local,&pmesg[n*lg_msg],lg_msg,0,(struct sockaddr*)&adr_distant,sizeof(adr_distant));
	}
}



void puit_tcp(int n_port,int nb_message,int lg_msg){
	//creation socket
	int sock_local;
	int lg_adr_local=sizeof(adr_local);
	if((sock_local=socket(AF_INET , SOCK_STREAM , IPPROTO_TCP))==-1){
		printf("echec de creation du socket\n");
		exit(1);
	}
	//Construction de l'adresse
	memset((char*) &adr_local, 0 , sizeof(adr_local));
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = htons(n_port);
	adr_local.sin_addr.s_addr=INADDR_ANY;

	//association de l'adresse au socket
	if(bind(sock_local, (struct sockaddr*) &adr_local,lg_adr_local)==-1){
		printf("echec du bind\n");
		exit(1);
	}
	printf("Succes du bind\n");

	/*dimensionnement de la file de demande de connexion ici 5 à la fois*/
	listen(sock_local,5);

	
	/*affectaion de la demande à un socket fils (avec verification)*/
	int sock_local_bis=0;
	/* adresse emetteur*/
	struct sockaddr* padr_em=malloc(sizeof(struct sockaddr));
	unsigned int* plg_adr_em=malloc(sizeof(int));
	if((sock_local_bis=accept(sock_local,(struct sockaddr *) padr_em,plg_adr_em))==-1){
		printf("echec du accept\n");
		exit(1);
	}

	int nb_octet_lu=0;
	switch(fork()){
		case -1: /*erreur du fork*/
			printf("erreur fork\n");
			exit(1);
		case 0: /*socket fils actif*/
			close(sock_local);
			

			/* alocation d'espace de reception pour le message */
			char * pmesg=malloc(lg_max * sizeof(char));
			int nb_msg = 1;
			while ((nb_octet_lu = read(sock_local_bis,pmesg,lg_msg))>0){
				printf("%d - %s\n",nb_msg, pmesg);
				nb_msg ++;
			}
			free(pmesg);
			exit(0);

		default : /* dans le socket mère */
			close(sock_local_bis);
			exit(0);
	}
	/* liberation d'espace*/
		
	free(padr_em);
	free(plg_adr_em);
}
void source_tcp(int n_port, char* nom_domaine, int nb_message, int lg_msg){
	//creation socket
	int sock_local;
	if((sock_local=socket(AF_INET , SOCK_STREAM , IPPROTO_TCP))==-1){
		printf("echec de creation du socket\n");
		exit(1);
	}

	//création adresse de socket distant
	memset((char *) &adr_distant, 0 ,sizeof(adr_distant));
	adr_distant.sin_family = AF_INET;
	adr_distant.sin_port = htons(n_port);//port

	if((hp = gethostbyname(nom_domaine))==NULL){
		printf("erreur gethostby name");
		exit(1);
	}
	memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);//ip

	//connection
	int etat=-1;
	while(etat==-1){
		etat=connect(sock_local,(struct sockaddr*)&adr_distant,sizeof(adr_distant));
	}
	printf("connecté\n");

	//creation message
	char * pmesg=malloc(lg_max * sizeof(char));

	int lg_mesg= construire_messages(pmesg,lg_msg,nb_message);
	if(lg_mesg >lg_max){
		printf("Message trop long");
	}
	
	etat=-1;
	while(etat==-1){
		etat=send(sock_local,pmesg,lg_mesg,0); //0 ou MSG_OOB urgent
	}
	
	if (close(sock_local)==-1){
		printf("echec fermeture d'emission\n");
	}
}

int construire_lettres(char* pmesg, int lg_msg, int nb_message, int recepteur_vise) {
    int lg_mesg = 0;
    char alphabet[28] = {'+', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '\0'};
    char chiffres[6];
    chiffres[6] = '\0';
    int q = recepteur_vise;
    int r = 0;

    // Préparation des chiffres pour le récepteur visé
    for (int i = 0; i < 5; i++) {
        r = q % 10;
        q = q / 10;
        chiffres[4 - i] = (char)r + 48; // 48 est '0' en ASCII
    }

    for (int n = 0; n < nb_message; n++) {
        int j = 0;
        while (j < 5 && chiffres[j] == '0') {
            chiffres[j] = ' ';
            j += 1;
        }

        for (int i = 0; i < lg_msg; i++) {
            if (i < 5) {
                pmesg[n * lg_msg + i] = chiffres[i];
            } else {
                pmesg[n * lg_msg + i] = alphabet[(n % 26) + 1];
            }
            lg_mesg += 1;
        }

        // Imprimer la lettre individuelle
        printf("Lettre %d: ", n + 1);
        for (int i = 0; i < lg_msg; i++) {
            printf("%c", pmesg[n * lg_msg + i]);
        }
        printf("\n");
    }

    pmesg[nb_message * lg_msg] = '\0';
    return lg_mesg;
}

void emetteur_bal(int n_port,char* nom_domaine, synchro* p_trame){
    //creation socket
	int sock_local;
	if((sock_local=socket(AF_INET , SOCK_STREAM , IPPROTO_TCP))==-1){
		printf("echec de creation du socket\n");
		exit(1);
	}

	//création adresse de socket distant
	memset((char *) &adr_distant, 0 ,sizeof(adr_distant));
	adr_distant.sin_family = AF_INET;
	adr_distant.sin_port = htons(n_port);//port

	if((hp = gethostbyname(nom_domaine))==NULL){
		printf("erreur gethostby name");
		exit(1);
	}
	memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);//ip

	//debug
	printf("emetteur pret\n");

	//connection
	int etat=-1;
	while(etat==-1){
		etat=connect(sock_local,(struct sockaddr*)&adr_distant,sizeof(adr_distant));
	}
	printf("connecté à %s\n",nom_domaine);

    // Debut envoi message synchro
    etat = -1;
	printf("%d\n",p_trame->emetteur_ou_recepteur);
    while (etat ==-1)
    {
        etat = send(sock_local,p_trame,sizeof(synchro),0);
    }

    // Envoi de toutes les lettres
	char * pmesg=malloc(p_trame->nb_lettres*p_trame->taille_lettre*sizeof(char));
    int lg_mesg= construire_lettres(pmesg,p_trame->taille_lettre,p_trame->nb_lettres,p_trame->identifiant );
	etat=-1;
	while(etat==-1){
		etat=send(sock_local,pmesg,lg_mesg,0); //0 ou MSG_OOB urgent
	}
	
	if (close(sock_local)==-1){
		printf("echec fermeture d'emission\n");
	}
	free(pmesg);

}

void recepteur_bal(int n_port, char* nom_domaine, synchro* p_trame){
    //creation socket
	int sock_local;
	if((sock_local=socket(AF_INET , SOCK_STREAM , IPPROTO_TCP))==-1){
		printf("echec de creation du socket\n");
		exit(1);
	}

	//création adresse de socket distant
	memset((char *) &adr_distant, 0 ,sizeof(adr_distant));
	adr_distant.sin_family = AF_INET;
	adr_distant.sin_port = htons(n_port);//port

	if((hp = gethostbyname(nom_domaine))==NULL){
		printf("erreur gethostby name\n");
		exit(1);
	}
	memcpy((char*)&(adr_distant.sin_addr.s_addr),hp->h_addr,hp->h_length);//ip

	//debug
	printf("Récepteur \n");
	//connection
	int etat=-1;
	while(etat==-1){
		etat=connect(sock_local,(struct sockaddr*)&adr_distant,sizeof(adr_distant));
	}
	printf("Connecté à %s\n",nom_domaine);

    // Debut envoi message synchro
    etat = -1;
    while (etat ==-1)
    {
        etat = send(sock_local,p_trame,sizeof(synchro),0);
    }
	/* alocation d'espace de reception pour le message */
	int* ptaille=malloc(sizeof(int));
	
	int nb_octet_lu;
	while ((nb_octet_lu = read(sock_local,ptaille,sizeof(int)))>0){
		//debug printf("%d\n",nb_octet_lu);
		//printf("Taille de la lettre reçu : %d\n",*ptaille);
		
		if (*ptaille==-1){
			printf("Aucun message à lire\n");
			if (close(sock_local)==-1){
				printf("echec fermeture d'emission\n");
			}
		}
		if (*ptaille==0){
			printf("Tous les messages ont été lus\n");
			if (close(sock_local)==-1){
				printf("echec fermeture d'emission\n");
			}
		}
		if (*ptaille>0){
			printf("Taille de la lettre reçu : %d\n",*ptaille);
			char *pmesg = malloc((*ptaille + 1) * sizeof(char));
			int total_lu = 0;
			while (total_lu < *ptaille) {
				nb_octet_lu = read(sock_local, pmesg + total_lu, *ptaille - total_lu);
				if (nb_octet_lu <= 0) {
					break;
				}
				total_lu += nb_octet_lu;
			}
			pmesg[*ptaille] = '\0'; // Pour affichage correct
			printf("%s\n", pmesg);
			free(pmesg);
		}
	}
	free(ptaille);
	exit(0);
}

void boite_bal(int n_port){
	// initialisation de la mémoire de la bal
	boite_recep* p_init_bal = malloc(sizeof(boite_recep));
	p_init_bal->identifiant_recep=-1;
	p_init_bal->premiere_lettre=NULL;
	p_init_bal->ptr_next_boite=NULL;

	//lancement du programme de la bal
	//creation socket
	int sock_local;
	int lg_adr_local=sizeof(adr_local);
	if((sock_local=socket(AF_INET , SOCK_STREAM , IPPROTO_TCP))==-1){
		printf("echec de creation du socket\n");
		exit(1);
	}
	//Construction de l'adresse
	memset((char*) &adr_local, 0 , sizeof(adr_local));
	adr_local.sin_family = AF_INET;
	adr_local.sin_port = htons(n_port);
	adr_local.sin_addr.s_addr=INADDR_ANY;

	//association de l'adresse au socket
	if(bind(sock_local, (struct sockaddr*) &adr_local,lg_adr_local)==-1){
		printf("echec du bind\n");
		exit(1);
	}
	printf("Succes du bind\n");

	/*dimensionnement de la file de demande de connexion ici 5 à la fois*/
	listen(sock_local,5);

	
	/*affectaion de la demande à un socket fils (avec verification)*/
	int sock_local_bis=0;
	/* adresse emetteur*/
	struct sockaddr* padr_em=malloc(sizeof(struct sockaddr));
	unsigned int* plg_adr_em=malloc(sizeof(int));

	int nb_octet_lu=0;
	printf("b.a.l. opérationnelle\n");
	while (1) {
    if ((sock_local_bis = accept(sock_local, (struct sockaddr *)padr_em, plg_adr_em)) == -1) {
        printf("echec du accept\n");
        exit(1);
    }

    // Récupération de la trame
    synchro *p_trame = malloc(sizeof(synchro));
    nb_octet_lu = read(sock_local_bis, p_trame, sizeof(synchro));
    printf("%d\n", p_trame->emetteur_ou_recepteur);

    // Émetteur
    if (p_trame->emetteur_ou_recepteur == 0) {
        printf("Type de client : Emetteur\n");

        // Recherche de la boîte aux lettres du récepteur
        boite_recep *p_boite_cherchee = NULL;
        boite_recep *p_boite_actuelle = p_init_bal;

        while (p_boite_actuelle != NULL) {
            if (p_boite_actuelle->identifiant_recep == p_trame->identifiant) {
                p_boite_cherchee = p_boite_actuelle;
                break; // Arrêter la recherche dès qu'on trouve la boîte
            }
            if (p_boite_actuelle->ptr_next_boite == NULL) {
                break; // Dernière boîte atteinte
            }
            p_boite_actuelle = p_boite_actuelle->ptr_next_boite;
        }

        // Cas de non-existence de la boîte
        if (p_boite_cherchee == NULL) {
            printf("Nouvelle boite créée\n");
            boite_recep *p_boite_cree = malloc(sizeof(boite_recep));
            p_boite_cree->identifiant_recep = p_trame->identifiant;
            p_boite_cree->ptr_next_boite = NULL;
            p_boite_cree->premiere_lettre = NULL;

            if (p_boite_actuelle != NULL) {
                p_boite_actuelle->ptr_next_boite = p_boite_cree;
            } else {
                p_init_bal = p_boite_cree; // Si c'est la première boîte
            }

            p_boite_cherchee = p_boite_cree;
        }

        // Ajout des lettres dans la boîte
        char *pmesg = malloc(p_trame->taille_lettre * sizeof(char));
        while ((nb_octet_lu = read(sock_local_bis, pmesg, p_trame->taille_lettre)) > 0) {
            lettre *p_lettre_arrivee = malloc(sizeof(lettre));
            p_lettre_arrivee->message = malloc(nb_octet_lu + 1); // +1 pour le \0
            strncpy(p_lettre_arrivee->message, pmesg, nb_octet_lu);
            p_lettre_arrivee->message[nb_octet_lu] = '\0'; // Ajout du \0
            p_lettre_arrivee->taille_lettre = nb_octet_lu;
            p_lettre_arrivee->ptr_next = NULL;

            if (p_boite_cherchee->premiere_lettre == NULL) {
                // Si la boîte est vide, ajouter la première lettre
                p_boite_cherchee->premiere_lettre = p_lettre_arrivee;
            } else {
                // Sinon, ajouter à la fin de la liste
                lettre *p_lettre_actuelle = p_boite_cherchee->premiere_lettre;
                while (p_lettre_actuelle->ptr_next != NULL) {
                    p_lettre_actuelle = p_lettre_actuelle->ptr_next;
                }
                p_lettre_actuelle->ptr_next = p_lettre_arrivee;
            }
        }
        printf("messages enregistrés\nFin de communication avec l'emetteur\n");
		printf("Les messages sont :\n");
		boite_recep *boite_affichage = p_init_bal->ptr_next_boite;
		while (boite_affichage != NULL) {
			printf("Boite n° %d \n", boite_affichage->identifiant_recep);
			lettre *lettre_courante = boite_affichage->premiere_lettre;
			while (lettre_courante != NULL) {
				printf("%s\n", lettre_courante->message);
				lettre_courante = lettre_courante->ptr_next;
			}
			boite_affichage = boite_affichage->ptr_next_boite;
		}
        free(pmesg);
    }

    // Récepteur 
    if (p_trame->emetteur_ou_recepteur == 1) {
        printf("Type de client : Récepteur\n");

    // Recherche de la boîte aux lettres du récepteur
    boite_recep* p_boite_cherchee = NULL;
    boite_recep* p_boite_actuelle = p_init_bal;

    while (p_boite_actuelle != NULL) {
        if (p_boite_actuelle->identifiant_recep == p_trame->identifiant) {
            p_boite_cherchee = p_boite_actuelle;
            break; // Arrêter la recherche dès qu'on trouve la boîte
        }
        p_boite_actuelle = p_boite_actuelle->ptr_next_boite;
    }

    int etat;
    int taille;

    // Cas de non-existence de la boîte ou si elle est vide
    if (p_boite_cherchee == NULL || p_boite_cherchee->premiere_lettre == NULL) {
        // Envoyer une taille de -1 pour indiquer qu'il n'y a rien à lire
        taille = -1;
        etat = send(sock_local_bis, &taille, sizeof(int), 0);
        if (etat == -1) {
            printf("Erreur lors de l'envoi de la taille (-1)\n");
        }
    } 
	else {
        // Envoi des lettres présentes dans la boîte
        lettre* p_lettre = p_boite_cherchee->premiere_lettre;

        while (p_lettre != NULL) {
            // Envoyer la taille de la lettre
            taille = p_lettre->taille_lettre;
			printf("Taille de la lettre envoyé : %d\n",taille);
            etat = send(sock_local_bis, &taille, sizeof(int), 0);
            if (etat == -1) {
                printf("Erreur lors de l'envoi de la taille de la lettre\n");
                break;
            }

            // Envoyer le contenu de la lettre
			printf("Contenu envoyé : %s\n",p_lettre->message);
            etat = send(sock_local_bis, p_lettre->message, p_lettre->taille_lettre, 0);
            if (etat == -1) {
                printf("Erreur lors de l'envoi du message\n");
                break;
            }

            // Passer à la lettre suivante
            p_lettre = p_lettre->ptr_next;
        }

        // Envoyer une taille de 0 pour indiquer la fin de la communication
        taille = 0;
        etat = send(sock_local_bis, &taille, sizeof(int), 0);
        if (etat == -1) {
            printf("Erreur lors de l'envoi de la taille (0)\n");
        }
    }

    printf("Fin de communication avec le récepteur\n");
    }

    free(p_trame);
}
	/* liberation d'espace*/
	free(padr_em);
	free(plg_adr_em);
	free(p_init_bal);
}