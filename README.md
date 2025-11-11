# Music Library Management System (C)

This is a simple music library management system written in C. The system allows you to manage **artists** and their **songs** using file-based storage with basic indexing. It supports CRUD operations, song linking, and utility functions for debugging and calculating statistics.

---

## 🗂 Project Structure

- **Entities.c** – Contains struct definitions for:
  - `Artist`
  - `Song`
  - `Indexer`
- **Common.c** – Utility functions for checking indices and entity existence.
- **Artist.c** – Functions to manage artists:
  - Get, insert, delete, and update artists.
  - Handle linked song addresses and garbage IDs.
- **Song.c** – Functions to manage songs:
  - Get, insert, delete, and update songs.
  - Link songs to artists in a linked-list fashion.
- **main.c** – Console-based user interface for interacting with the database.

---

## 💾 Database Files

- `artist.fl` – Binary file storing artist records.
- `artist.ind` – Binary file storing artist indices.
- `deleted_artists.txt` – Text file tracking deleted artist IDs.
- `song.fl` – Binary file storing song records.
- `deleted_songs.txt` – Text file tracking deleted song IDs.

---

## 🎯 Features

### Artist Operations

1. **Get artist by ID**  
   Retrieves and displays artist information.

2. **Insert artist**  
   Adds a new artist to the database and assigns an available ID (reuses deleted IDs if any).

3. **Update artist**  
   Modify the artist’s name, age, and country.

4. **Delete artist**  
   Marks the artist as deleted and removes all their songs. Updates deleted IDs list.

5. **Utility function (`ut_m`)**  
   Prints all existing artists in the database.

---

### Song Operations

1. **Get song by ID**  
   Retrieves a song of a specific artist.

2. **Insert song**  
   Adds a song to an artist’s collection. Maintains linked-list structure of songs for the artist.

3. **Update song**  
   Updates song information (title, album, genre).

4. **Delete song**  
   Removes a song from the database, relinking the artist’s remaining songs.

5. **Utility function (`ut_s`)**  
   Prints all existing songs in the database.

---

### Statistics / Calculation

- Calculate total number of artists.
- Calculate total number of songs.
- List each artist with their song count.
