#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Entities.c"
#include "Common.c"

//getSong
int getSong(struct Artist artist, struct Song* song, int songId, char* error)
{
	if (!artist.songsCount)
	{
		strcpy(error, "Artist doesn't have any songs");
		return 0;
	}

	FILE* songsDatabase = fopen("song.fl", "rb");
	fseek(songsDatabase, artist.firstSongAddress, SEEK_SET);
	fread(song, sizeof(struct Song), 1, songsDatabase);

	for (int i = 0; i < artist.songsCount; i++)
	{
		if (song->songId == songId)
		{
			fclose(songsDatabase);
			return 1;
		}

		fseek(songsDatabase, song->nextAddress, SEEK_SET);
		fread(song, sizeof(struct Song), 1, songsDatabase);
	}

	strcpy(error, "Song doesn't exist");
	fclose(songsDatabase);

	return 0;
}

//outputSong
void outputSong(struct Song song, struct Artist artist)
{
	printf("Artist's name: %s, age: %d, country: %s\n", artist.name, artist.age, artist.country);
	printf("Title: %s\n", song.title);
	printf("Album: %s\n", song.album);
	printf("Genre: %s\n", song.genre);
}

//inputSong
void inputSong(struct Song* song)
{
	char title[100];
	char album[50];
	char genre[30];

	title[0] = '\0';
	printf("Enter song's title: ");
	scanf("%s", title);
	strcpy(song->title, title);

	album[0] = '\0';
	printf("Enter song's album: ");
	scanf("%s", album);
	strcpy(song->album, album);

	genre[0] = '\0';
	printf("Enter song's genre: ");
	scanf("%s", genre);
	strcpy(song->genre, genre);
}

//updateSong
int updateSong(struct Song song, int songId)
{
	FILE* songsDatabase = fopen("song.fl", "r+b");
	song.exists = 1;
	fseek(songsDatabase, song.selfAddress, SEEK_SET);
	fwrite(&song, sizeof(struct Song), 1, songsDatabase);
	fclose(songsDatabase);

	return 1;
}

//ut_s
void ut_s()
{
	FILE* songsDatabase = fopen("song.fl", "rb");
	fseek(songsDatabase, 0, SEEK_SET);

	struct Song song;

	while (fread(&song, sizeof(struct Song), 1, songsDatabase) == 1)
	{
		if (song.exists != 0)
		{
			printf("Id: %d\n", song.songId);
			printf("Artist's id: %d\n", song.artistId);
			printf("Title: %s\n", song.title);
			printf("Album: %s\n", song.album);
			printf("Genre: %s\n", song.genre);
			printf("Self Address: %ld\n", song.selfAddress);
			printf("Next Address: %ld\n", song.nextAddress);
			printf("\n");
		}
	}
}
