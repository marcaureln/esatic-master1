#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <mysql/mysql.h>
#include "../include/parking.h"
#include "../include/vehicule.h"


Parking creerParking(int nbPlacesMoto, int nbPlacesVoiture, int nbPlacesCamion)
{
    Parking parking = {nbPlacesMoto, nbPlacesVoiture, nbPlacesCamion, "vehicule"};
    return parking;
}

void afficherCapaciteParking(Parking *parking)
{
    // À faire : Récupérer les véhicules présent dans le parking via la base de données
    printf("Capacité du parking :\n");
    printf("Nombre de places pour moto : 0/%d\n", parking->nbPlacesMoto);
    printf("Nombre de places pour voiture : 0/%d\n", parking->nbPlacesVoiture);
    printf("Nombre de places pour camion : 0/%d\n", parking->nbPlacesCamion);
}

void afficherTarifs()
{
    printf("Tarifs :\n");
    printf("Moto : 200 FCFA/h\n");
    printf("Voiture : 500 FCFA/h\n");
    printf("Camion : 1000 FCFA/h\n");
}

void stationnerVehicule(Vehicule *vehicule)
{
    // À faire : Vérifier les places disponibles

    printf("Stationnement du véhicule immatriculé %s...\n", vehicule->immatriculation);

    MYSQL *con = mysql_init(NULL);

    // S'assurer que la fonction mysql_init s'est bien exécutée
    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        return;
    }

    // conection à la base de données
    if (!mysql_real_conect(con, "db", "root", "example", "parking", 0, NULL, 0))
    {
        printf("Impossible de se conecter à la base de données. Erreur: %s\n", mysql_error(con));
        return;
    }


// verification des places disponibles avant insertion 
    if(is_space_available(vehicule->type)){
        char requete[100];

        sprintf(requete, "insert into `vehicules` (`type`, `immatriculation`,`parked_hours`, `parked_minutes`) values ('%d', '%s','%d','%d')",  vehicule->type, vehicule->immatriculation, vehicule->parked_hours, vehicule->parked_minutes);

        if (mysql_query(con, requete))
        {
            printf("Impossible d'exécuter cette requête. Erreur: %s\n", mysql_error(con));
            return;
        }
        

        mysql_close(con);

        printf("Véhicule stationné avec succès !");

    }
    else {
        double taux_camion = 0.0 ;
        taux_camion = occupancy_rate_by_type(3);
        char requete[100];
        if(vehicule->type == 2 && taux_camion < 0.80) {
            sprintf(requete, "insert into `vehicules` (`type`, `immatriculation`,`parked_hours`, `parked_minutes`) values ('%d', '%s','%d','%d')",  3, vehicule->immatriculation, vehicule->parked_hours, vehicule->parked_minutes);
            if (mysql_query(con, requete))
                {
                    printf("Impossible d'exécuter cette requête. Erreur: %s\n", mysql_error(con));
                    return;
                }

            mysql_close(con);

            printf("Véhicule stationné avec succès !");
            }
        else {
            printf("il y a aucune places disponibles dans la categorie souhaiter ");
        }
    }
    // Insertion d'un véhicule dans la base de données
    


    
}

int is_space_available(int type) {
    

    MYSQL *con = mysql_init(NULL);

    // S'assurer que la fonction mysql_init s'est bien exécutée
    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        return;
    }

    // conection à la base de données
    if (!mysql_real_conect(con, "db", "root", "example", "parking", 0, NULL, 0))
    {
        printf("Impossible de se conecter à la base de données. Erreur: %s\n", mysql_error(con));
        return;
    }

    int max_count = 0, parked_count = 0;
    char query[100];
    switch(type) {
        case 1:
            max_count = 100;
            sprintf(query, "SELECT COUNT(*) FROM vehicules WHERE type=%d", 1);
            mysql_query(con, query);
            break;
        case 2:
            max_count = 100;
            sprintf(query, "SELECT COUNT(*) FROM vehicules WHERE type=%d", 2);
            mysql_query(con, query);
            break;
        case 3:
            max_count = 100;
            sprintf(query, "SELECT COUNT(*) FROM vehicules WHERE type=%d", 3);
            mysql_query(con, query);
            break;
        default:
            return 0;
    }
    MYSQL_RES *res = mysql_use_result(con);
    MYSQL_ROW row = mysql_fetch_row(res);
    parked_count = atoi(row[0]);
    mysql_free_result(res);
    if (parked_count < max_count) {
        return 1;
    }
    else {
        double taux_camion = 0.0; 
        taux_camion = occupancy_rate_by_type(3);
        if (type == 2 && taux_camion < 0.80){
            return 1;
        } 
        else{
          return 0;  
        }
        
    }
    mysql_close(con);
}




