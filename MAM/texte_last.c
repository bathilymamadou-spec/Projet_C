#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pont.h"
#define NB_CAPTEURS 24
#define NB_ALERTES_MAX 50

void rapport_inspection(Capteur capteurs[], int n, Alerte alertes[], int nb_alr) {
    IndiceHealthStructural sante;

    // 1. Remplir RapportInspection
    RapportInspection rapport;

    strcpy(rapport.date, "01-08-2024");
    rapport.nb_capteurs = n;
    rapport.nb_alertes = nb_alr;

    // Copier les capteurs lrs alertes
    for (int i = 0; i < n; i++) {
        rapport.capteurs[i] = capteurs[i];
        rapport.alertes[i] = alertes[i];
    }


    // 2. Nom du fichier
    char nom_fichier[50];
    sprintf(nom_fichier, "rapport_inspection_%s.txt", rapport.date);

    // Ouvrir le fichier
    FILE *f = fopen(nom_fichier, "w");
    if (f == NULL) {
        perror("fopen");
        return;
    }

    // Présentation du rapport
    fprintf(f, "======================================================\n");
    fprintf(f, "RAPPORT D'INSPECTION\n");
    fprintf(f, "Pont Faidherbe de Saint-Louis\n");
    fprintf(f, "Date : %s\n", rapport.date);
    fprintf(f, "Norme : EN 1999 (Eurocode 9) - Conception des structures en aluminium\n");
    fprintf(f, "======================================================\n\n");


    //  Résumé de santé structurale
    calculer_indice_sante(capteurs, n, alertes, nb_alr, &sante, f);

    // Alertes actives
     int nbr_alr =0; //nombre d'alerte totale
 detecter_anomalies_charge(capteurs, n, alertes, &nbr_alr);
    int compt_jaune =0; //compteur des alertes jaunes
    int compt_rouge = 0; //compteur des alertes rouges

    fprintf(f, "\nALERTES ACTIVES\n");
    //on parcoure les capteurs et on compte les alertes selon le type
    for (int i = 0; i < n; i++){
        if (capteurs[i].etat == JAUNE)
            compt_jaune +=1;
        else if (capteurs[i].etat == ROUGE)
            compt_rouge +=1;
    };

    //affichage du nombre d'alerte
    fprintf(f, "Niveau JAUNE (%d alerte(s))\n", compt_jaune);
    fprintf(f, "Niveau ROUGE (%d alerte(s))\n", compt_rouge);



    // Conformité Eurocode
    int i=0;
    int validation_deform =1;
const char* conformite_cap(int valide){
    if (valide ==1)
        return "OK";
}

     while (validation_deform == 1 && i < NB_CAPTEURS){
        if (strcmp(capteurs[i].type, "DEFORM")==0){
             validation_deform = valider_capteur(&capteurs[i]);
    }
        i++;
}

     while (validation_deform == 1 && i < NB_CAPTEURS){
        if (strcmp(capteurs[i].type, "VIBR")==0){
             int validation_vibr = valider_capteur(&capteurs[i]);
    }
        i++;
}

     while (validation_deform == 1 && i < NB_CAPTEURS){
        if (strcmp(capteurs[i].type, "CHARGE")==0){
             int validation_charge = valider_capteur(&capteurs[i]);
    }
        i++;
}

const char* conform_deform = conformite_cap(validation_deform);
const char* conform_vibr = conformite_cap(validation_deform);
const char* conform_charge = conformite_cap(validation_deform);

    fprintf(f, "\nCONFORMITE EUROCODE 9\n");
    fprintf(f, "Déformation vs limites \t: %s\n",conform_deform);
    fprintf(f, "Vibration vs résonance \t: %s\n", conform_vibr);
    fprintf(f, "Charge vs capacité \t: %s\n", conform_charge);
    fprintf(f, "\n");

    // Pied de page
    fprintf(f, "FIN DU RAPPORT\n");
    fprintf(f, "======================================================\n");

    fclose(f);

    printf("Rapport genere : %s\n", nom_fichier);
}
