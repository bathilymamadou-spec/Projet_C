
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pont.h"




void detecter_anomalies_charge(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr) {
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "CHARGE") == 0) {
            float div = capteurs[i].valeur_mesuree / capteurs[i].valeur_nominale;
            if (div >= 0.80) {
                capteurs[i].etat = 2;
                strcpy(capteurs[i].remarque, "Alerte JAUNE: Utilisation de plus de 80% de la capacit�");
                Alerte alerts = {"27/07/2026 10:00", i, "SURCHARGE", "JAUNE", capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.80, "Diminuer les charges lourdes"};
                alertes[(*nb_alr)++]=alerts;
            }
            else if (div >= 0.90) {
                capteurs[i].etat = 3;
                strcpy(capteurs[i].remarque, "Alerte ROUGE: Utilisation de plus de 90% de la capacit�)");
                Alerte alerts = {"27/07/2026 10:00", i, "SURCHARGE", "ROUGE", capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.90, "Fermeture circulation Poids-Lourds"};
                alertes[(*nb_alr)++]=alerts;
            }
        }
    }
}



// Calcul du score de déformation pondéré
float calculer_score_deformation(Capteur capteurs[], int n) {
    float max = 0; // l'ecart maximal
    float somme_pile = 0; //l'ecart des piles
    float somme_travee = 0; //l'ecart des travees

    //on parcoure le tableau et vérifie si le capteur est de type déformation
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "DEFORM") == 0) {
            if (capteurs[i].valeur_nominale == 150) {
                somme_pile += capteurs[i].valeur_mesuree;
            }
            if (capteurs[i].valeur_nominale == 80) {
                somme_travee += capteurs[i].valeur_mesuree;
            }
            max += 200;
        }
    }

    float somme_ponderee = (somme_pile * 0.7 + somme_travee * 0.4);
    if (max == 0)
        return 100;
    return 100 - ((somme_ponderee / max) * 100);
}

// Calcul du score de vibration sur les fréquences propres
float calculer_score_vibration(Capteur capteurs[], int n) {
    float somme = 0;
    float max = 0;

    //on parcoure le tableau et vérifie si le capteur est de type vibration
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "VIBR") == 0) {
            float ecart = 0;
            float ideal = 0.425; //on prend le milieu de la plage donnée
            ecart = fabs(capteurs[i].valeur_mesuree - ideal);
            somme += ecart;
            max += 0.1;
        }
    }

    if (max == 0)
        return 100;
    return 100 - ((somme / max) * 100);
}

// Calcul du score de charge avec le pourcentage d'utilisation
float calculer_score_charge(Capteur capteurs[], int n) {
    float somme = 0;
    int nb_charge = 0; //le nombre de capteur de charge
    float utilisation; //le pourcentage de la charge utilisé

    //on parcoure le tableau et vérifie si le capteur est de type charge
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "CHARGE") == 0) {
                utilisation = (capteurs[i].valeur_mesuree / capteurs[i].valeur_nominale) * 100;
            somme += utilisation;
            nb_charge++;
        }
    }

    if (nb_charge == 0)
        return 100;
    return somme / nb_charge;
}

//calcul de l'indice global et affichage des valeurs et des recommandations
void calculer_indice_sante(Capteur capteurs[], int n, Alerte alertes[], int nb_alr,
                          IndiceHealthStructural *sante, FILE *f) {
    sante->score_deformation = calculer_score_deformation(capteurs, n);
    sante->score_vibration = calculer_score_vibration(capteurs, n);
    sante->score_charge = calculer_score_charge(capteurs, n);

    sante->indice_global = (sante->score_deformation * 0.4 +
                            sante->score_vibration * 0.35 +
                            sante->score_charge * 0.25);
    fputs("\n========== RESUME DE SANTE STRUCTURALE ==========\n", f);
    fprintf(f, "Indice global (SHI) \t: %.2f / 100\n", sante->indice_global);
    fprintf(f, "  Score deformation \t: %.2f / 100\n", sante->score_deformation);
    fprintf(f,"  Score vibration \t: %.2f / 100\n", sante->score_vibration);
    fprintf(f,"  Score charge \t\t: %.2f / 100\n", sante->score_charge);
    fputs("---------------------------------------------------\n", f);

    if (sante->indice_global >= 90) {
        fputs("Etat general \t\t: BONNE SANTE\n",f);
        fputs("Recommandation \t\t: Inspections annuelles\n", f);
    }
    else if (sante->indice_global >= 80) {
        fputs("Etat general \t\t: ALERTE JAUNE\n", f);
        fputs("Recommandation \t\t: Inspections trimestrielles\n",f);
    }
    else if (sante->indice_global >= 70) {
        fputs("Etat general \t\t: ALERTE ORANGE\n", f);
        fputs("Recommandation \t\t: Inspections mensuelles\n", f);
    }
    else {
            fputs("Etat general \t\t: ALERTE ROUGE\n", f);
        fputs("Recommandation \t\t: Inspection d'urgence\n",f);
    }
    fputs("==================================================\n", f);
}

void trier_capteurs_par_type(Capteur capteurs[], int n){
    for (int i = 0; i < n-1; i++){
        for (int j = i+1; j < n; j++){
            if (strcmp(capteurs[i].type, capteurs[j].type) > 0){
                Capteur copie = capteurs[i];
                capteurs[i] = capteurs[j];
                capteurs[j] = copie;
            }
        }
    }
}

Capteur* recherche_capteur_critique(Capteur capteurs[], int n){
    Capteur *pointeur = NULL;
    int max=1;
    for (int i = 0; i < n; i++){
        if (capteurs[i].etat > max){
            max = capteurs[i].etat;
            pointeur = &capteurs[i];
        }
    }
    return pointeur;
}

void afficher_menu(){
    int choix = 0;
    printf("\n=== SUIVI STRUCTURALE PONT FAIDHERBE ===\n");
    printf("1. Charger donnees de mesure\n");
    printf("2. Valider tous les capteurs\n");
    printf("3. Afficher etat detaille des capteurs\n");
    printf("4. Analyser anomalies\n");
    printf("5. Calculer indice de sante structurale\n");
    printf("6. Generer rapport d'inspection\n");
    printf("7. Exporter rapport reglementaire (Eurocode)\n");
    printf("8. Afficher alertes actives\n");
    printf("9. Quitter\n");
    printf("Entrer votre choix\n");
    scanf("%d", &choix);
}