double occupancy_rate_by_type(int vehicule_type) {

    MYSQL *con = mysql_init(NULL);

    // S'assurer que la fonction mysql_init s'est bien exécutée
    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        return;
    }

    // conection à la base de données
    if (!mysql_real_conect(con, "db", "root", "example", "parking", 0, NULL, 0))
    {
        printf("Impossible de se conecter à la base de données. Erreur: %s\n", mysql_error(con));
        return;
    }

    char query[100];
    sprintf(query, "SELECT COUNT(*) FROM vehicules WHERE type=%d", vehicule_type);
    mysql_query(con, query);
    MYSQL_RES *result = mysql_store_result(con);
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = atoi(row[0]);
    mysql_free_result(result);

    mysql_close(con);
    return (double) count / MAX_CAPACITY[vehicule_type];
}

Vehicle* get_vehicle_data(MYSQL_ROW row) {
    Vehicle* vehicle = malloc(sizeof(Vehicle));
    if (vehicle == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for vehicle data.\n");
        return NULL;
    }
    vehicule->id = atoi(row[0]);
    vehicule->type = atoi(row[1]);
    vehicule->immatriculation = row[2];
    vehicule->parked_hours = atoi(row[3]);
    vehicule->parked_minutes = atoi(row[4]);
    
    return vehicule;
}


int calculate_parking_cost(char immatriculation[9]) {
    MYSQL *con = mysql_init(NULL);

    // S'assurer que la fonction mysql_init s'est bien exécutée
    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        return;
    }

    // conection à la base de données
    if (!mysql_real_conect(con, "db", "root", "example", "parking", 0, NULL, 0))
    {
        printf("Impossible de se conecter à la base de données. Erreur: %s\n", mysql_error(con));
        return;
    }
    char requete[100];

    sprintf(requete, "SELECT * FROM my_table WHERE immatriculation=%d", immatriculation);

    if (mysql_query(con, requete))
        {
            printf("Impossible d'exécuter cette requête. Erreur: %s\n", mysql_error(con));
            return;
        }
     MYSQL_RES* result_set = mysql_store_result(con);
    if (result_set == NULL) {
        fprintf(stderr, "Error: Failed to retrieve data from database. %s\n", mysql_error(con));
       mysql_close(con);
        return 1;
    }

    MYSQL_ROW row;
    Vehicule* vehicule = get_vehicule_data(row);
 

    int cost = 0;
    switch(v->type) {
        case 2:
            cost = 500;
            break;
        case 1:
            cost = 200;
            break;
        case 3:
            cost = 1000;
            break;
    }
    int total_parked_minutes = v->parked_hours * 60 + v->parked_minutes;
    int total_parked_hours = total_parked_minutes / 60;
    if (total_parked_minutes % 60 > 0) {
        total_parked_hours++;
    }
    cost *= total_parked_hours;
    return cost;
}

void print_invoice(Vehicule *v, int cost) {
    printf("Invoice for vehicle ID %d:\n", v->id);
    printf("Type: ");
    switch(v->type) {
        case 2:
            printf("Vehicle\n");
            break;
        case 1:
            printf("Moto\n");
            break;
        case 3:
            printf("Camion\n");
            break;
    }
    printf("Parked for: %d hours and %d minutes\n", v->parked_hours, v->parked_minutes);
    printf("Total cost: %d francs\n", cost);
}




void libererPlace(Parking *parking)
{
    // À faire : Afficher tout les véhicules présents et selectionner par son ID le véhicule à rétiter
    // À faire : Rétirer le véhicule de la base données
    // À faire : Calculer le temps total passé dans le parking et retourner la facture
}