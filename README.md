# Pamela

## Description

Un module pam développé dans le cadre du projet Pamela d'Epitech.

## Fonctionnalitées

* Ajout à la session de chaque utilisateur d'un conteneur chiffré monté pour l'utilisateur
* Besoins personalisables par le biais d'un fichier de conf placé dans le home du user
* Authentification double facteur USB possible

## Installation

Valable uniquement sur les systèmes disposant d'apt

* Executez la commande `make install`
* Modifiez votre fichier /etc/pam.d/session afin d'intégrer les mode `auth`et `session` pour le module mypam.so

