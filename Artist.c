#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Entities.c"
#include "Common.c"

//getArtist
int getArtist(struct Artist* artist, int id, char* error)
{
	FILE* indexTable = fopen("artist.ind", "rb");
	FILE* artistsDatabase = fopen("artist.fl", "rb");
	struct Indexer indexer;

	if (!checkIndexExists(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * sizeof(struct Indexer), SEEK_SET);
	fread(&indexer, sizeof(struct Indexer), 1, indexTable);

	if (!checkEntityExists(indexer, error))
	{
		return 0;
	}

	fseek(artistsDatabase, indexer.address, SEEK_SET);
	fread(artist, sizeof(struct Artist), 1, artistsDatabase);

	fclose(indexTable);
	fclose(artistsDatabase);

	return 1;
}

//--------------- insertArtist ---------------
int findMinAvailableArtistIndex() 
{
	FILE* artistsGarbage = fopen("deleted_artists.txt", "r+");
	int garbageAmount;
	
	fscanf(artistsGarbage, "%d", &garbageAmount);
	
	int* deletedIds = (int*)malloc(garbageAmount * sizeof(int));
	for (int i = 0; i < garbageAmount; i++)
	{
		fscanf(artistsGarbage, "%d", &deletedIds[i]);
	}

	int minId = deletedIds[0];
	for (int i = 1; i < garbageAmount; i++)
	{
		if (deletedIds[i] < minId) 
		{
			minId = deletedIds[i];
		}
	}

	fclose(artistsGarbage);
	artistsGarbage = fopen("deleted_artists.txt", "w");
	
	rewind(artistsGarbage);
	fprintf(artistsGarbage, "%d", garbageAmount - 1);
	for (int i = 0; i < garbageAmount; i++)
	{
		if (deletedIds[i] != minId) 
		{
			fprintf(artistsGarbage, " %d", deletedIds[i]);
		}
	}

	free(deletedIds);
	fclose(artistsGarbage);
	
	return minId;
}
void inputArtist(struct Artist* artist)
{
	char name[50];
	int age;
	char country[30];

	name[0] = '\0';
	printf("Enter artist's name: ");
	scanf("%s", name);
	strcpy(artist->name, name);

	printf("Enter artist's age: ");
	scanf("%d", &age);
	artist->age = age;

	country[0] = '\0';
	printf("Enter artist's country: ");
	scanf("%s", country);
	strcpy(artist->country, country);
}
int insertArtist(struct Artist artist)
{
	FILE* indexTable = fopen("artist.ind", "a+b");
	FILE* artistsDatabase = fopen("artist.fl", "a+b");
	FILE* artistsGarbage = fopen("deleted_artists.txt", "a+b");
	struct Indexer indexer;
	int garbageAmount = 0;

	fscanf(artistsGarbage, "%d", &garbageAmount);
	fclose(artistsGarbage);

	if (garbageAmount > 0)
	{
		int minAvailableIndex = findMinAvailableArtistIndex();
		artist.artistId = minAvailableIndex;
		fclose(indexTable);
		fclose(artistsDatabase);
		indexTable = fopen("artist.ind", "r+b");
		artistsDatabase = fopen("artist.fl", "r+b");
		fseek(indexTable, (artist.artistId - 1) * sizeof(struct Indexer), SEEK_SET);
		fread(&indexer, sizeof(struct Indexer), 1, indexTable);
		fseek(artistsDatabase, indexer.address, SEEK_SET);
	}
	else
	{
		fseek(indexTable, 0, SEEK_END);

		if (ftell(indexTable))
		{
			fseek(indexTable, -((long)sizeof(struct Indexer)), SEEK_END);
			fread(&indexer, sizeof(struct Indexer), 1, indexTable);
			artist.artistId = indexer.artistId + 1;
		}
		else
		{
			artist.artistId = 1;
		}
	}

	artist.firstSongAddress = -1;
	artist.songsCount = 0;
	fwrite(&artist, sizeof(struct Artist), 1, artistsDatabase);

	indexer.artistId = artist.artistId;
	indexer.address = (artist.artistId - 1) * sizeof(struct Artist);
	indexer.exists = 1;
	printf("Artist's id: %d\n", artist.artistId);

	fseek(indexTable, (artist.artistId - 1) * sizeof(struct Indexer), SEEK_SET);
	fwrite(&indexer, sizeof(struct Indexer), 1, indexTable);

	fclose(indexTable);
	fclose(artistsDatabase);

	return 1;
}
//--------------- insertArtist ---------------

//ut_m
void ut_m()
{
	FILE* artistsDatabase = fopen("artist.fl", "rb");
	fseek(artistsDatabase, 0, SEEK_SET);

	struct Artist artist;

	while (fread(&artist, sizeof(struct Artist), 1, artistsDatabase) == 1)
	{
		if (artist.exists != 0)
		{
			printf("Id: %d\n", artist.artistId);
			printf("Name: %s\n", artist.name);
			printf("Age: %d\n", artist.age);
			printf("Country: %s\n", artist.country);
			printf("First song address: %ld\n", artist.firstSongAddress);
			printf("Songs count: %d\n", artist.songsCount);
			printf("\n");
		}
	}
}
