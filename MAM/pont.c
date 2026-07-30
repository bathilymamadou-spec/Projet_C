#include <stdio.h>
#include <stdlib.h>
#include "pont.h"

// Calcul du score de déformation pondéré
float calculer_score_deformation(Capteur capteurs[], int n) {
    float max = 0; // l'écart maximale possible
    float somme_pile = 0; //somme des écarts des capteurs Pile
    float somme_travee = 0; //somme des écarts des capteurs Travée

//La boucle traverse le tableau et ne prend en compte que les capteurs de déformation
    for (int i=0; i < n; i++){

        if (capteurs[i].type == DEFORMATION){
            float ecart = 0;
            //ecart et somme selon si capteur est pile ou travée
            if (capteurs[i].valeur_nominale == 150){
                ecart = capteurs[i].valeur_mesuree - capteurs[i].valeur_nominale;
                 somme_pile += ecart;
            }
            if (capteurs[i].valeur_nominale == 80){
                ecart = capteurs[i].valeur_mesuree - capteurs[i].valeur_nominale;
                 somme_travee += ecart;
            }
            max += 150;
        }
    }
    float somme_ponderee = (somme_pile*0.7 +somme_travee*0.3); //somme des écarts pondérés selon importance
  return 100 - ((somme_ponderee / max)*100); //Formule du SHI
};

// Calcul du score de vibration sur les fréquences propres
float calculer_score_vibration(Capteur capteurs[], int n) {
    float somme = 0; // la somme des écarts
    float max = 0; // l'écart maximale possible


//La boucle traverse le tableau et ne prend en compte que les capteurs de vibration
    for (int i=0; i < n; i++){
        if (capteurs[i].type == VIBRATION){
            float ecart = 0;
            //ecart selon si valeur est au-dessus ou en-dessous du seuil
            if (capteurs[i].valeur_mesuree > 0.5)
                ecart = capteurs[i].valeur_mesuree - 0.5;
            if (capteurs[i].valeur_mesuree < 0.3)
                ecart = 0.3 - capteurs[i].valeur_mesuree;
            somme += ecart;
            max += 0.3;
        }
    }
    return 100 - ((somme / max)*100); //Formule du SHI
}

// Calcul du score de charge avec le pourcentage d'utilisation
float calculer_score_charge(Capteur capteurs[], int n){
    float somme = 0; // la somme des écarts
    float max = 0; // l'écart maximale possible

    //La boucle traverse le tableau et ne prend en compte que les capteurs de charge
    for (int i=0; i < n; i++){
        float utilisation = 0;
        if (capteurs[i].type == CHARGE){
                //pourcentage d'utilisation
            utilisation = (capteurs[i].valeur_mesuree/capteurs[i].valeur_nominale)*100;
            somme += utilisation;
            max += 2100;
        }
    }
    if (max == 0)
        return 0;
    return 100 - ((somme / max)*100); //Formule du SHI
}
void calculer_indice_sante(Capteur capteurs[], int n, Alerte alertes[], int nb_alr,
IndiceHealthStructural *sante){
   sante->score_deformation = calculer_score_deformation(capteurs, n);
   sante->score_vibration = calculer_score_vibration(capteurs, n);
   sante->score_charge = calculer_score_charge(capteurs, n);
   //calcul indice SHI global
   sante->indice_global = (sante->score_deformation*0.4 + sante->score_vibration*0.35 + sante->score_charge*0.25);

   //Affichage complet
    printf("RESUME DE SANTE STRUCTURALE\n");
    printf("Indice global(SHI) \t\t:%f/ 100\n", sante->indice_global);
    printf("\t Score deformation \t: %f/ 100\n", sante->score_deformation);
    printf("\t Score vibration \t: %f/ 100\n", sante->score_vibration);
    printf("\t Score charge \t: %f/ 100\n", sante->score_charge);

//Affichage de l'Etat et de la reco selon la valeur du SHI
    if (sante->indice_global == 100){
        printf("Etat général \t\t: EXCELLENT\n");
        printf("Recommandation \t\t: Aucune alerte\n");
    }
    else if (sante->indice_global >= 90 && sante->indice_global <= 99){
        printf("Etat général \t\t: BONNE SANTE\n");
        printf("Recommandation \t\t: Inspections annuelles requises");
    }
    else if (sante->indice_global >= 80 && sante->indice_global <= 89){
        printf("Etat général \t\t: ALERTE JAUNE \n");
        printf("Recommandation \t\t: Inspections trimestrielles requises\n");
    }
    else if (sante->indice_global >= 70 && sante->indice_global <= 79){
        printf("Etat général \t\t: ALERTE ORANGE \n");
        printf("Recommandation \t\t: Inspections mensuelles requises\n");
    }
    else if (sante->indice_global < 70){
        printf("Etat général \t\t: ALERTE ROUGE\n");
        printf("Recommandation \t\t: Inspection d'urgence requise\n");
    }

};
