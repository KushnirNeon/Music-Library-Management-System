#pragma once

struct Artist
{
	int artistId;
	char name[50];
	int age;
	char country[30];
	int exists;
	long firstSongAddress;
	int songsCount;
};

struct Song
{
	int songId;
	int artistId;
	char title[100];
	char album[50];
	char genre[30];
	int exists;
	long selfAddress;
	long nextAddress;
};

struct Indexer
{
	int artistId;
	int address;
	int exists;
};