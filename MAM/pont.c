#include <stdio.h>
#include <stdlib.h>
#include <string.h> //pour utiliser strcmp, strcpy, strstr
#include "pont.h"


//Détecte surcharges (> 80% capacité = jaune, > 90% = rouge), déséquilibre entre piles.
void detecter_anomalies_charge(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr) {
    float charge_nord = -1.0, charge_sud = -1.0;
    int idx_nord = -1;
    // ÉTAPE 1 : Détection des erreurs & surcharges individuelles (Capteur par capteur)
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "CHARGE") == 0) { //permet de comparer les chaines de caractères
            float div = capteurs[i].valeur_mesuree / capteurs[i].valeur_nominale;//représente le ratio
            if (div >= 0.80) {
                capteurs[i].etat = 2;
                strcpy(capteurs[i].remarque, "Alerte JAUNE: Utilisation de plus de 80% de la capacite");
                Alerte alerts = {"27/07/2026 10:00", i, "SURCHARGE", "JAUNE", capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.80, "Diminuer les charges lourdes"};
                alertes[(*nb_alr)++]=alerts;//on initialise un tableau d'alertes
            }
            else if (div >= 0.90) {
                capteurs[i].etat = 3;
                strcpy(capteurs[i].remarque, "Alerte ROUGE: Utilisation de plus de 90% de la capacite)"); //permet de copier le texte dans la remarque
                Alerte alerts = {"27/07/2026 10:00", i, "SURCHARGE", "ROUGE", capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.90, "Fermer la circulation aux Poids-Lourds"};
                alertes[(*nb_alr)++]=alerts;
            }
        }
       // ÉTAPE 2 : Détection du déséquilibre entre les piles (Analyse globale)
            // Récupération des charges aux deux extrémités (Pile Nord vs Pile Sud)
            if (strstr(capteurs[i].nom, "Pile Nord") != NULL) {
                charge_nord = capteurs[i].valeur_mesuree;
                idx_nord = i;
            }
            if (strstr(capteurs[i].nom, "Pile Sud") != NULL) {
                charge_sud = capteurs[i].valeur_mesuree;
            }
        }
    // Calcul du déséquilibre entre les deux piles
    if (charge_nord != -1.0 && charge_sud != -1.0) {
        float ecart = charge_nord - charge_sud;
        if (ecart < 0) ecart = -ecart; // Valeur absolue
        if (ecart > 100.0) {
            Alerte al_desequilibre = {"27/07/2026 10:00", idx_nord, "SURCHARGE", "JAUNE", ecart, 100.0, "Desequilibre important de charge entre Pile Nord et Pile Sud"};
            alertes[(*nb_alr)++] = al_desequilibre;
        }
    }
}



// Calcul du score de déformation pondéré
float calculer_score_deformation(Capteur capteurs[], int n) {
    float max = 0;
    float somme_pile = 0;
    float somme_travee = 0;

    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "DEFORM") == 0) {
            float ecart = 0;
            if (capteurs[i].valeur_nominale == 150) {
                ecart = capteurs[i].valeur_mesuree;
                somme_pile += ecart;
            }
            if (capteurs[i].valeur_nominale == 80) {
                ecart = capteurs[i].valeur_mesuree;
                somme_travee += ecart;
            }
            max += 200;
        }
    }

    float somme_ponderee = (somme_pile * 0.7 + somme_travee * 0.3);
    if (max == 0) return 100;
    return 100 - ((somme_ponderee / max) * 100);
}

// Calcul du score de vibration sur les fréquences propres
float calculer_score_vibration(Capteur capteurs[], int n) {
    float somme = 0;
    float max = 0;

    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "VIBR") == 0) {
            float ecart = 0;

            if (capteurs[i].valeur_mesuree > 0.50) {
                ecart = capteurs[i].valeur_mesuree - 0.50;
            }
            else if (capteurs[i].valeur_mesuree < 0.35) {
                ecart = 0.35 - capteurs[i].valeur_mesuree;
            }

            somme += ecart;
            max += 0.25;
        }
    }

    if (max == 0) return 100;
    return 100 - ((somme / max) * 100);
}

// Calcul du score de charge avec le pourcentage d'utilisation
float calculer_score_charge(Capteur capteurs[], int n) {
    float somme = 0;
    int nb_charge = 0;

    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "CHARGE") == 0) {
            float utilisation = (capteurs[i].valeur_mesuree / capteurs[i].valeur_nominale) * 100;
            somme += utilisation;
            nb_charge++;
        }
    }

    if (nb_charge == 0) return 100;

    float max = nb_charge * 100;
    return 100 - ((somme / max) * 100);
}

void calculer_indice_sante(Capteur capteurs[], int n, Alerte alertes[], int nb_alr,
                          IndiceHealthStructural *sante) {
    sante->score_deformation = calculer_score_deformation(capteurs, n);
    sante->score_vibration = calculer_score_vibration(capteurs, n);
    sante->score_charge = calculer_score_charge(capteurs, n);

    sante->indice_global = (sante->score_deformation * 0.4 +
                            sante->score_vibration * 0.35 +
                            sante->score_charge * 0.25);

    printf("\n========== RESUME DE SANTE STRUCTURALE ==========\n");
    printf("Indice global (SHI) \t: %.2f / 100\n", sante->indice_global);
    printf("  Score deformation \t: %.2f / 100\n", sante->score_deformation);
    printf("  Score vibration \t: %.2f / 100\n", sante->score_vibration);
    printf("  Score charge \t\t: %.2f / 100\n", sante->score_charge);
    printf("---------------------------------------------------\n");

    if (sante->indice_global >= 90) {
        printf("Etat general \t\t: BONNE SANTE\n");
        printf("Recommandation \t\t: Inspections annuelles\n");
    }
    else if (sante->indice_global >= 80) {
        printf("Etat general \t\t: ALERTE JAUNE\n");
        printf("Recommandation \t\t: Inspections trimestrielles\n");
    }
    else if (sante->indice_global >= 70) {
        printf("Etat general \t\t: ALERTE ORANGE\n");
        printf("Recommandation \t\t: Inspections mensuelles\n");
    }
    else {
        printf("Etat general \t\t: ALERTE ROUGE\n");
        printf("Recommandation \t\t: Inspection d'urgence\n");
    }
    printf("==================================================\n");
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

//Tri à bulles groupant capteurs par type (DEFORM, VIBR, CHARGE).
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

//Affiche le menu principal
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


