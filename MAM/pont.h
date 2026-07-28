#ifndef PONT_H_INCLUDED
#define PONT_H_INCLUDED

typedef enum{
    DEFORMATION,
    VIBRATION,
    CHARGE
}Type_capteur;


typedef struct {
char id[6]; /* ex: "C01", "C15" */
char nom[40]; /* ex: "Pile Nord" */
Type_capteur type; /* Enumératîon des types */
float valeur_mesuree; /* valeur actuelle */
float valeur_precedente; /* mesure précédente (24h avant) */
float seuil_alerte_j; /* seuil alerte jaune */
float seuil_alerte_r; /* seuil alerte rouge */
float valeur_nominale; /* référence de bon fonctionnement */
int etat; /* 1=OK, 2=JAUNE, 3=ROUGE */
char remarque[100]; /* description alert */
} Capteur;


typedef struct {
char horodatage[20];
int num_capteur;
char type_alerte[30];
char niveau[10];
float valeur;
float seuil;
char action[100];
} Alerte;
/* "JJ/MM/AAAA HH:MM" */
/* index 0-23 */
/* "DEFORMATION", "VIBRATION", "SURCHARGE" */
/* "JAUNE", "ROUGE" */
/* action recommandée */


typedef struct {
float indice_global; /* SHI global 0-100 */
float score_deformation; /* composante déformation */
float score_vibration; /* composante vibration */
float score_charge; /* composante charge */
int nb_alertes_jaunes;
int nb_alertes_rouges;
char diagnostic[100];  /* "EXCELLENT", "BON", "ALERTE", "CRITIQUE" */

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















float calculer_score_deformation(Capteur capteurs[], int n);
float calculer_score_vibration(Capteur capteurs[], int n);
float calculer_score_charge(Capteur capteurs[], int n);
void calculer_indice_sante(Capteur capteurs[], int n, Alerte alertes[], int nb_alr,
IndiceHealthStructural *sante);

#endif // PONT_H_INCLUDED
