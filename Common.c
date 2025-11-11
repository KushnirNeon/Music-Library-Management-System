#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Entities.c"

//--------------- checks ---------------
int checkIndexExists(FILE* indexTable, char* error, int id)
{
	fseek(indexTable, 0, SEEK_END);

	long indexTableSize = ftell(indexTable);

	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize)
	{
		strcpy(error, "ID doesn't exist");
		return 0;
	}

	return 1;
}
int checkEntityExists(struct Indexer indexer, char* error)
{
	if (!indexer.exists)
	{
		strcpy(error, "Entity has been deleted");
		return 0;
	}

	return 1;
}
//--------------- checks ---------------

//--------------- deleteSong ---------------
void relinkAddresses(FILE* database, struct Song previous, struct Song song, struct Artist* artist)
{
	if (song.selfAddress == artist->firstSongAddress)
	{
		if (song.selfAddress == song.nextAddress)
		{
			artist->firstSongAddress = -1;
		}
		else
		{
			artist->firstSongAddress = song.nextAddress;
		}
	}
	else
	{
		if (song.selfAddress == song.nextAddress)
		{
			previous.nextAddress = previous.selfAddress;
		}
		else
		{
			previous.nextAddress = song.nextAddress;
		}

		fseek(database, previous.selfAddress, SEEK_SET);
		fwrite(&previous, sizeof(struct Song), 1, database);
	}
}
void markDeletedSong(int id)
{
	FILE* songsGarbage = fopen("deleted_songs.txt", "rb");
	int garbageAmount;
	fscanf(songsGarbage, "%d", &garbageAmount);
	int* deletedIds = (int*)malloc(garbageAmount * sizeof(int));

	for (int i = 0; i < garbageAmount; i++)
	{
		fscanf(songsGarbage, "%d", deletedIds + i);
	}

	fclose(songsGarbage);
	songsGarbage = fopen("deleted_songs.txt", "wb");
	fprintf(songsGarbage, "%d", garbageAmount + 1);

	for (int i = 0; i < garbageAmount; i++)
	{
		fprintf(songsGarbage, " %d", deletedIds[i]);
	}

	fprintf(songsGarbage, " %d", id);
	free(deletedIds);
	fclose(songsGarbage);
}
void deleteSong(struct Artist artist, struct Song song, int songId, char* error)
{
	FILE* songsDatabase = fopen("song.fl", "r+b");
	struct Song previous;

	fseek(songsDatabase, artist.firstSongAddress, SEEK_SET);

	do
	{
		fread(&previous, sizeof(struct Song), 1, songsDatabase);
		fseek(songsDatabase, previous.nextAddress, SEEK_SET);
	} while (previous.nextAddress != song.selfAddress && song.selfAddress != artist.firstSongAddress);

	relinkAddresses(songsDatabase, previous, song, &artist);
	markDeletedSong(song.songId);

	song.exists = 0;

	fseek(songsDatabase, song.selfAddress, SEEK_SET);
	fwrite(&song, sizeof(struct Song), 1, songsDatabase);
	fclose(songsDatabase);

	artist.songsCount--;
	updateArtist(artist, error);
}
//--------------- deleteSong ---------------

