#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pont.h"
#define NB_CAPTEURS 24
#define NB_ALERTES_MAX 50

// Fonction pour la conformité (en dehors de rapport_inspection)
const char* conformite_cap(int valide) {
    if (valide == 1)
        return "OK";
    else
        return "Â vérifier";
}

void rapport_inspection(Capteur capteurs[], int n, Alerte alertes[], int nb_alr) {
    IndiceHealthStructural sante;

    // Remplir RapportInspection
    RapportInspection rapport;

    strcpy(rapport.date, "01-08-2024");
    rapport.nb_capteurs = n;
    rapport.nb_alertes = nb_alr;

    // Copier les capteurs
    for (int i = 0; i < n; i++) {
        rapport.capteurs[i] = capteurs[i];
    }

    // Copier les alertes (nb_alr peut être différent de n)
    for (int i = 0; i < nb_alr; i++) {
        rapport.alertes[i] = alertes[i];
    }

    // Nom du fichier
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
    fprintf(f, "======================================================\n");

    // Résumé de santé structurale
    calculer_indice_sante(capteurs, n, alertes, nb_alr, &sante, f);

    // Alertes actives
    int compt_jaune = 0;
    int compt_rouge = 0;

    for (int i = 0; i < n; i++) {
        if (capteurs[i].etat == JAUNE)
            compt_jaune++;
        else if (capteurs[i].etat == ROUGE)
            compt_rouge++;
    }

//Affichage des alertes
    fprintf(f, "\nALERTES ACTIVES\n");
    fprintf(f, "Niveau JAUNE (%d alerte(s))\n", compt_jaune);
    // Afficher les alertes jaunes
    if (compt_jaune == 0)
        fprintf(f, "  AUCUNE ALERTE");
    else{
        for (int i = 0; i < n; i++) {
            if (capteurs[i].etat == JAUNE) {
                fprintf(f, "- %s %s : %s\n", capteurs[i].id, capteurs[i].nom, capteurs[i].remarque);
        }
    }
}


    fprintf(f, "\nNiveau ROUGE (%d alerte(s))\n", compt_rouge);
    // Afficher les alertes rouges
    if (compt_rouge == 0)
        fprintf(f, "  AUCUNE ALERTE\n");
    else {
        for (int i = 0; i < n; i++) {
            if (capteurs[i].etat == ROUGE) {
                fprintf(f, "- %s %s : %s\n", capteurs[i].id, capteurs[i].nom, capteurs[i].remarque);
            }
        }
    }

    // Conformité Eurocode
    int validation_deform = 1;
    int validation_vibr = 1;
    int validation_charge = 1;

    // Vérification déformation
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "DEFORM") == 0) {
            if (valider_capteur(&capteurs[i]) == 0) {
                validation_deform = 0;
                break;
            }
        }
    }

    // Vérification vibration
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "VIBR") == 0) {
            if (valider_capteur(&capteurs[i]) == 0) {
                validation_vibr = 0;
                break;
            }
        }
    }

    // Vérification charge
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "CHARGE") == 0) {
            if (valider_capteur(&capteurs[i]) == 0) {
                validation_charge = 0;
                break;
            }
        }
    }

    const char* conform_deform = conformite_cap(validation_deform);
    const char* conform_vibr = conformite_cap(validation_vibr);
    const char* conform_charge = conformite_cap(validation_charge);

    fprintf(f, "\nCONFORMITE EUROCODE 9\n");
    fprintf(f, "   Deformation vs limites           : %s\n", conform_deform);
    fprintf(f, "   Vibration vs resonance           : %s\n", conform_vibr);
    fprintf(f, "   Charge vs capacite               : %s\n", conform_charge);
    fprintf(f, "\n");

    // Actions prioritaires
    fprintf(f, "ACTIONS PRIORITAIRES\n");
    int action_number = 1;
    for (int i = 0; i < nb_alr; i++) {
        if (strlen(alertes[i].action) > 0) {
            fprintf(f, "%d. %s\n", action_number++, alertes[i].action);
        }
    }
    if (action_number == 1) {
        fprintf(f, "AUCUNE ACTION PRIORITAIRE\n");
    }

    // Pied de page
    fprintf(f, "\n======================================================\n");
    fprintf(f, "FIN DU RAPPORT\n");
    fprintf(f, "======================================================\n");

    fclose(f);

    printf(" Rapport genere : %s\n", nom_fichier);


}

void alertes_jour(Capteur capteurs[], int n, Alerte alertes[], int nb_alertes) {
FILE *f_alert = fopen("alertes_01-08-2024", "w");
    if (f_alert == NULL) {
        perror("fopen");
        return;
    }
    for (int i = 0; i < n; i++) {
            fprintf(f_alert, "- %s %s : %s\n", capteurs[i].id, capteurs[i].nom, capteurs[i].remarque);
        }
    printf("Fichier alerte généré");
}
