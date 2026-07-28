#ifndef PONT_H_INCLUDED
#define PONT_H_INCLUDED

typedef struct {
char id[6]; /* ex: "C01", "C15" */
char nom[40]; /* ex: "Pile Nord" */
char type[15]; /* "DEFORM", "VIBR", "CHARGE" */
float valeur_mesuree; /* valeur actuelle */
float valeur_precedente; /* mesure précédente (24h avant) */
float seuil_alerte_j; /* seuil alerte jaune */
float seuil_alerte_r; /* seuil alerte rouge */
float valeur_nominale; /* référence de bon fonctionnement */
int etat; /* 1=OK, 2=JAUNE, 3=ROUGE */
char remarque[100]; /* description alert */
} Capteur;

typedef struct {
char horodatage[20]; /* "JJ/MM/AAAA HH:MM" */
int num_capteur; /* index 0-23 */
char type_alerte[30]; /* "DEFORMATION", "VIBRATION", "SURCHARGE" */
char niveau[10]; /* "JAUNE", "ROUGE" */
float valeur;
float seuil;
char action[100]; /* action recommandée */
} Alerte;

typedef struct {
float indice_global; /* SHI global 0-100 */
float score_deformation; /* composante déformation */
float score_vibration; /* composante vibration */
float score_charge; /* composante charge */
int nb_alertes_jaunes;
int nb_alertes_rouges;
char diagnostic[100]; /* "EXCELLENT", "BON", "ALERTE", "CRITIQUE" */
char recommandation[200];/* action prioritaire */
} IndiceHealthStructural;

typedef struct {
char date[20]; /* "JJ/MM/AAAA" */
int nb_capteurs; /* 24 */
Capteur capteurs[24];
int nb_alertes;
Alerte alertes[50];
IndiceHealthStructural sante;
int conformite_eurocode; /* 1=OUI, 0=NON */
} RapportInspection;

int valider_capteur(Capteur *cap);
//Vérifie la cohérence et la plage physique du capteur. Retourne 1 si OK, 0 sinon.

void detecter_anomalies_deformation(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr);
//Détecte déformations croissantes (saut > 10 μm/m = alerte jaune, > 25 μm/m = alerte rouge), fissuration (asymétrie entre capteurs).

void detecter_anomalies_vibration(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr);
//Détecte fréquences propres hors plage nominale, comportement de résonance, perte de raideur.

void detecter_anomalies_charge(Capteur capteurs[], int n, Alerte alertes[], int *nb_alr);
//Détecte surcharges (> 80% capacité = jaune, > 90% = rouge), déséquilibre entre piles.

float calculer_score_deformation(Capteur capteurs[], int n);
//Calcule le score de déformation (0 - 100) en fonction des écarts. Pondération : piles vs travées.

float calculer_score_vibration(Capteur capteurs[], int n);
//Calcule le score de vibration (0 - 100) basé sur les fréquences propres.

float calculer_score_charge(Capteur capteurs[], int n);
//Calcule le score de charge (0 - 100) basé sur utilisation de capacité.

void calculer_indice_sante(Capteur capteurs[], int n, Alerte alertes[], int nb_alr, IndiceHealthStructural *sante);
//Combine les trois scores avec pondérations (40% déf, 35% vibr, 25% charge).
//Génère diagnostic et recommandations.
void trier_capteurs_par_type(Capteur capteurs[], int n);
//Tri à bulles groupant capteurs par type (DEFORM, VIBR, CHARGE).

Capteur* recherche_capteur_critique(Capteur capteurs[], int n);
/*Recherche séquentielle du capteur avec l'état le plus grave (ROUGE prioritaire).
Retourne pointeur ou NULL.*/

void afficher_menu();
//Affiche menu principal

#endif // PONT_H_INCLUDED
