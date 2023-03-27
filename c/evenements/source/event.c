#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "../include/utils.h"
#include "../include/event.h"
#include "../include/database.h"

Evenement creerEvenementNull()
{
    Evenement evenement = {0, "", 0, 0};
    return evenement;
}

Evenement creerEvenement(BD *bd)
{
    Evenement evenement;
    Evenement *p_evenement = &evenement;
    printf("Nom de l'évènement : ");
    lire(p_evenement->titre, 256);
    printf("Type d'évènement : \n");
    afficherTypeEvenement(bd);
    printf("> ");
    char saisieType[2];
    lire(saisieType, sizeof(saisieType));
    p_evenement->typeId = atoi(saisieType);
    printf("Nombre de places : ");
    char saisiePlaces[sizeof(int)];
    lire(saisiePlaces, sizeof(saisiePlaces));
    p_evenement->nbPlaces = atoi(saisiePlaces);

    printf("\nEnregistrement de l'évènement en cours...\n");

    MYSQL *con = connectionBD(bd->hote, bd->utilisateur, bd->mdp, bd->nom);

    if (con == NULL)
    {
        printf("Erreur lors de la création de l'évènement !\n");
        return creerEvenementNull();
    }

    char requete[384];

    // Insertion de l'évènement dans la base de données
    if (0 > snprintf(requete, sizeof(requete), "INSERT INTO evenements.evenement(`titre`, `nbPlaces`, `typeId`) VALUES (\"%s\", %d, %d)", p_evenement->titre, p_evenement->nbPlaces, p_evenement->typeId))
    {
        printf("Erreur lors du formatage de la requête !\n");
    }
    else
    {
        if (mysql_query(con, requete))
        {
            printf("Erreur lors de l'insertion dans la base de données !\n");
            erreurBD(con);
        }
        else
        {
            printf("L'évènement a bien été créé !\n");

            // Insertion du menu de l'évènement dans la base de données
            if (0 < snprintf(requete, sizeof(requete), "INSERT INTO evenements.menu(`titre`, `evenementId`) VALUES (\"Menu %s\", %d)", p_evenement->titre, mysql_insert_id(con)))
            {
                if (mysql_query(con, requete))
                {
                    printf("Erreur lors de la création du menu !\n");
                    erreurBD(con);
                }
            }
        }
    }

    mysql_close(con);

    return evenement;
}

Evenement recupererEvenement(BD *bd, int id)
{
    Evenement evenement;
    Evenement *p_evenement = &evenement;

    char requete[128];

    if (0 > snprintf(requete, sizeof(requete), "SELECT titre, nbPlaces, typeId FROM evenements.evenement WHERE id = %d", id))
    {
        printf("Erreur lors du formatage de la requête !\n");
        return creerEvenementNull();
    }

    MYSQL *con = connectionBD(bd->hote, bd->utilisateur, bd->mdp, bd->nom);

    if (con == NULL)
    {
        printf("Erreur lors de la connection à la base de données !\n");
        return creerEvenementNull();
    }

    if (mysql_query(con, requete))
    {
        printf("Erreur lors de la récupération de l'évènement !\n");
        erreurBD(con);
        return creerEvenementNull();
    }
    else
    {
        MYSQL_RES *result = mysql_store_result(con);

        if (result == NULL)
        {
            printf("Aucun résultat pour l'évènement demandé !\n");
        }
        else
        {
            MYSQL_ROW row = mysql_fetch_row(result);

            if (row)
            {
                p_evenement->id = id;
                strcpy(p_evenement->titre, row[0]);
                p_evenement->nbPlaces = atoi(row[1]);
                p_evenement->typeId = atoi(row[2]);
            }
        }

        mysql_free_result(result);
    }

    mysql_close(con);

    return evenement;
}

// Affiche la liste des évènements enregistrés dans la base de données et retourne le nombre total d'évènements
int afficherEvenements(BD *bd)
{
    MYSQL *con = connectionBD(bd->hote, bd->utilisateur, bd->mdp, bd->nom);

    // S'assurer que la connection à la base de données s'est bien exécutée
    if (con == NULL)
    {
        return 0;
    }

    if (mysql_query(con, "SELECT evenements.evenement.id, titre, libelle FROM evenements.evenement, evenements.typeEvenement WHERE evenements.evenement.typeId = evenements.typeEvenement.id"))
    {
        erreurBD(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
    {
        erreurBD(con);
    }

    int count = 0;

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        count++;
        printf("%s. %s (%s)\n", row[0], row[1], row[2]);
    }

    mysql_free_result(result);
    mysql_close(con);

    return count;
}

void afficherTypeEvenement(BD *bd)
{
    MYSQL *con = connectionBD(bd->hote, bd->utilisateur, bd->mdp, bd->nom);

    // S'assurer qu'on a bien une connection à la base de données
    if (con == NULL)
    {
        return;
    }

    if (mysql_query(con, "SELECT id, libelle FROM evenements.typeEvenement"))
    {
        erreurBD(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
    {
        erreurBD(con);
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        printf("%s. %s\n", row[0], row[1]);
    }

    mysql_free_result(result);
    mysql_close(con);
}