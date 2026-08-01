
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pont.h"
#define NB_CAPTEURS 24
#define NB_ALERTES_MAX 50

#include <time.h>

void rapport_inspection(Capteur capteurs[], int n, Alerte alertes[], int nb_alr) {
    // Générer le nom du fichier avec la date
    long t = time(NULL); //stockage de la date actuelle en seconde
    struct tm temp; //la structure tm a le format A/M/J/H/Mi/S
    localtime_r(&t, &temp); /* on convertit la date en seconde dans un format lisible
                                et on le stocke dans notre structure */

    char nom_fichier[50];

    //création du nom du fichier avec le jour et la date de sa création
    sprintf(nom_fichier, "rapport_inspection_%02d-%02d-%04d.txt",
            temp.tm_mday, temp.tm_mon + 1, temp.tm_year + 1900);

    FILE *f = fopen(nom_fichier, "w");
    if (f == NULL) {
        perror("fopen");
        return;
    }

    // En-tête
    fprintf(f, "======================================================\n");
    fprintf(f, "RAPPORT D'INSPECTION\n");
    fprintf(f, "Pont Faidherbe de Saint-Louis\n");
    fprintf(f, "Date : %02d/%02d/%04d à %02d:%02d\n",
            temp.tm_mday, temp.tm_mon + 1, temp.tm_year + 1900, temp.tm_hour, temp.tm_min);
    fprintf(f, "Norme : EN 1999 (Eurocode 9) - Conception des structures en aluminium\n");
    fprintf(f, "================================================================\n\n");

    // Résumé de santé structurale
    IndiceHealthStructural sante;
    calculer_indice_sante(capteurs, NB_CAPTEURS,alertes,  2, &sante, f);

    // Détail des capteurs
    fprintf(f, " DETAIL DES CAPTEURS\n");
    fprintf(f, "--------------------------------\n");
    fprintf(f, "%-6s %-20s %-12s %-10s %-10s %-10s\n",
            "ID", "Nom", "Type", "Valeur", "Nominal", "Etat");
    fprintf(f, "----------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        char *etat_str = "";
        switch(capteurs[i].etat) {
            case OK: etat_str = "OK"; break;
            case JAUNE: etat_str = "JAUNE"; break;
            case ROUGE: etat_str = "ROUGE"; break;
        }

        fprintf(f, "%-6s %-20s %-12s %-10.2f %-10.2f %-10s\n",
                capteurs[i].id,
                capteurs[i].nom,
                capteurs[i].type,
                capteurs[i].valeur_mesuree,
                capteurs[i].valeur_nominale,
                etat_str);
    }
    fprintf(f, "\n");

    // Conformité Eurocode
    fprintf(f, "CONFORMITE EUROCODE 9\n");
    fprintf(f, "--------------------------------\n");
    int conforme = (sante.indice_global >= 70) ? 1 : 0;
    fprintf(f, "Conformite : %s\n", conforme ? "OUI" : "NON");
    fprintf(f, "\n");

    // Pied de page
    fprintf(f, "FIN DU RAPPORT\n");
    fprintf(f, "================================================================\n");

    fclose(f);

    printf(" Rapport genere : %s\n", nom_fichier);
}
