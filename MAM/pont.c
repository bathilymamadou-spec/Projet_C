#include <stdio.h>
#include <stdlib.h>
#include <string.h> //pour utiliser strcmp, strcpy, strstr
#include <math.h> // pour fabs
#include "pont.h"

int valider_capteur(Capteur *cap){
    // --- CASE 1 : CAPTEURS DE DÉFORMATION ---
    if (strcmp(cap->type, "DEFORM")==0){
        // 1. Vérification de la cohérence physique selon le type de capteur
            // Une déformation aberrante dépasserait largement les plages physiques admissibles
        if (cap->valeur_mesuree<-700 || cap->valeur_mesuree>700){
            strcpy(cap->remarque, "ERREUR: Signal hors plage physique");
            return 0;
        }
        // 2. Évaluation de l'état (OK, JAUNE, ROUGE) et rédaction de la remarque
        else{
            float saut=fabsf(cap->valeur_mesuree-cap->valeur_precedente);
            if (fabsf(cap->valeur_mesuree)>200 || saut>25.0f){
                cap->etat=3; // ROUGE
                strcpy(cap->remarque, "ALERTE ROUGE: Seuil critique");
            }
            else if (saut>10.0f){
                cap->etat=2; // JAUNE
                strcpy(cap->remarque, "ALERTE JAUNE: Augmentation du saut");
            }
            else{
                cap->etat=1; // OK
                strcpy(cap->remarque, "OK: Deformation normale");
            }
            return 1; // Mesure valide
        }

    }

    // --- CASE 2 : CAPTEURS DE VIBRATION ---
    if (strcmp(cap->type, "VIBR")==0){
    // 1. Vérification de la cohérence physique selon le type de capteur
        // Une fréquence de vibration ne peut pas être négative ou exagérément élevée
        if (cap->valeur_mesuree<0 || cap->valeur_mesuree>2){
            strcpy(cap->remarque, "ERREUR: Signal de vibration incohérent");
            return 0;
        }
    // 2. Évaluation de l'état (OK, JAUNE, ROUGE) et rédaction de la remarque
        else{
            if (cap->valeur_mesuree>0.60f || cap->valeur_mesuree<0.30f) {
                cap->etat = 3; // ROUGE
                strcpy(cap->remarque, "ALERTE ROUGE: Fréquence hors plage");
            }
            else if (cap->valeur_mesuree>=0.51f && cap->valeur_mesuree<=0.60f) {
                cap->etat=2;// JAUNE
                strcpy(cap->remarque, "ALERTE JAUNE: Augmentation de la fréquence");
            }
            else {
                cap->etat=1; // OK
                strcpy(cap->remarque, "OK: Fréquence normale");
            }
            return 1; // Mesure valide
        }
    }

    // --- CASE 3 : CAPTEURS DE CHARGE ---
    if (strcmp(cap->type, "CHARGE")==0){
    // 1. Vérification de la cohérence physique selon le type de capteur
        // Une charge ne peut pas être négative ou dépasser largement la capacité critique
        if (cap->valeur_mesuree<0 || cap->valeur_mesuree>5000){
            strcpy(cap->remarque, "ERREUR: Valeur de charge absurde");
            return 0;
        }
    // 2. Évaluation de l'état (OK, JAUNE, ROUGE) et rédaction de la remarque
        else{
            if (cap->valeur_mesuree>cap->seuil_alerte_r) {
                cap->etat = 3; // ROUGE
                strcpy(cap->remarque, "ALERTE ROUGE: Surcharge critique!!");
            }
            else if (cap->valeur_mesuree>cap->seuil_alerte_j) {
                cap->etat = 2; // JAUNE
                strcpy(cap->remarque, "ALERTE JAUNE: Surcharge modèréé");
            }
            else {
                cap->etat = 1; // OK
                strcpy(cap->remarque, "OK: Charge normale");
            }
            return 1; // Mesure valide
        }
    }
}

