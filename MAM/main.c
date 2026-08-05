
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
    {"", "Pile Nord", "DEFORM", DEFAULT, DEFAULT, 150.0, 200.0, 150.0, DEFAULT, ""},
    {"", "Pile Centre 1", "DEFORM", DEFAULT, DEFAULT, 150.0, 200.0, 150.0, DEFAULT, ""},
    {"", "Pile Centre 2", "DEFORM", DEFAULT, DEFAULT, 150.0, 200.0, 150.0, DEFAULT, ""},
    {"", "Pile Sud", "DEFORM", DEFAULT, DEFAULT, 150.0, 200.0, 150.0, DEFAULT, ""},

    // 4 capteurs de déformation travées
    {"", "Travée Nord", "DEFORM",DEFAULT, DEFAULT, 80.0, 200.0, 80.0, DEFAULT, ""},
    {"", "Travée Centre-N", "DEFORM", DEFAULT, DEFAULT, 80.0, 200.0, 80.0, DEFAULT, ""},
    {"", "Travée Centre-S", "DEFORM", DEFAULT, DEFAULT, 80.0, 200.0, 80.0, DEFAULT, ""},
    {"", "Travée Sud", "DEFORM", DEFAULT, DEFAULT, 80.0, 200.0, 80.0, DEFAULT, ""},

    // 8 capteurs de vibration
    {"", "Noeud 1", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"", "Noeud 2", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"", "Noeud 3", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"", "Noeud 4", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"", "Noeud 5", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"", "Noeud 6", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"", "Noeud 7", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},
    {"", "Noeud 8", "VIBR", DEFAULT, DEFAULT, 0.50, 0.60, 0.425, DEFAULT, ""},

    // 8 capteurs de charge
    {"", "Pile Nord", "CHARGE", DEFAULT, DEFAULT, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"", "Pile Centre 1", "CHARGE", DEFAULT, DEFAULT, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"", "Pile Centre 2", "CHARGE", DEFAULT, DEFAULT, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"", "Pile Sud", "CHARGE", DEFAULT, DEFAULT, 2400.0, 2700.0, 3000.0, DEFAULT, ""},
    {"", "Appui Nord", "CHARGE", DEFAULT, DEFAULT, 1200.0, 1350.0, 1500.0, DEFAULT, ""},
    {"", "Appui Centre", "CHARGE", DEFAULT, DEFAULT, 1200.0, 1350.0, 1500.0, DEFAULT, ""},
    {"", "Appui Sud", "CHARGE", DEFAULT, DEFAULT, 1200.0, 1350.0, 1500.0, DEFAULT, ""},
    {"", "Appui Extreme", "CHARGE", DEFAULT, DEFAULT, 1200.0, 1350.0, 1500.0, DEFAULT, ""}
};


int choix ;
    do {
        afficher_menu();
        // scanf retourne 1 si et seulement si la saisie est un entier valide
        if (scanf("%d", &choix) != 1) {
            printf("\nErreur : Saisie invalide ! Veuillez entrer un nombre entier.\n");
            viderBuffer(); // On élimine le caractère invalide
            choix = 0;      // On force une valeur neutre pour re-boucler sereinement
            continue;      // On repasse directement au debut
        }


        switch (choix) {
            case 1:
                charger_donnees_mesures(capteurs, NB_CAPTEURS);
                sleep(2);
                break;

            case 2:
               printf("\n--- Validation des Capteurs ---\n");
               int nbr=0;
               for (int i = 0; i < NB_CAPTEURS; i++){
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
                               printf("[ERREUR] Capteur %s -> %s hors plages physiques! Valeur = %.2f\n",
                                      capteurs[i].id ,capteurs[i].nom, capteurs[i].valeur_mesuree);
                            }
                            else{
                                printf("Capteur %s -> %-17s: %s\n", capteurs[i].id, capteurs[i].nom, "validee");
                            }
                       }
                        else{
                            printf("%s n'a pas encore de valeurs. Veuillez charger ses donnees des mesures.\n", capteurs[i].nom);
                        }
                   }


                }

                printf("--------------------------------------------------------------------\n");
                printf("Validation terminee.\n");

                //Pour recueillir les structures
                sauvegarder_capteurs_binaire(capteurs, NB_CAPTEURS);
                sleep(2);
                break;

            case 3:
                //Pour trier tous les capteurs avant de les afficher
               trier_capteurs_par_type(capteurs, NB_CAPTEURS);

                //Pour afficher l'état détaillé
               int nb=0;
               printf("\n%-6s %-16s %-8s %-10s %-8s %-10s\n", "ID", "Nom", "Type", "Valeur", "Etat", "Remarque");
               printf("--------------------------------------------------------------------\n");

               for (int i = 0; i < NB_CAPTEURS; i++){
                   if (capteurs[i].valeur_mesuree!=DEFAULT){//on vérifie que tous les capteurs ont été charger
                        nb++;
                    }
               }
                if (nb==0){
                    printf("Les capteurs n'ont pas encore de valeurs. Veuillez charger les donnees des mesures.\n");
                }
                else{
                    for (int i = 0; i < NB_CAPTEURS; i++){
                        if (capteurs[i].valeur_mesuree == DEFAULT) {
                            printf("%-17s | Statut : NON CHARGE (Donnees manquantes)\n ", capteurs[i].nom);
                        }
                        else{

                            printf("%-6s|%-16s %-8s %-10.2f %-8s %-10s\n",
                                   capteurs[i].id, capteurs[i].nom, capteurs[i].type, capteurs[i].valeur_mesuree, etatToString(capteurs[i].etat), capteurs[i].remarque);

                        }
                    }
                }
                //Pointeur pointant le capteur le plus critoqie
                Capteur *critique = recherche_capteur_critique(capteurs, NB_CAPTEURS);
                //affichage du dit capteur
                if (critique != NULL) {
                    printf("Capteur le plus critique :\n");
                    printf("  ID       : %s\n", critique->id);
                    printf("  Nom      : %s\n", critique->nom);
                    printf("  Type     : %s\n", critique->type);
                    printf("  Etat     : %s\n", etatToString(critique->etat));
                    printf("  Valeur   : %.2f\n", critique->valeur_mesuree);
                    printf("  Remarque : %s\n", critique->remarque);
                } else {
                    printf("Aucun capteur critique trouve (tous OK).\n");
                }
                break;
                printf("--------------------------------------------------------------------\n");
                sleep(2);
                break;

            case 4:
                nb_alertes = 0; // Réinitialisation
                detecter_anomalies(capteurs,  NB_CAPTEURS, alertes, &nb_alertes);

                if (nb_alertes == 0)
                    printf("\nAnalyse effectuee. aucune alerte detectee.\n");
                else
                    printf("\nAnalyse effectuee. %d alerte(s) detectee(s) et archivee(s).\n", nb_alertes);

                sleep(2);
                break;

            case 5:
                calculer_indice_sante(capteurs, NB_CAPTEURS, alertes, nb_alertes, &sante, NULL);
                sleep(2);
                break;

            case 6:
                rapport_inspection(capteurs, NB_CAPTEURS, alertes, nb_alertes);
                sleep(2);
                break;

            case 7:
                 exporter_rapport_inspection(capteurs, NB_CAPTEURS, alertes, nb_alertes);

                sleep(2);
                break;

            case 8:
                alertes_jour(capteurs, NB_CAPTEURS, alertes, nb_alertes);
                 sleep(2);
                break;

            case 9:
                printf("Fermeture du systeme de suivi structural.\n");
                break;

            default:
                printf("Choix invalide. Réessayez.\n");
        }
    } while (choix != 9);

    return 0;
}


