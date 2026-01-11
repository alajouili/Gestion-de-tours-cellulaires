# Gestion de Tours Cellulaires 📡

> Simulation de réseau téléphonique et gestion d'antennes en langage C.

## 📝 Description
Ce projet a été réalisé dans le cadre du cursus à l'**École Supérieure des Communications de Tunis (Sup'Com)**. Il simule le fonctionnement d'un opérateur mobile en gérant les antennes relais, les utilisateurs, et les appels via une simulation de l'accès radio (FTDMA).

## 🚀 Fonctionnalités
L'application permet de gérer un réseau complet :

* **Gestion des Antennes** : Ajout, suppression et recherche d'antennes (ID, Région, Fréquence).
* **Gestion des Utilisateurs** : Listes chaînées dynamiques, ajout/suppression, et recherche globale.
* **Simulation Réseau** :
    * Simulation d'accès **FTDMA**.
    * Gestion des appels et facturation.
    * **Handover** : Transfert d'utilisateurs entre antennes.
* **Persistance** : Sauvegarde et chargement de l'état du réseau via fichiers.
* **Statistiques** : Analyse de la charge du réseau.

## 🛠️ Aspects Techniques
Ce projet met en pratique des concepts avancés du langage C :
* **Structures de données** : Utilisation mixte de tableaux (Antennes) et de listes chaînées (Utilisateurs).
* **Gestion de la mémoire** : Allocation dynamique (`malloc`, `free`) et prévention des fuites mémoire.
* **Algorithmique** : Tri, recherche et manipulation de pointeurs.

## 🏗️ Structure du Projet
Le code est organisé de manière modulaire :
* `antenne.c/h` : Gestion des infrastructures.
* `user.c/h` : Gestion des abonnés.
* `ftdma.c/h` : Simulation des créneaux temporels et fréquences.
* `fichiers.c/h` : Gestion de la sauvegarde.
* `main.c` : Point d'entrée et menu principal.



**Encadré par :** Mme. Nesrine Zaghdoud  
**Année :** 2025-2026