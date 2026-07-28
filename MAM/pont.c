#include <stdio.h>
#include <stdlib.h>
#include <pont.h>




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


