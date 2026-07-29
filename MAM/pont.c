#include <stdio.h>
#include <stdlib.h>
#include <pont.h>


void detecter_anomalies_charge(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr) {
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "CHARGE") == 0) {
            float div = capteurs[i].valeur_mesuree / capteurs[i].valeur_nominale;
            if (div >= 0.80) {
                capteurs[i].etat = 2;
                strcpy(capteurs[i].remarque, "Alerte JAUNE: Utilisation de plus de 80% de la capacité");
                alertes[(*nb_alr)++] = {"27/07/2026 10:00", i, "SURCHARGE", "JAUNE", , capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.80, "Diminuer les charges lourdes"};
            }
            else if (ratio >= 0.90) {
                capteurs[i].etat = 3;
                strcpy(capteurs[i].remarque, "Alerte ROUGE: Utilisation de plus de 90% de la capacité)");
                Alertes alertes[(*nb_alr)++] = {"27/07/2026 10:00", i, "SURCHARGE", "ROUGE", capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.90, "Fermeture circulation Poids-Lourds"};
            }
        }
    }
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
    printf("\n=== SUIVI STRUCTURALE PONT FAIDHERBE ===\n");
    printf("1. Charger données de mesure\n");
    printf("2. Valider tous les capteurs\n");
    printf("3. Afficher état détaillé des capteurs\n");
    printf("4. Analyser anomalies\n");
    printf("5. Calculer indice de santé structurale\n");
    printf("6. Générer rapport d'inspection\n");
    printf("7. Exporter rapport réglementaire (Eurocode)\n");
    printf("8. Afficher alertes actives\n");
    printf("9. Quitter\n");
    printf("Entrer votre choix\n")
    scanf("%d", &choix)
}


