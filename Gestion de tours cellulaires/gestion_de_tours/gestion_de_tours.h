#ifndef GESTION_TOURS_H
#define GESTION_TOURS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct User {
    int userID;
    char name[30];
    char phone[15];
    struct User* next;
} User;

typedef struct Antenne {
    int antenneID;
    char region[30];
    int frequence;
    User* userList;
} Antenne;

void ajouterAntenne(Antenne *Antennes, int index);

void supprimerAntenne(Antenne *Antennes, int *nt, int id);

void afficher_Antennes(Antenne *Antennes, int nt);

Antenne* rechercherAntenneParID(Antenne *Antennes, int nt, int id);

Antenne* rechercherAntenneParRegion(Antenne *Antennes, int nt, char *region);


void ajouterUser(Antenne *a, int userID, char *nom, char *phone);

void supprimerUser(Antenne *a, int userID);

void afficherUsers(const Antenne *a);

User* rechercheGlobaleUser(Antenne *Antennes, int nt, int userID);


void libererUsers(User *u);

#endif
