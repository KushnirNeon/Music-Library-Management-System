#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Entities.c"
#include "Common.c"
#include "Artist.c"
#include "Song.c"

int main()
{
    struct Artist artist;
    struct Song song;

    while (1)
    {
        int choice;
        int id;
        char error[100];

        printf("Available database operations:\n1. Get artist\n2. Get song\n3. Delete artist\n4. Delete song\n5. Update artist\n6. Update song\n7. Insert artist\n8. Insert song\n9. Calculate artists\n10. Calculate songs\n11. Calculate songs of every artist\n12. Utility artist\n13. Utility song\n14. Exit\n");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            printf("Enter artist's ID: ");
            scanf("%d", &id);
            getArtist((struct Artist*)&artist, id, error) ? outputArtist(artist) : printf("Error: %s\n", error);
            break;

        case 2:
            printf("Enter artist's ID: ");
            scanf("%d", &id);
            if (getArtist(&artist, id, error))
            {
                printf("Enter song's ID: ");
                scanf("%d", &id);
                getSong(artist, &song, id, error) ? outputSong(song, artist) : printf("Error: %s\n", error);
            }
            else
            {
                printf("Error: %s\n", error);
            }
            break;

        case 3:
            printf("Enter artist's ID: ");
            scanf("%d", &id);
            deleteArtist(id, error) ? printf("Artist was successfully deleted\n") : printf("Error: %s\n", error);
            break;

        case 4:
            printf("Enter artist's ID: ");
            scanf("%d", &id);
            if (getArtist(&artist, id, error))
            {
                printf("Enter song's ID: ");
                scanf("%d", &id);
                if (getSong(artist, &song, id, error))
                {
                    deleteSong(artist, song, id, error);
                    printf("Song was successfully deleted\n");
                }
                else
                {
                    printf("Error: %s\n", error);
                }
            }
            else
            {
                printf("Error: %s\n", error);
            }
            break;

        case 5:
            printf("Enter artist's ID: ");
            scanf("%d", &id);
            artist.artistId = id;
            getArtist(&artist, id, error);
            inputArtist(&artist);
            updateArtist(artist, error) ? printf("Artist was successfully updated\n") : printf("Error: %s\n", error);
            break;

        case 6:
            printf("Enter artist's ID: ");
            scanf("%d", &id);
            if (getArtist(&artist, id, error))
            {
                printf("Enter song's ID: ");
                scanf("%d", &id);
                if (getSong(artist, &song, id, error))
                {
                    inputSong(&song);
                    updateSong(song, id);
                    printf("Song was successfully updated\n");
                }
                else
                {
                    printf("Error: %s\n", error);
                }
            }
            else
            {
                printf("Error: %s\n", error);
            }
            break;

        case 7:
            artist.songsCount = 0;
            artist.exists = 1;
            inputArtist(&artist);
            insertArtist(artist);
            break;

        case 8:
            printf("Enter artist's ID: ");
            scanf("%d", &id);
            if (getArtist(&artist, id, error))
            {
                song.exists = 1;
                song.artistId = artist.artistId;
                inputSong(&song);
                insertSong(artist, song, error);
            }
            else
            {
                printf("Error: %s\n", error);
            }
            break;

        case 9:
            calculate(9);
            break;

        case 10:
            calculate(10);
            break;

        case 11:
            calculate(11);
            break;

        case 12:
            ut_m();
            break;

        case 13:
            ut_s();
            break;

        case 14:
            return 0;

        default:
            printf("Input isn't valid; please select an option between 1 and 14\n");
        }
        printf("-----------------------------------------------------------------------\n");
    }

    return 0;
}