//--------------- deleteArtist ---------------
void markDeletedArtist(int id)
{
	FILE* artistsGarbage = fopen("deleted_artists.txt", "rb");
	int garbageAmount;

	fscanf(artistsGarbage, "%d", &garbageAmount);
	int* deleteIds = (int*)malloc(garbageAmount * sizeof(int));

	for (int i = 0; i < garbageAmount; i++)
	{
		fscanf(artistsGarbage, "%d", deleteIds + i);
	}

	fclose(artistsGarbage);
	artistsGarbage = fopen("deleted_artists.txt", "wb");
	fprintf(artistsGarbage, "%d", garbageAmount + 1);

	for (int i = 0; i < garbageAmount; i++)
	{
		fprintf(artistsGarbage, " %d", deleteIds[i]);
	}

	fprintf(artistsGarbage, " %d", id);
	free(deleteIds);
	fclose(artistsGarbage);
}
int deleteArtist(int id, char* error)
{
	FILE* indexTable = fopen("artist.ind", "r+b");
	struct Artist artist;
	struct Indexer indexer;

	if (!checkIndexExists(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * sizeof(struct Indexer), SEEK_SET);
	fread(&indexer, sizeof(struct Indexer), 1, indexTable);

	if (indexer.exists == 0)
	{
		fclose(indexTable);
		return 0;
	}

	FILE* artistDatabase = fopen("artist.fl", "r+b");
	fseek(artistDatabase, indexer.address, SEEK_SET);
	fread(&artist, sizeof(struct Artist), 1, artistDatabase);

	artist.exists = 0;
	markDeletedArtist(artist.artistId);

	fseek(artistDatabase, indexer.address, SEEK_SET);
	fwrite(&artist, sizeof(struct Artist), 1, artistDatabase);

	fclose(artistDatabase);

	indexer.exists = 0;

	fseek(indexTable, (id - 1) * sizeof(struct Indexer), SEEK_SET);
	fwrite(&indexer, sizeof(struct Indexer), 1, indexTable);

	fclose(indexTable);

	FILE* songDatabase = fopen("song.fl", "r+b");
	struct Song song;
	fseek(songDatabase, artist.firstSongAddress, SEEK_SET);

	for (int i = 0; i < artist.songsCount; i++)
	{
		fread(&song, sizeof(struct Song), 1, songDatabase);
		deleteSong(artist, song, song.songId, error);

		fseek(songDatabase, song.nextAddress, SEEK_SET);
	}

	fclose(songDatabase);

	return 1;
}
//--------------- deleteArtist ---------------

//updateArtist
int updateArtist(struct Artist artist, char* error)
{
	FILE* indexTable = fopen("artist.ind", "r+b");
	FILE* artistsDatabase = fopen("artist.fl", "r+b");
	struct Indexer indexer;
	int id = artist.artistId;

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
	fwrite(&artist, sizeof(struct Artist), 1, artistsDatabase);

	fclose(indexTable);
	fclose(artistsDatabase);

	return 1;
}

//--------------- insertSong ---------------
int findMinAvailableSongIndex()
{
	FILE* songsGarbage = fopen("deleted_songs.txt", "r+");
	int garbageAmount;

	fscanf(songsGarbage, "%d", &garbageAmount);

	int* deletedIds = (int*)malloc(garbageAmount * sizeof(int));
	for (int i = 0; i < garbageAmount; i++)
	{
		fscanf(songsGarbage, "%d", &deletedIds[i]);
	}

	int minId = deletedIds[0];
	for (int i = 1; i < garbageAmount; i++)
	{
		if (deletedIds[i] < minId)
		{
			minId = deletedIds[i];
		}
	}

	fclose(songsGarbage);
	songsGarbage = fopen("deleted_songs.txt", "w");

	rewind(songsGarbage);
	fprintf(songsGarbage, "%d", garbageAmount - 1);
	for (int i = 0; i < garbageAmount; i++)
	{
		if (deletedIds[i] != minId)
		{
			fprintf(songsGarbage, " %d", deletedIds[i]);
		}
	}

	free(deletedIds);
	fclose(songsGarbage);

	return minId;
}
int insertSong(struct Artist artist, struct Song song, char* error)
{
	FILE* songsDatabase = fopen("song.fl", "a+b");
	FILE* songsGarbage = fopen("deleted_songs.txt", "a+b");
	int garbageAmount = 0;

	fscanf(songsGarbage, "%d", &garbageAmount);
	fclose(songsGarbage);

	if (garbageAmount > 0)
	{
		fclose(songsDatabase);
		songsDatabase = fopen("song.fl", "r+b");
		int minAvailableId = findMinAvailableSongIndex();
		song.songId = minAvailableId;
		song.selfAddress = (minAvailableId - 1) * sizeof(struct Song);
		song.nextAddress = song.selfAddress;
		song.artistId = artist.artistId;
		song.exists = 1;
		fseek(songsDatabase, song.selfAddress, SEEK_SET);
		fwrite(&song, sizeof(struct Song), 1, songsDatabase);
		fclose(songsDatabase);
	}
	else
	{
		fclose(songsDatabase);
		songsDatabase = fopen("song.fl", "r+b");
		song.exists = 1;
		fseek(songsDatabase, 0, SEEK_END);
		int dbSize = ftell(songsDatabase);
		song.selfAddress = dbSize;
		song.nextAddress = dbSize;
		song.songId = (dbSize / sizeof(struct Song)) + 1;
		fwrite(&song, sizeof(struct Song), 1, songsDatabase);
		fclose(songsDatabase);
	}

	songsDatabase = fopen("song.fl", "r+b");
	artist.songsCount++;
	int min = -1;

	if (artist.songsCount == 1) 
	{
		artist.firstSongAddress = song.selfAddress;
		updateArtist(artist, error);
		printf("Song's id: %d\n", song.songId);
		return 1;
	}
	else
	{
		fseek(songsDatabase, 0, SEEK_END);
		int dbSize = ftell(songsDatabase);

		fseek(songsDatabase, 0, SEEK_SET);
		struct Song current_song1;
		int min = -1;
		int n = dbSize / sizeof(struct Song);

		for (int i = 0; i < n; i++) 
		{
			fread(&current_song1, sizeof(struct Song), 1, songsDatabase);

			if (current_song1.exists && (min == -1 || current_song1.songId < min) && current_song1.artistId == artist.artistId)
			{
				min = current_song1.songId;
				artist.firstSongAddress = current_song1.selfAddress;
			}
		}

		updateArtist(artist, error);

		fseek(songsDatabase, artist.firstSongAddress, SEEK_SET);
		struct Song current_song, next_song;

		fread(&current_song, sizeof(struct Song), 1, songsDatabase);

		for (int i = 0; i < n; i++)
		{
			fread(&next_song, sizeof(struct Song), 1, songsDatabase);

			if (next_song.exists && current_song.artistId == next_song.artistId) 
			{
				current_song.nextAddress = next_song.selfAddress;
				fseek(songsDatabase, current_song.selfAddress, SEEK_SET);
				fwrite(&current_song, sizeof(struct Song), 1, songsDatabase);
				fseek(songsDatabase, next_song.selfAddress, SEEK_SET);
				fread(&current_song, sizeof(struct Song), 1, songsDatabase);
			}
		}

		printf("Song's id: %d\n", song.songId);
	}

	fclose(songsDatabase);
	return 1;
}
//--------------- insertSong ---------------

//--------------- calculate ---------------
void outputArtist(struct Artist artist)
{
	printf("Name: %s\n", artist.name);
	printf("Age: %d\n", artist.age);
	printf("Country: %s\n", artist.country);
	printf("Songs count: %d\n\n", artist.songsCount);
}
void iterateOverArtists(int ids, struct Artist* artist, char* error, int* artistsCount, int* songsCount)
{
	for (int i = 1; i <= ids; i++)
	{
		if (getArtist(artist, i, error))
		{
			(*artistsCount)++;
			(*songsCount) += (*artist).songsCount;
		}
	}
}
void iterateOverArtistsPrint(int ids, struct Artist* artist, char* error, int* artistsCount, int* songsCount)
{
	for (int i = 1; i <= ids; i++)
	{
		if (getArtist(artist, i, error))
		{
			(*artistsCount)++;
			(*songsCount) += (*artist).songsCount;
			printf("Artist with id %d: %d songs\n", i, (*artist).songsCount);
		}
	}
}
void calculate(int n)
{
	FILE* indexTable = fopen("artist.ind", "rb");
	int artistsCount = 0;
	int songsCount = 0;
	struct Artist artist;
	char error[100];

	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);

	iterateOverArtists(indAmount, &artist, error, &artistsCount, &songsCount);

	fclose(indexTable);

	if (n == 9) printf("All artists: %d\n", artistsCount);
	if (n == 10) printf("All songs: %d\n", songsCount);
	if (n == 11) iterateOverArtistsPrint(indAmount, &artist, error, &artistsCount, &songsCount);
}
//--------------- calculate ---------------