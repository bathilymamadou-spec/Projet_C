
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pont.h"
#define NB_CAPTEURS 24
#define NB_ALERTES_MAX 50


int main()
{
// tableau des 24 capteurs
Capteur capteurs[NB_CAPTEURS] = {
    // 4 capteurs de déformation piles
    {"C01", "Pile Nord", "DEFORM", 125.3, 120.1, 150.0, 200.0, 150.0, OK, ""},
    {"C02", "Pile Centre 1", "DEFORM", 98.5, 95.0, 150.0, 200.0, 150.0, OK, ""},
    {"C03", "Pile Centre 2", "DEFORM", 102.4, 100.0, 150.0, 200.0, 150.0, OK, ""},
    {"C04", "Pile Sud", "DEFORM", 115.8, 112.0, 150.0, 200.0, 150.0, OK, ""},

    // 4 capteurs de déformation travées
    {"C05", "Travée Nord", "DEFORM", 45.2, 43.0, 80.0, 200.0, 80.0, OK, ""},
    {"C06", "Travée Centre-N", "DEFORM", 52.1, 50.0, 80.0, 200.0, 80.0, OK, ""},
    {"C07", "Travée Centre-S", "DEFORM", 48.9, 47.0, 80.0, 200.0, 80.0, OK, ""},
    {"C08", "Travée Sud", "DEFORM", 44.5, 42.0, 80.0, 200.0, 80.0, OK, ""},

    // 8 capteurs de vibration
    {"C09", "Noeud 1", "VIBR", 0.42, 0.41, 0.50, 0.60, 0.425, OK, ""},
    {"C10", "Noeud 2", "VIBR", 0.38, 0.37, 0.50, 0.60, 0.425, OK, ""},
    {"C11", "Noeud 3", "VIBR", 0.45, 0.44, 0.50, 0.60, 0.425, OK, ""},
    {"C12", "Noeud 4", "VIBR", 0.40, 0.39, 0.50, 0.60, 0.425, OK, ""},
    {"C13", "Noeud 5", "VIBR", 0.51, 0.42, 0.50, 0.60, 0.425, JAUNE, "Alerte jaune"},
    {"C14", "Noeud 6", "VIBR", 0.39, 0.38, 0.50, 0.60, 0.425, OK, ""},
    {"C15", "Noeud 7", "VIBR", 0.43, 0.42, 0.50, 0.60, 0.425, OK, ""},
    {"C16", "Noeud 8", "VIBR", 0.41, 0.40, 0.50, 0.60, 0.425, OK, ""},

    // 8 capteurs de charge
    {"C17", "Pile Nord", "CHARGE", 2450.0, 2400.0, 2400.0, 2700.0, 3000.0, JAUNE, ""},
    {"C18", "Pile Centre 1", "CHARGE", 2380.0, 2350.0, 2400.0, 2700.0, 3000.0, OK, ""},
    {"C19", "Pile Centre 2", "CHARGE", 2410.0, 2380.0, 2400.0, 2700.0, 3000.0, JAUNE, "Utlisation 81% de la capacité"},
    {"C20", "Pile Sud", "CHARGE", 2440.0, 2410.0, 2400.0, 2700.0, 3000.0, JAUNE, "Alerte jaune"},
    {"C21", "Appui Nord", "CHARGE", 1200.0, 1180.0, 1200.0, 1350.0, 1500.0, OK, ""},
    {"C22", "Appui Centre", "CHARGE", 1180.0, 1160.0, 1200.0, 1350.0, 1500.0, OK, ""},
    {"C23", "Appui Sud", "CHARGE", 1210.0, 1190.0, 1200.0, 1350.0, 1500.0, JAUNE, "Alerte jaune"},
    {"C24", "Appui Extreme", "CHARGE", 1195.0, 1175.0, 1200.0, 1350.0, 1500.0, OK, ""}
};

Alerte alertes[] ={ {"27/07/2026 10:00", 1, "SURCHARGE", "ROUGE", capteurs[1].valeur_mesuree,
                    capteurs[1].valeur_nominale * 0.90, "Fermeture circulation Poids Lourds"},
                     {"27/07/2026 10:00", 2, "SURCHARGE", "ROUGE", capteurs[2].valeur_mesuree,
                    capteurs[2].valeur_nominale * 0.90, "Fermeture circulation Poids Lourds"}

};

calculer_score_deformation(capteurs, NB_CAPTEURS);
calculer_score_vibration(capteurs, NB_CAPTEURS);
calculer_score_charge(capteurs, NB_CAPTEURS);










rapport_inspection(capteurs, NB_CAPTEURS,alertes, 2);

    return 0;
}
