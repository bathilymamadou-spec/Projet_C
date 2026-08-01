
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pont.h"
#define NB_CAPTEURS 24
#define NB_ALERTES_MAX 50

#include <time.h>

void rapport_inspection(Capteur capteurs[], int n, Alerte alertes[], int nb_alr) {
    IndiceHealthStructural sante;
    // Générer le nom du fichier avec la date

  RapportInspection rapport ={
      "01/08/2024",
       NB_CAPTEURS,
       capteurs,
       2,
       alertes,
       0
  }

sprintf("rapport_inspection_%s", date)
    FILE *f = fopen(nom_fichier, "w");
    if (f == NULL) {
        perror("fopen");
        return;
    }

    // Présentation du rapport
    fprintf(f, "======================================================\n");
    fprintf(f, "RAPPORT D'INSPECTION\n");
    fprintf(f, "Pont Faidherbe de Saint-Louis\n");
    fprintf(f, "Date : %s",date);
    fprintf(f, "Norme : EN 1999 (Eurocode 9) - Conception des structures en aluminium\n");
    fprintf(f, "================================================================\n\n");

    // Résumé de santé structurale

    calculer_indice_sante(capteurs, NB_CAPTEURS,sizeof(alertes)/sizeof(Alerte), &sante, f);

    //ALERTES ACTIVES
    fprintf(f, "ALERTES ACTIVES\n");
    fprintf(f, "Niveau JAUNE (%d alerte(s))", a);
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