void detecter_anomalies_deformation(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr){
    const char *DATE_COURANTE="30-07-2026 18:00"; // Exemple d'horodatage courant
    for(int i=0; i<n; i++){
            // On ne traite que les capteurs de déformation
        if(strcmp(capteurs[i].type, "DEFORM")!=0){
            continue;
        }
        float mesure = capteurs[i].valeur_mesuree;
        float precedente = capteurs[i].valeur_precedente;
        float saut = fabsf(mesure - precedente);
        // --- 1. CAS CRITIQUE : Déformation unilatérale > 200 µm/m (Seuil de fissuration) ---
        if (fabsf(mesure) > 200.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = mesure;
            alr.seuil = 200.0f;
            strcpy(alr.action, "URGENT: Seuil critique de fissuration dépassé");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
        // --- 2. CAS ROUGE : Saut important (> 25 µm/m) ---
        else if (saut > 25.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = saut;
            alr.seuil = 25.0f;
            strcpy(alr.action, "Alerte Rouge!!: Augmentation brutale de la déformation");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
        // --- 3. CAS JAUNE : Saut moderé (> 10 µm/m) ---
        else if (saut > 10.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "JAUNE");
            alr.valeur = saut;
            alr.seuil = 10.0f;
            strcpy(alr.action, "Alerte Jaune: Augmentation progressive par rapport au jour précèdent");

            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
    }
    // --- 4. DÉTECTION D'ASYMÉTRIE (Fissuration / Comportement anormal entre capteurs) ---
    // Exemple : Comparaison entre C06 (Travée Centre-N, index 5) et C07 (Travée Centre-S, index 6)
    int T_centre_n = 5; // C06
    int T_centre_s = 6; // C07

    if (T_centre_n < n && T_centre_s < n) {
        float diff_asymetrie = fabsf(capteurs[T_centre_n].valeur_mesuree - capteurs[T_centre_s].valeur_mesuree);

        // Si l'écart entre les deux côtés de la travée dépasse 15 µm/m
        if (diff_asymetrie > 15.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = T_centre_n;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = diff_asymetrie;
            alr.seuil = 15.0f;
            strcpy(alr.action, "ALERTE ASYMETRIE: Ecart important entre Travee Centre-N et Centre-S (Risque fissuration)");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
    }
}

void detecter_anomalies_vibration(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr){
    const char *DATE_COURANTE = "30/07/2026 18:00"; // Horodatage courant des mesures
    for (int i = 0; i < n; i++) {
        // Filtrage : on ne traite que les capteurs de vibration
        if (strcmp(capteurs[i].type, "VIBR") != 0) {
            continue;
        }

        float freq = capteurs[i].valeur_mesuree;

        // --- 1. CAS ROUGE : Fréquence hors limites critiques (< 0.30 Hz ou > 0.60 Hz) ---
        if (freq > 0.60f || freq < 0.30f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "VIBRATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = freq;
            alr.seuil = (freq > 0.60f) ? 0.60f : 0.30f;
            strcpy(alr.action, "URGENT: Frequence critique hors norme - Risque d'instabilite ou perte de raideur");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
        // --- 2. CAS JAUNE : Dérive de fréquence (0.51 Hz à 0.60 Hz) ---
        else if (freq >= 0.51f && freq <= 0.60f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "VIBRATION");
            strcpy(alr.niveau, "JAUNE");
            alr.valeur = freq;
            alr.seuil = 0.50f; // La limite nominale supérieure en service est 0.50 Hz
            strcpy(alr.action, "Alerte Jaune: Derive de frequence observee (augmentation de raideur)");

            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
    }
}

void detecter_anomalies_charge(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr) {
    float charge_nord = -1.0, charge_sud = -1.0;
    int idx_nord = -1;
// ÉTAPE 1 : Détection des erreurs & surcharges individuelles (Capteur par capteur)
    for (int i = 0; i < n; i++) {
        if (strcmp(capteurs[i].type, "CHARGE") == 0) {
            float div = (capteurs[i].valeur_mesuree / capteurs[i].valeur_nominale)*100;
            if (div > 80.0) {
                capteurs[i].etat = 2;
                sprintf(capteurs[i].remarque, "Utilisation de %f de la capacite", div);
                Alerte alerts = {"27/07/2026 10:00", i, "SURCHARGE", "JAUNE", capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.80, "Diminuer les charges lourdes"};
                alertes[(*nb_alr)++]=alerts;//on initialise un tableau d'alertes
            }
            else if (div > 90.0) {
                capteurs[i].etat = 3;
                sprintf(capteurs[i].remarque, "Utilisation de %f de la capacité", div);
                Alerte alerts = {"27/07/2026 10:00", i, "SURCHARGE", "ROUGE", capteurs[i].valeur_mesuree, capteurs[i].valeur_nominale * 0.90, "Fermeture circulation Poids-Lourds"};
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

// Calcul de l'indice global et affichage des valeurs et des recommandations
void calculer_indice_sante(Capteur capteurs[], int n, Alerte alertes[], int nb_alr,
                          IndiceHealthStructural *sante, FILE *f) {
    sante->score_deformation = calculer_score_deformation(capteurs, n);
    sante->score_vibration = calculer_score_vibration(capteurs, n);
    sante->score_charge = calculer_score_charge(capteurs, n);

    sante->indice_global = (sante->score_deformation * 0.4 +
                            sante->score_vibration * 0.35 +
                            sante->score_charge * 0.25);

    // Affichage à l'écran (si f == NULL) ou dans le fichier (si f != NULL)
    if (f == NULL) {
        printf("\nRESUME DE SANTE STRUCTURALE\n");
        printf("Indice global (SHI)       : %.2f / 100 \n", sante->indice_global);
        printf("  Score deformation       : %.2f / 100 (40%% ponderation)\n", sante->score_deformation);
        printf("  Score vibration         : %.2f / 100 (35%% ponderation)\n", sante->score_vibration);
        printf("  Score charge            : %.2f / 100 (25%% ponderation)\n\n", sante->score_charge);

        if (sante->indice_global >= 90) {
            printf("Etat general            : BONNE SANTE\n");
            printf("Recommandation          : Inspections annuelles\n");
        }
        else if (sante->indice_global >= 80) {
            printf("Etat general            : ALERTE JAUNE\n");
            printf("Recommandation          : Inspections trimestrielles\n");
        }
        else if (sante->indice_global >= 70) {
            printf("Etat general            : ALERTE ORANGE\n");
            printf("Recommandation          : Inspections mensuelles\n");
        }
        else {
            printf("Etat general            : ALERTE ROUGE\n");
            printf("Recommandation          : Inspection d'urgence\n");
        }
    }
    else {
        fprintf(f, "\nRESUME DE SANTE STRUCTURALE\n");
        fprintf(f, "Indice global (SHI)       : %.2f / 100 \n", sante->indice_global);
        fprintf(f, "  Score deformation       : %.2f / 100 (40%% ponderation)\n", sante->score_deformation);
        fprintf(f, "  Score vibration         : %.2f / 100 (35%% ponderation)\n", sante->score_vibration);
        fprintf(f, "  Score charge            : %.2f / 100 (25%% ponderation)\n\n", sante->score_charge);

        if (sante->indice_global >= 90) {
            fprintf(f, "Etat general            : BONNE SANTE\n");
            fprintf(f, "Recommandation          : Inspections annuelles\n");
        }
        else if (sante->indice_global >= 80) {
            fprintf(f, "Etat general            : ALERTE JAUNE\n");
            fprintf(f, "Recommandation          : Inspections trimestrielles\n");
        }
        else if (sante->indice_global >= 70) {
            fprintf(f, "Etat general            : ALERTE ORANGE\n");
            fprintf(f, "Recommandation          : Inspections mensuelles\n");
        }
        else {
            fprintf(f, "Etat general            : ALERTE ROUGE\n");
            fprintf(f, "Recommandation          : Inspection d'urgence\n");
        }
    }
}

//Tri à bulles groupant capteurs par type (DEFORM, VIBR, CHARGE)
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

//Affiche le menu principal
void afficher_menu(){
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
    printf("Entrer votre choix:");
}

int charger_donnees_mesures(Capteur capteurs[], int n, const char *nom_fichier) {
    FILE *f = fopen(nom_fichier, "r");
    if (f == NULL) {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }
    int i = 0;
    while (fscanf(f,"ID:%5s, valeur_mesuree:%f, valeur_24h_avant:%f\n", capteurs[i].id, &capteurs[i].valeur_mesuree, &capteurs[i].valeur_precedente) == 3 && i < n) {
        i++;
    }
    fclose(f);
    return 1;
}

int sauvegarder_capteurs_binaire(Capteur capteurs[], int n, const char *nom_fichier) {
    FILE *f = fopen(nom_fichier, "wb");
    if (f == NULL) {
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    fwrite(capteurs, sizeof(Capteur), n, f);
    fclose(f);
    return 1;
    }
}
