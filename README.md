# Gestion de Tours Cellulaires 📡

> **Simulation de réseau téléphonique et gestion d'antennes en langage C.**

Ce projet a été réalisé dans le cadre du cursus à l'**École Supérieure des Communications de Tunis (Sup'Com)**. Il simule le fonctionnement d'un opérateur mobile en gérant les antennes relais, les utilisateurs, et les appels via une simulation de l'accès radio.

---

## 👥 Auteur
**Réalisé par :**
* Ala Jouili 

---

## 🎯 Objectifs du Projet
L'application vise à reproduire plusieurs aspects réels d'un opérateur télécom :
* **Gestion des Antennes** : Administration du parc d'antennes relais.
* **Gestion des Utilisateurs** : Suivi des abonnés mobiles.
* **Simulation FTDMA** : Simulation de l'accès radio (Fréquences et Time Slots).
* **Gestion des Appels** : Simulation d'appels avec facturation et rechargement de crédit.
* **Statistiques** : Suivi de la charge du réseau.
* **Persistance** : Sauvegarde et chargement des données.

---

## ⚙️ Fonctionnalités Détaillées

### 1. Module Antennes 📡
* Ajout et suppression d'antennes.
* Recherche d'antennes par ID ou par région.
* Affichage de la liste des antennes disponibles.

### 2. Module Utilisateurs 👥
* Gestion dynamique des abonnés (Ajout/Suppression).
* **Recherche globale** d'utilisateurs dans tout le réseau.
* **Handover** : Transfert d'un utilisateur d'une antenne à une autre.
* Tri alphabétique des listes d'utilisateurs.

### 3. Simulation Réseau & Système 📞
* **Accès FTDMA** : Gestion des fréquences et créneaux temporels.
* **Facturation** : Débit du crédit lors des appels.
* **Statistiques** : Calcul de la charge par antenne et identification des zones saturées.
* **Sauvegarde** : Exportation et importation des données via fichiers.

---

## 🛠️ Architecture Technique

Le projet repose sur une architecture modulaire et des structures de données mixtes pour optimiser la mémoire.

### Structures de Données
* **Antennes (Tableau Statique)** :
  Chaque antenne est définie par un ID, une région, une fréquence et un pointeur vers sa liste d'abonnés.
  ```c
  typedef struct Antenne {
      int antenneID;
      char region[MAX_REGION];
      int frequence;
      User* userList; // Pointeur vers la liste chaînée
  } Antenne;
