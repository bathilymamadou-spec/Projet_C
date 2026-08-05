#include <stdio.h>
#include <stdlib.h>
#include <string.h> //pour utiliser strcmp, strcpy, strstr...
#include <math.h> // pour fabs
#include <time.h> // permet d'obtenir la date du jour
#include "pont.h"
#define NB_CAPTEURS 24
#define NB_ALERTES_MAX 50

//fonction convertissant l'énumaration en chaine
const char* etatToString(Etat etat) {
    switch(etat) {
        case OK:
            return "OK";
        case JAUNE:
            return "JAUNE";
        case ROUGE:
            return "ROUGE";
        default:
            return "INCONNU";
    }
}

//On vérfie si les mesures du capteur sont valides
int valider_capteur(Capteur *cap){

    // --- CASE 1 : CAPTEURS DE DÉFORMATION ---
    if (strcmp(cap->type, "DEFORM")==0){

        // Vérification de la cohérence physique selon le type de capteur
            // Une déformation aberrante dépasserait largement les plages physiques admissibles
        if (cap->valeur_mesuree<-700 || cap->valeur_mesuree>700){
            strcpy(cap->remarque, "ERREUR: Signal hors plage physique");
            return 0;
        }
        // Évaluation de l'état et rédaction de la remarque
        else{
            float saut=fabsf(cap->valeur_mesuree-cap->valeur_precedente);
            if (fabsf(cap->valeur_mesuree)>200 || saut>25.0f){
                cap->etat=ROUGE;
                strcpy(cap->remarque, "ALERTE ROUGE: Seuil critique");
            }
            else if (saut>10.0f){
                cap->etat=JAUNE;
                strcpy(cap->remarque, "ALERTE JAUNE: Augmentation du saut");
            }
            else{
                cap->etat=OK;
                strcpy(cap->remarque, "OK: Deformation normale");
            }
            return 1;
        }
    }


    // --- CASE 2 : CAPTEURS DE VIBRATION ---
    if (strcmp(cap->type, "VIBR")==0){
    // Vérification de la cohérence physique selon le type de capteur
        // Une fréquence de vibration ne peut pas être négative ou exagérément élevée
        if (cap->valeur_mesuree<0 || cap->valeur_mesuree>2){
            strcpy(cap->remarque, "ERREUR: Signal de vibration incoherent");
            return 0;
        }
        else{
            if (cap->valeur_mesuree>0.60f || cap->valeur_mesuree<0.30f) {

                cap->etat = ROUGE;
                strcpy(cap->remarque, "ALERTE ROUGE: Fréquence hors plage");
            }
            else if (cap->valeur_mesuree>=0.51f && cap->valeur_mesuree<=0.60f) {
                cap->etat=JAUNE;
                strcpy(cap->remarque, "ALERTE JAUNE: Augmentation de la fréquence");
            }
            else {
                cap->etat=OK;
                strcpy(cap->remarque, "OK: Fréquence normale");
            }
            return 1;
        }
    }


    // --- CASE 3 : CAPTEURS DE CHARGE ---
    if (strcmp(cap->type, "CHARGE")==0){
    // Vérification de la cohérence physique selon le type de capteur
        // Une charge ne peut pas être négative ou dépasser largement la capacité critique
        if (cap->valeur_mesuree<0 || cap->valeur_mesuree>5000){
            strcpy(cap->remarque, "ERREUR: Valeur de charge absurde");
            return 0;
        }
    // Évaluation de l'état et rédaction de la remarque
        else{
            if (cap->valeur_mesuree>cap->seuil_alerte_r) {
                cap->etat = ROUGE; // ROUGE
                sprintf(cap->remarque, "Utilisation de %f de la capacité", (cap->valeur_mesuree/cap->valeur_nominale)*100);
            }
            else if (cap->valeur_mesuree>cap->seuil_alerte_j) {
                cap->etat = JAUNE; // JAUNE
                sprintf(cap->remarque, "Utilisation de %f de la capacite", (cap->valeur_mesuree/cap->valeur_nominale)*100);
            }
            else {
                cap->etat = OK; // OK
                strcpy(cap->remarque, "OK: Charge normale");
            }
            return 1; // Mesure valide
        }
    }
}


