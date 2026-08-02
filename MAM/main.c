
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pont.h"
#define NB_CAPTEURS 24
#define NB_ALERTES_MAX 50


int main(){
Alerte alertes[NB_ALERTES_MAX];
IndiceHealthStructural sante;
RapportInspection rapport;
int nb_alertes = 0;



// tableau des 24 capteurs
Capteur capteurs[NB_CAPTEURS] = {
    // 4 capteurs de déformation piles
    {"C01", "Pile Nord", "DEFORM", 125.3, 120.1, 150.0, 200.0, 150.0, DEFAULT, ""},
    {"C02", "Pile Centre 1", "DEFORM", 98.5, 95.0, 150.0, 200.0, 150.0, DEFAULT, ""},
    {"C03", "Pile Centre 2", "DEFORM", 102.4, 100.0, 150.0, 200.0, 150.0, DEFAULT, ""},
    {"C04", "Pile Sud", "DEFORM", 115.8, 112.0, 150.0, 200.0, 150.0, DEFAULT, ""},

    // 4 capteurs de déformation travées
    {"C05", "Travée Nord", "DEFORM", 45.2, 43.0, 80.0, 200.0, 80.0, DEFAULT, ""},
    {"C06", "Travée Centre-N", "DEFORM", 52.1, 50.0, 80.0, 200.0, 80.0, DEFAULT, ""},
    {"C07", "Travée Centre-S", "DEFORM", 48.9, 47.0, 80.0, 200.0, 80.0, DEFAULT, ""},
    {"C08", "Travée Sud", "DEFORM", 44.5, 42.0, 80.0, 200.0, 80.0, DEFAULT, ""},

    // 8 capteurs de vibration
    {"C09", "Noeud 1", "VIBR", 0.42, 0.41, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"C10", "Noeud 2", "VIBR", 0.38, 0.37, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"C11", "Noeud 3", "VIBR", 0.45, 0.44, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"C12", "Noeud 4", "VIBR", 0.40, 0.39, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"C13", "Noeud 5", "VIBR", 0.51, 0.42, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"C14", "Noeud 6", "VIBR", 0.39, 0.38, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"C15", "Noeud 7", "VIBR", 0.43, 0.42, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"C16", "Noeud 8", "VIBR", 0.41, 0.40, 0.50, 0.60, 0.425, DEFAULT, ""},

    // 8 capteurs de charge
    {"C17", "Pile Nord", "CHARGE", 2450.0, 2400.0, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"C18", "Pile Centre 1", "CHARGE", 2380.0, 2350.0, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"C19", "Pile Centre 2", "CHARGE", 2410.0, 2380.0, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"C20", "Pile Sud", "CHARGE", 2440.0, 2410.0, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"C21", "Appui Nord", "CHARGE", 1200.0, 1180.0, 1200.0, 1350.0, 1500.0, DEFAULT, ""},
    {"C22", "Appui Centre", "CHARGE", 1180.0, 1160.0, 1200.0, 1350.0, 1500.0, DEFAULT, ""},
    {"C23", "Appui Sud", "CHARGE", 1210.0, 1190.0, 1200.0, 1350.0, 1500.0, DEFAULT, ""},
    {"C24", "Appui Extreme", "CHARGE", 1195.0, 1175.0, 1200.0, 1350.0, 1500.0, DEFAULT, ""}
};


int choix ;
    do {
        afficher_menu();
        scanf("%d", &choix);
        switch (choix) {
            case 1:
                if (!charger_donnees_mesures(capteurs, NB_CAPTEURS, "mesures_capteurs.txt")) {
                    printf("Note : Fichier 'mesures_capteurs.txt' introuvable.\n");
                }
                else {
                    printf("Donnees de mesures chargees avec succes !\n");
                    sleep(2);
                }
                break;

            case 2:
               printf("\n--- Validation des Capteurs ---\n");
               for (int i = 0; i < NB_CAPTEURS; i++) {
                   if (!valider_capteur(&capteurs[i])){
                       printf("[ERREUR] Capteur %s -> %s hors plages physiques! Valeur = %.2f\n", capteurs[i].id ,capteurs[i].nom, capteurs[i].valeur_mesuree);
                    }
                    else{
                        printf("Capteur %s -> %-17s: %s\n", capteurs[i].id, capteurs[i].nom, "validee");
                    }
                }
                printf("\nValidation terminee.\n");
                sleep(2);

                sauvegarder_capteurs_binaire(capteurs, NB_CAPTEURS, "capteurs.dat");
                break;

            case 3:
                trier_capteurs_par_type(capteurs, NB_CAPTEURS);

                printf("\n%-6s %-16s %-8s %-10s %-8s %-10s\n", "ID", "Nom", "Type", "Valeur", "Etat", "Remarque");
                printf("--------------------------------------------------------------------\n");
                for (int i = 0; i < NB_CAPTEURS; i++){
                    printf("%-6s|%-16s %-8s %-10.2f %-8d %-10s\n", capteurs[i].id, capteurs[i].nom, capteurs[i].type, capteurs[i].valeur_mesuree, capteurs[i].etat, capteurs[i].remarque);
                }
                printf("--------------------------------------------------------------------\n");
                sleep(2);
                break;

            case 4:
                nb_alertes = 0; // Réinitialisation
                detecter_anomalies_vibration(capteurs, NB_CAPTEURS, alertes, &nb_alertes);
                detecter_anomalies_deformation(capteurs, NB_CAPTEURS,alertes, &nb_alertes);
                detecter_anomalies_charge(capteurs, NB_CAPTEURS, alertes, &nb_alertes);
                printf("Analyse effectuee. %d alerte(s) detectee(s) et archivee(s).\n", nb_alertes);
                sleep(2);
                break;

            case 5:
                calculer_indice_sante(capteurs, NB_CAPTEURS, alertes, nb_alertes, &sante, NULL);
                sleep(2);
                break;

            case 6:


                sleep(2);
                break;

            case 7:
                sleep(2);
                break;

            case 8:

                sleep(2);
                alertes_jour(capteurs, NB_CAPTEURS, alertes, nb_alertes);
                break;

            case 9:
                printf("Fermeture du système de suivi structural.\n");
                break;

            default:
                printf("Choix invalide. Réessayez.\n");
                sleep(2);
        }
    } while (choix != 9);



    return 0;
}


