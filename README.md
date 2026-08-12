# README - Projet Suivi Structural Pont Faidherbe

## Description
Systeme de suivi structural en C pour le Pont Faidherbe de Saint-Louis.
Surveillance de 24 capteurs (deformation, vibration, charge) avec calcul d'indice de sante SHI et generation de rapports reglementaires (Eurocode 9).

## Structure du projet

Projet_C/  
├── MAM/  
│   ├── main.c          
│   ├── pont.c         
│   ├── pont.h         
├── mesures_capteurs.txt 
└── README.md

## Compilation

```bash
gcc MAM/main.c MAM/pont.c MAM/fichier.c MAM/texte_last.c -o MAM -lm