//On vérfie les anomalies de la déformation
void detecter_anomalies_deformation(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr){
    int nbr = 0;
    for (int i = 0; i < n; i++){
        if (capteurs[i].valeur_mesuree!=DEFAULT){
            nbr++;
        }
    }
    if (nbr==0){
        printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
    }
    else{
        for (int i = 0; i < NB_CAPTEURS; i++){
            if (capteurs[i].valeur_mesuree!=DEFAULT){
                if (!valider_capteur(&capteurs[i])){
                    printf("[ERREUR] Capteur %s -> %s hors plages physiques! Valeur = %.2f\n", capteurs[i].id ,capteurs[i].nom, capteurs[i].valeur_mesuree);
                }
            }
        }
    }

    //on receuille le date courante
    time_t t = time(NULL);
    struct tm *now = localtime(&t);

    char DATE_COURANTE[30];
    sprintf(DATE_COURANTE,"%d-%d-%d %d:%d:%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec); //pour stocker la date actuelle dans la variable
    for(int i=0; i<n; i++){
    // On ne traite que les capteurs de déformation
        if(strcmp(capteurs[i].type, "DEFORM")!=0){
            continue;
        }
        float mesure = capteurs[i].valeur_mesuree;
        float precedente = capteurs[i].valeur_precedente;
        float saut = fabsf(mesure - precedente);

        // ---  CAS CRITIQUE : Déformation unilatérale > 200 µm/m (Seuil de fissuration) ---
        if (fabsf(mesure) > 200.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = mesure;
            alr.seuil = 200.0f;
            strcpy(alr.action, "URGENT: Seuil critique de fissuration dépassé");
            alertes[*nb_alr] = alr; //on l'insère dans un tableau d'alertes
            (*nb_alr)++;
        }

        // --- CAS ROUGE : Saut important (> 25 µm/m) ---
        else if (saut > 25.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = saut;
            alr.seuil = 25.0f;
            strcpy(alr.action, "Alerte Rouge!!: Augmentation brutale de la déformation");
            alertes[*nb_alr] = alr; //on l'insère dans un tableau d'alertes
            (*nb_alr)++;
        }

        // --- CAS JAUNE : Saut moderé (> 10 µm/m) ---
        else if (saut > 10.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "JAUNE");
            alr.valeur = saut;
            alr.seuil = 10.0f;
            strcpy(alr.action, "Alerte Jaune: Augmentation progressive par rapport au jour précèdent");
            alertes[*nb_alr] = alr; //on l'insère dans un tableau d'alertes
            (*nb_alr)++;
        }
    }

    // ---  DÉTECTION D'ASYMÉTRIE (Fissuration / Comportement anormal entre capteurs) ---
    // Déclaration des variables pour les déformations
    float def_p_nord = -1.0, def_p_sud = -1.0;
    float def_p_c1 = -1.0, def_p_c2 = -1.0;
    float def_t_nord = -1.0, def_t_sud = -1.0;
    float def_t_cn = -1.0, def_t_cs = -1.0;

    int idx_p_nord = -1, idx_p_c1 = -1;
    int idx_t_nord = -1, idx_t_cn = -1;

    // --- DÉTECTION D'ASYMÉTRIE / DÉFORMATION (Piles et Travées) ---
    // Récupération des valeurs de déformation
    for (int i = 0; i < n; i++) {
        if (strstr(capteurs[i].type, "DEFORM") != NULL) {
            // Piles
            if (strstr(capteurs[i].nom, "Pile Nord") != NULL) {
                def_p_nord = capteurs[i].valeur_mesuree;
                idx_p_nord = i;
            }
            else if (strstr(capteurs[i].nom, "Pile Sud") != NULL) {
                def_p_sud = capteurs[i].valeur_mesuree;
            }
            else if (strstr(capteurs[i].nom, "Pile Centre 1") != NULL) {
                def_p_c1 = capteurs[i].valeur_mesuree;
                idx_p_c1 = i;
            }
            else if (strstr(capteurs[i].nom, "Pile Centre 2") != NULL) {
                def_p_c2 = capteurs[i].valeur_mesuree;
            }
            // Travées
            else if (strstr(capteurs[i].nom, "Travée Nord") != NULL) {
                def_t_nord = capteurs[i].valeur_mesuree;
                idx_t_nord = i;
            }
            else if (strstr(capteurs[i].nom, "Travée Sud") != NULL) {
                def_t_sud = capteurs[i].valeur_mesuree;
            }
            else if (strstr(capteurs[i].nom, "Travée Centre-N") != NULL) {
                def_t_cn = capteurs[i].valeur_mesuree;
                idx_t_cn = i;
            }
            else if (strstr(capteurs[i].nom, "Travée Centre-S") != NULL) {
                def_t_cs = capteurs[i].valeur_mesuree;
            }
        }
    }

    // 1. Asymétrie Piles de Déformation (Pile Nord vs Pile Sud)
    if (def_p_nord != -1.0 && def_p_sud != -1.0) {
        float diff = fabsf(def_p_nord - def_p_sud);
        if (diff > 30.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = idx_p_nord;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = diff;
            alr.seuil = 30.0f;
            strcpy(alr.action, "ALERTE ASYMETRIE: Ecart important entre Pile Nord et Pile Sud (Deformation)");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
    }

    // 2. Asymétrie Piles de Déformation (Pile Centre 1 vs Pile Centre 2)
    if (def_p_c1 != -1.0 && def_p_c2 != -1.0) {
        float diff = fabsf(def_p_c1 - def_p_c2);
        if (diff > 30.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = idx_p_c1;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = diff;
            alr.seuil = 30.0f;
            strcpy(alr.action, "ALERTE ASYMETRIE: Ecart important entre Pile Centre 1 et Pile Centre 2 (Deformation)");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
    }

    // 3. Asymétrie Travées de Déformation (Travée Nord vs Travée Sud)
    if (def_t_nord != -1.0 && def_t_sud != -1.0) {
        float diff = fabsf(def_t_nord - def_t_sud);
        if (diff > 15.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = idx_t_nord;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = diff;
            alr.seuil = 15.0f;
            strcpy(alr.action, "ALERTE ASYMETRIE: Ecart important entre Travee Nord et Travee Sud (Risque fissuration)");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
    }

    // 4. Asymétrie Travées de Déformation (Travée Centre-N vs Travée Centre-S)
    if (def_t_cn != -1.0 && def_t_cs != -1.0) {
        float diff = fabsf(def_t_cn - def_t_cs);
        if (diff > 15.0f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = idx_t_cn;
            strcpy(alr.type_alerte, "DEFORMATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = diff;
            alr.seuil = 15.0f;
            strcpy(alr.action, "ALERTE ASYMETRIE: Ecart important entre Travee Centre-N et Travee Centre-S (Risque fissuration)");
            alertes[*nb_alr] = alr;
            (*nb_alr)++;
        }
    }
}


//Détecte fréquences propres hors plage nominale, comportement de résonance, perte de raideur
void detecter_anomalies_vibration(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr){
    int nbr = 0;
    for (int i = 0; i < n; i++){
        if (capteurs[i].valeur_mesuree!=DEFAULT){
            nbr++;
        }
    }
    if (nbr==0){
        printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
    }
    else{
        for (int i = 0; i < n; i++){
            if (capteurs[i].valeur_mesuree!=DEFAULT){
                if (!valider_capteur(&capteurs[i])){
                    printf("[ERREUR] Capteur %s -> %s hors plages physiques! Valeur = %.2f\n", capteurs[i].id ,capteurs[i].nom, capteurs[i].valeur_mesuree);
                }
            }
            else{
                printf("%s n'a pas encore de valeurs. Veuillez charger ses donnees des mesures.\n", capteurs[i].nom);
            }
        }
    }

    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    char DATE_COURANTE[30];
    sprintf(DATE_COURANTE,"%d-%d-%d %d:%d:%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec); //pour stocker la date actuelle dans la variable
    for (int i = 0; i < n; i++) {

    // Filtrage : on ne traite que les capteurs de vibration
        if (strcmp(capteurs[i].type, "VIBR") != 0) {
            continue;
        }
        float freq = capteurs[i].valeur_mesuree;

        // ---  CAS ROUGE : Fréquence hors limites critiques (< 0.30 Hz ou > 0.60 Hz) ---
        if (freq > 0.60f || freq < 0.30f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "VIBRATION");
            strcpy(alr.niveau, "ROUGE");
            alr.valeur = freq;
            alr.seuil = (freq > 0.60f) ? 0.60f : 0.30f;
            strcpy(alr.action, "URGENT: Frequence critique hors norme - Risque d'instabilite ou perte de raideur");
            alertes[*nb_alr] = alr; //on l'insère dans un tableau d'alertes
            (*nb_alr)++;
        }

        // --- CAS JAUNE : Dérive de fréquence (0.51 Hz à 0.60 Hz) ---
        else if (freq >= 0.51f && freq <= 0.60f) {
            Alerte alr;
            strcpy(alr.horodatage, DATE_COURANTE);
            alr.num_capteur = i;
            strcpy(alr.type_alerte, "VIBRATION");
            strcpy(alr.niveau, "JAUNE");
            alr.valeur = freq;
            alr.seuil = 0.50f; // La limite nominale supérieure en service est 0.50 Hz
            strcpy(alr.action, "Alerte Jaune: Derive de frequence observee (augmentation de raideur)");
            alertes[*nb_alr] = alr; //on l'insère dans un tableau d'alertes
            (*nb_alr)++;
        }
    }

}


//Détecte surcharges (> 80% capacité = jaune, > 90% = rouge), déséquilibre entre piles
void detecter_anomalies_charge(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr) {
    int nbr = 0;
    for (int i = 0; i < n; i++){
        if (capteurs[i].valeur_mesuree!=DEFAULT){
            nbr++;
        }
    }
    if (nbr==0){
        printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
        return;
    }
    else{
        for (int i = 0; i < n; i++){
            if (capteurs[i].valeur_mesuree!=DEFAULT){
                if (!valider_capteur(&capteurs[i])){
                    printf("[ERREUR] Capteur %s -> %s hors plages physiques! Valeur = %.2f\n", capteurs[i].id ,capteurs[i].nom, capteurs[i].valeur_mesuree);
                    continue;
                }

            }
        }
    }

    //on receuille le date courante
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    char DATE_COURANTE[30];
    sprintf(DATE_COURANTE,"%d-%d-%d %d:%d:%d", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);

    // ÉTAPE 1 : Détection des erreurs & surcharges individuelles (Capteur par capteur)
    for (int i = 0; i < n; i++){
        if (strcmp(capteurs[i].type, "CHARGE") == 0) {
            float div = (capteurs[i].valeur_mesuree / capteurs[i].valeur_nominale)*100;
                if (div > 80.0){
                    Alerte alr;
                    strcpy(alr.horodatage, DATE_COURANTE);
                    alr.num_capteur = i;
                    strcpy(alr.type_alerte, "SURCHARGE");
                    strcpy(alr.niveau, "JAUNE");
                    alr.valeur = capteurs[i].valeur_mesuree;
                    alr.seuil = capteurs[i].valeur_nominale * 0.80;
                    strcpy(alr.action, "Diminuer les charges lourdes");
                    alertes[*nb_alr] = alr;//on l'insère dans un tableau d'alertes
                    (*nb_alr)++;
                }
                else if (div > 90.0){
                    capteurs[i].etat = ROUGE;
                    sprintf(capteurs[i].remarque, "Utilisation de %f de la capacité", div);
                    Alerte alr;
                    strcpy(alr.horodatage, DATE_COURANTE);
                    alr.num_capteur = i;
                    strcpy(alr.type_alerte, "SURCHARGE");
                    strcpy(alr.niveau, "JAUNE");
                    alr.valeur = capteurs[i].valeur_mesuree;
                    alr.seuil = capteurs[i].valeur_nominale * 0.80;
                    strcpy(alr.action, "Fermeture circulation Poids-Lourds");
                    alertes[*nb_alr] = alr;//on l'insère dans un tableau d'alertes
                    (*nb_alr)++;
                }


    }
    }

    // ÉTAPE 2 : Détection du déséquilibre entre piles
    // Déclaration des variables initiales
    float charge_nord = -1.0, charge_sud = -1.0;
    float charge_c1 = -1.0, charge_c2 = -1.0;
    int idx_nord = -1, idx_c1 = -1;

    // Récupération des charges pour toutes les piles (Nord, Sud, Centre 1, Centre 2)
    for (int i = 0; i < n; i++) {
        if (strstr(capteurs[i].type, "CHARGE") != NULL) {
            if (strstr(capteurs[i].nom, "Pile Nord") != NULL) {
                charge_nord = capteurs[i].valeur_mesuree;
                idx_nord = i;
            }
            else if (strstr(capteurs[i].nom, "Pile Sud") != NULL) {
                charge_sud = capteurs[i].valeur_mesuree;
            }
            else if (strstr(capteurs[i].nom, "Pile Centre 1") != NULL) {
                charge_c1 = capteurs[i].valeur_mesuree;
                idx_c1 = i;
            }
            else if (strstr(capteurs[i].nom, "Pile Centre 2") != NULL) {
                charge_c2 = capteurs[i].valeur_mesuree;
            }
        }
    }

    // --- 2.1 : Calcul du déséquilibre entre Pile Nord et Pile Sud ---
    if (charge_nord != -1.0 && charge_sud != -1.0) {
        float ecart_ext = charge_nord - charge_sud;
        if (ecart_ext < 0) ecart_ext = -ecart_ext; // Valeur absolue

        if (ecart_ext > 100.0) {
            Alerte al_des;
            strcpy(al_des.horodatage, DATE_COURANTE);
            al_des.num_capteur = idx_nord;
            strcpy(al_des.type_alerte, "SURCHARGE");
            strcpy(al_des.niveau, "JAUNE");
            al_des.valeur = ecart_ext;
            al_des.seuil = 100.0;
            strcpy(al_des.action, "Verification de charge entre Pile Nord et Pile Sud");
            alertes[*nb_alr] = al_des; // on l'insère dans un tableau d'alertes
            (*nb_alr)++;
        }
    }

    // --- 2.2 : Calcul du déséquilibre entre Pile Centre 1 et Pile Centre 2 ---
    if (charge_c1 != -1.0 && charge_c2 != -1.0) {
        float ecart_centre = charge_c1 - charge_c2;
        if (ecart_centre < 0) ecart_centre = -ecart_centre; // Valeur absolue

        if (ecart_centre > 100.0) {
            Alerte al_des;
            strcpy(al_des.horodatage, DATE_COURANTE);
            al_des.num_capteur = idx_c1;
            strcpy(al_des.type_alerte, "SURCHARGE");
            strcpy(al_des.niveau, "JAUNE");
            al_des.valeur = ecart_centre;
            al_des.seuil = 100.0;
            strcpy(al_des.action, "Verification de charge entre Pile Centre 1 et Pile Centre 2");
            alertes[*nb_alr] = al_des; // on l'insère dans un tableau d'alertes
            (*nb_alr)++;
        }
    }

}


//fonction de detections des anomalies
void detecter_anomalies(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr){
    int nbr = 0;
    //  Compter les capteurs qui ont des valeurs
    for (int i = 0; i < n; i++){
        if (capteurs[i].valeur_mesuree != DEFAULT){
            nbr++;
        }
    }

    // Vérifier que les données sont chargées
    if (nbr == 0){
        printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
        return;
    }

    // Détecter les anomalies
    detecter_anomalies_vibration(capteurs, NB_CAPTEURS, alertes, nb_alr);
    detecter_anomalies_deformation(capteurs, NB_CAPTEURS, alertes, nb_alr);
    detecter_anomalies_charge(capteurs, NB_CAPTEURS, alertes, nb_alr);
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

    float somme_ponderee = (somme_pile * 0.7 + somme_travee * 0.3);
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
            max += 0.175;
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
        int nbr =0;
     //On s'assure que les capteurs ont des valeurs avant de calculer les indices
     for (int i = 0; i < NB_CAPTEURS; i++){
                   if (capteurs[i].valeur_mesuree!=DEFAULT){
                        nbr++;
                    }
               }
                if (nbr==0){
                    printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
                    return;
                }
                else{
                   for (int i = 0; i < NB_CAPTEURS; i++){
                       if (capteurs[i].valeur_mesuree==DEFAULT){
                            printf("%s n'a pas encore de valeurs. Veuillez charger ses donnees des mesures.\n", capteurs[i].nom);
                            return;
                        }

                       }

                   }




    sante->score_deformation = calculer_score_deformation(capteurs, n);
    sante->score_vibration = calculer_score_vibration(capteurs, n);
    sante->score_charge = calculer_score_charge(capteurs, n);

    sante->indice_global = (sante->score_deformation * 0.4 +
                            sante->score_vibration * 0.35 +
                            sante->score_charge * 0.25);

    // Affichage à l'écran (si f == NULL)
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

    // On affiche dans le fichier (si f != NULL)
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


//Recherche séquentielle du capteur avec l'état le plus grave (ROUGE prioritaire).
Capteur* recherche_capteur_critique(Capteur capteurs[], int n){
    Capteur *pointeur = NULL; //on initialise le pointeur
    int max=1;
    for (int i = 0; i < n; i++){
        if (capteurs[i].etat > max){
            max = capteurs[i].etat; //on prends etat le plus eleve a chaque tour
            pointeur = &capteurs[i]; //on stock le capteur correspondant dans la variable
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
void viderBuffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


//charger les données des mesure en mémoire
int charger_donnees_mesures(Capteur capteurs[], int n){
    char nom_fichier[50];
    strcpy(nom_fichier, "mesures_capteurs.txt");
    FILE *f = fopen(nom_fichier, "r");
    if (f == NULL) {
        perror("Erreur d'ouverture du fichier");
        return  EXIT_FAILURE;
    }
    int i = 0;
    int nb_charges = 0; // Compteur de capteurs correctement mis à jour
    char ligne[256];


    // On lit ligne par ligne
    while (i < 24 && fgets(ligne, sizeof(ligne), f) != NULL) {
        char temp_id[10];
        float temp_val, temp_prec;

        // 1. On lit les données dans des variables temporaires
        int lus = sscanf(ligne, "ID:%[^,], valeur_mesuree:%f, valeur_24h_avant:%f",
                         temp_id, &temp_val, &temp_prec);

        if (lus == 3) {
            strcpy(capteurs[i].id, temp_id);
            capteurs[i].valeur_mesuree = temp_val;
            capteurs[i].valeur_precedente = temp_prec;
            nb_charges++; // Un capteur de plus a reçu ses mesures
        }
        else {
            printf("Avertissement : Ligne invalide ignoree -> %s", ligne);
            printf("Veuillez rectifier cette ligne pour charger correctement les donnees.\n\n");
            // Le capteur capteurs[i] est sauté et garde ses valeurs par défaut
        }
        i++;
    }

    fclose(f);

    // Si au moins un capteur a été mis à jour
    if (nb_charges > 0) {
        if (nb_charges==24){
            printf("\nDonnees de mesures chargees avec succes\n");
        }
        else{
            printf("\n(%d/24 capteurs mis a jour) !\n", nb_charges);
        }
    }
    else {
        printf("\nErreur : Aucune donnee valide n'a pu etre chargee.\n");
    }
}


//sauvegarde du tableau de structure
int sauvegarder_capteurs_binaire(Capteur capteurs[], int n){
    char nom_fichier[50];
    strcpy(nom_fichier, "capteurs.dat");
    FILE *f = fopen(nom_fichier, "wb");
    if (f == NULL){
        perror("Erreur d'ouverture du fichier");
        return EXIT_FAILURE;
    }
    fwrite(capteurs, sizeof(Capteur), n, f);
    fclose(f);
    return 1;
}


// Fonction pour la conformité
const char* conformite_cap(int valide) {
    if (valide == 1)
        return "OK";
    else
        return "Â vérifier";
}


void rapport_inspection(Capteur capteurs[], int n, Alerte alertes[], int nb_alr) {
    IndiceHealthStructural sante;
    int nbr = 0;
    for (int i = 0; i < NB_CAPTEURS; i++){
                   if (capteurs[i].valeur_mesuree!=DEFAULT){
                        nbr++;
                    }
               }
                if (nbr==0){
                    printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
                    return;
                }
                else{
                   for (int i = 0; i < NB_CAPTEURS; i++){
                       if (capteurs[i].valeur_mesuree==DEFAULT){
                            printf("%s n'a pas encore de valeurs. Veuillez charger ses donnees des mesures.\n", capteurs[i].nom);
                            return;
                        }
                   }

                }

    // Remplir RapportInspection
    RapportInspection rapport;

    time_t t = time(NULL); //on stocke le temps actuelle en seconde
    struct tm *tm = localtime(&t); //on convertit la structure dans un format lisible
        sprintf(rapport.date,
            "alertes_%02d-%02d-%04d.log",
            tm->tm_mday,
            tm->tm_mon + 1,
            tm->tm_year + 1900);

    // Créer le nom du fichier avec la date du jour
    char nom_fichier[50];
     //on ecrit dans la chaine de caractère
    sprintf(nom_fichier,
            "rapport_inspection_%02d-%02d-%04d.txt",
            tm->tm_mday,
            tm->tm_mon + 1,
            tm->tm_year + 1900);

    // Copier les capteurs
    for (int i = 0; i < n; i++) {
        rapport.capteurs[i] = capteurs[i];
    }

    // Copier les alertes (nb_alr peut être différent de n)
    for (int i = 0; i < nb_alr; i++) {
        rapport.alertes[i] = alertes[i];
    }


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

    fprintf(f, "\n======================================================\n");
    fprintf(f, "FIN DU RAPPORT\n");
    fprintf(f, "======================================================\n");

    fclose(f);

    printf(" Rapport genere : %s\n", nom_fichier);
}


void exporter_rapport_inspection(Capteur capteurs[], int n, Alerte alertes[], int nb_alr) {
    IndiceHealthStructural sante;
    int nbr = 0;
    for (int i = 0; i < NB_CAPTEURS; i++){
                   if (capteurs[i].valeur_mesuree!=DEFAULT){
                        nbr++;
                    }
               }
                if (nbr==0){
                    printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
                    return;
                }
                else{
                   for (int i = 0; i < NB_CAPTEURS; i++){
                       if (capteurs[i].valeur_mesuree==DEFAULT){
                            printf("%s n'a pas encore de valeurs. Veuillez charger ses donnees des mesures.\n", capteurs[i].nom);

                        }
                   }
                }

    // Remplir RapportInspection
    RapportInspection rapport;

    time_t t = time(NULL); //on stocke le temps actuelle en seconde
    struct tm *tm = localtime(&t); //on convertit la structure dans un format lisible

    sprintf(rapport.date,
            "alertes_%02d-%02d-%04d.log",
            tm->tm_mday,
            tm->tm_mon + 1,
            tm->tm_year + 1900);
    rapport.nb_capteurs = n;
    rapport.nb_alertes = nb_alr;

    rapport.nb_capteurs = n;
    rapport.nb_alertes = nb_alr;


    // Créer le nom du fichier avec la date du jour
    char nom_fichier[50];
     //on ecrit dans la chaine de caractère
    sprintf(nom_fichier,
            "rapport_inspection_%02d-%02d-%04d.txt",
            tm->tm_mday,
            tm->tm_mon + 1,
            tm->tm_year + 1900);

    // Copier les capteurs
    for (int i = 0; i < n; i++) {
        rapport.capteurs[i] = capteurs[i];
    }

    // Copier les alertes (nb_alr peut être différent de n)
    for (int i = 0; i < nb_alr; i++) {
        rapport.alertes[i] = alertes[i];
    }


    // Ouvrir le fichier
    FILE *f = fopen(nom_fichier, "w");
    if (f == NULL) {
        perror("fopen");
        return;
    }

    // Présentation du rapport
    printf( "======================================================\n");
    printf( "RAPPORT D'INSPECTION\n");
    printf( "Pont Faidherbe de Saint-Louis\n");
    printf( "Date : %s\n", rapport.date);
    printf( "Norme : EN 1999 (Eurocode 9) - Conception des structures en aluminium\n");
    printf( "======================================================\n");

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
    printf( "\nALERTES ACTIVES\n");
    printf( "Niveau JAUNE (%d alerte(s))\n", compt_jaune);
    // Afficher les alertes jaunes
    if (compt_jaune == 0)
        printf("  AUCUNE ALERTE");
    else{
        for (int i = 0; i < n; i++) {
            if (capteurs[i].etat == JAUNE) {
                printf( "- %s %s : %s\n", capteurs[i].id, capteurs[i].nom, capteurs[i].remarque);
        }
    }
}


    printf("\nNiveau ROUGE (%d alerte(s))\n", compt_rouge);
    // Afficher les alertes rouges
    if (compt_rouge == 0)
        printf( "  AUCUNE ALERTE\n");
    else {
        for (int i = 0; i < n; i++) {
            if (capteurs[i].etat == ROUGE) {
                printf( "- %s %s : %s\n", capteurs[i].id, capteurs[i].nom, capteurs[i].remarque);
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

    printf("\nCONFORMITE EUROCODE 9\n");
    printf("   Deformation vs limites           : %s\n", conform_deform);
    printf("   Vibration vs resonance           : %s\n", conform_vibr);
    printf("   Charge vs capacite               : %s\n", conform_charge);
    printf("\n");

    // Actions prioritaires
    printf("ACTIONS PRIORITAIRES\n");
    int action_number = 1;
    for (int i = 0; i < nb_alr; i++) {
        if (strlen(alertes[i].action) > 0) {
            printf("%d. %s\n", action_number++, alertes[i].action);
        }
    }
    if (action_number == 1) {
        printf("AUCUNE ACTION PRIORITAIRE\n");
    }

    printf("\n======================================================\n");
    printf("FIN DU RAPPORT\n");
    printf("======================================================\n");


    printf(" Rapport genere : %s\n", nom_fichier);
}

void alertes_jour(Capteur capteurs[], int n, Alerte alertes[], int nb_alertes) {
    time_t t = time(NULL); //on stocke le temps actuelle en seconde
    struct tm *tm = localtime(&t); //on convertit la structure dans un format lisible

    // Créer le nom du fichier avec la date du jour
    char nom_fichier[50];
     //on ecrit dans la chaine de caractère
    sprintf(nom_fichier,
            "alertes_%02d-%02d-%04d.log",
            tm->tm_mday,
            tm->tm_mon + 1,
            tm->tm_year + 1900);

    // Ouvrir le fichier en mode ajout (append)
    FILE *f_alert = fopen(nom_fichier, "a");
    if (f_alert == NULL) {
        perror("fopen");
        return;
    }

    // Vérifier s'il y a des alertes
    if (nb_alertes == 0) {
        fprintf(f_alert, "[%s] AUCUNE ALERTE\n", "00:00");
        printf("[%s] AUCUNE ALERTE\n", "00:00");
    } else {
        // Parcourir toutes les alertes
        for (int i = 0; i < nb_alertes; i++) {
            // affichages des ALERTE
            printf("[%s] ALERTE %s - %s (%s) %s : %.2f (seuil : %.2f) -> %s\n",
                   alertes[i].horodatage,
                   alertes[i].niveau,
                   capteurs[alertes[i].num_capteur].id,
                   capteurs[alertes[i].num_capteur].nom,
                   alertes[i].type_alerte,
                   alertes[i].valeur,
                   alertes[i].seuil,
                   alertes[i].action);

            fprintf(f_alert, "[%s] ALERTE %s - %s (%s) %s : %.2f (seuil : %.2f) -> %s\n",
                    alertes[i].horodatage,
                    alertes[i].niveau,
                    capteurs[alertes[i].num_capteur].id,
                    capteurs[alertes[i].num_capteur].nom,
                    alertes[i].type_alerte,
                    alertes[i].valeur,
                    alertes[i].seuil,
                    alertes[i].action);
        }
    }

    // Fermer le fichier
    fclose(f_alert);
    printf("Fichier alerte genere : %s\n", nom_fichier);
}


