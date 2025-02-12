#include <stdio.h>
#include "music.h"
#include <string.h>
#include <stdlib.h>

/* Function to check what operation to perform: Edit, View, or Help */
Operation checkOperation(char **argv)
{
    if (strcmp(argv[1], "-e") == 0) // Check if the operation is Edit
        return Edit;
    else if (strcmp(argv[1], "-v") == 0) // Check if the operation is View
        return View;
    else // Any other case, return Help
        return Help;
}

/* Function to print the help menu with operation instructions */
void printHelp()
{
    // Display the help menu explaining various operations and sub-commands
    printf("\n.............Help Menu.....................\n\n");
    printf("1. -v -> to view mp3 file contents\n");
    printf("2. -e -> to edit mp3 file contents\n");
    printf(" 2.1. -t -> to edit song title\n");
    printf(" 2.2. -A -> to edit artist name\n");
    printf(" 2.3. -a -> to edit album name\n");
    printf(" 2.4. -y -> to edit song year\n");
    printf(" 2.5. -m -> to edit song content\n");
    printf(" 2.6. -c -> to edit song comment\n");
    printf("\n............................................\n\n");
}

/* Function to validate the given argument to check if it's a valid MP3 file */
Status readandvalidate(Music *music, char *arg)
{
    // Check if the provided file has an ".mp3" extension
    if (strcmp(strstr(arg, "."), ".mp3") == 0)
    {
        music->Filename = arg; // If valid, assign filename
        return success;
    }
    else
    {
        // If it's not an MP3 file, show an error message
        printf("\n..................................\n\n");
        printf("ERROR: Mp3 File Type only\n");
        printf("\n..................................\n\n");
    }
}

/* Function to open the MP3 file for reading and writing */
Status openFiles(Music *music)
{
    music->fptr_fname = fopen(music->Filename, "rb+"); // Open the file in read-write binary mode
    if (music->fptr_fname == NULL)
    {
        perror("fopen"); // If failed, print error
        fprintf(stderr, "ERROR: Unable to open file %s\n", music->Filename);
        return failure;
    }
    return success;
}

/* Function to check if the MP3 file header is valid and contains ID3v2 version 3.00 */
Status checkheaderandversion(FILE *fname)
{
    char head[4]; // To store the "ID3" header
    char ver[2];  // To store version number (should be 03 00 for v2.3)
    
    // Read the header and version bytes
    fread(head, 3, 1, fname);
    head[3] = '\0'; // Null-terminate the header string
    fread(ver, 2, 1, fname);

    // Validate the header and version
    if (strcmp(head, "ID3") == 0 && ver[0] == 0x03 && ver[1] == 0x00)
    {
        return success;
    }
    return failure; // Return failure if header or version is incorrect
}

/* Function to view and display MP3 file's metadata */
Status viewInfo(Music *music)
{
    // Open the file and check if it's valid
    if (openFiles(music) == success)
    {
        // Check header and version
        if (checkheaderandversion(music->fptr_fname) == success)
        {
            // Read the metadata and then print the information
            if (readInfo(music) == success)
            {
                printInfo(music);
            }
        }
    }
}

/* Function to read the metadata and tags from the MP3 file */
Status readInfo(Music *music)
{
    rewind(music->fptr_fname); // Go to the beginning of the file
    fseek(music->fptr_fname, 10, SEEK_CUR); // Skip the header section (first 10 bytes)

    // Read tags sequentially and store the metadata
    for (int i = 0; i < 6; i++)
    {
        tags t = tagreader(music->fptr_fname); // Read the tag type
        switch (t)
        {
        case m_title:
            music->pos[m_title] = ftell(music->fptr_fname); // Store the position of the title tag
            tagsizereader(music->fptr_fname, &music->Title_size); // Get the size of the title
            fseek(music->fptr_fname, 3, SEEK_CUR); // Skip the tag header
            readtaginfo(music->fptr_fname, &music->Title, music->Title_size); // Read the tag value
            break;
        case m_album:
            music->pos[m_album] = ftell(music->fptr_fname); // Store position for album tag
            tagsizereader(music->fptr_fname, &music->Album_size);
            fseek(music->fptr_fname, 3, SEEK_CUR);
            readtaginfo(music->fptr_fname, &music->album, music->Album_size);
            break;
        case m_artist:
            music->pos[m_artist] = ftell(music->fptr_fname);
            tagsizereader(music->fptr_fname, &music->Artist_size);
            fseek(music->fptr_fname, 3, SEEK_CUR);
            readtaginfo(music->fptr_fname, &music->Artist, music->Artist_size);
            break;
        case m_year:
            music->pos[m_year] = ftell(music->fptr_fname);
            tagsizereader(music->fptr_fname, &music->Year_size);
            fseek(music->fptr_fname, 3, SEEK_CUR);
            readtaginfo(music->fptr_fname, &music->Year, music->Year_size);
            break;
        case m_genre:
            music->pos[m_genre] = ftell(music->fptr_fname);
            tagsizereader(music->fptr_fname, &music->Genre_size);
            fseek(music->fptr_fname, 3, SEEK_CUR);
            readtaginfo(music->fptr_fname, &music->Genre, music->Genre_size);
            break;
        case m_comment:
            music->pos[m_comment] = ftell(music->fptr_fname);
            tagsizereader(music->fptr_fname, &music->Com_size);
            fseek(music->fptr_fname, 3, SEEK_CUR);
            readtaginfo(music->fptr_fname, &music->Comment, music->Com_size);
            break;
        default:
            break; // Skip any unexpected tags
        }
    }
    return success;
}

/* Function to print the MP3 metadata in a user-friendly format */
Status printInfo(Music *music)
{
    // Print the file metadata details
    printf("\n........................Selected View Details...................\n");
    printf("\n................................................................\n");
    printf("\n                MP3 TAG READER AND EDITOR FOR ID3v2          \n");
    printf("\n................................................................\n\n");
    printf("TITLE          : %s \n", music->Title);
    printf("ARTIST         : %s \n", music->Artist);
    printf("ALBUM          : %s \n", music->album);
    printf("YEAR           : %s \n", music->Year);
    printf("MUSIC          : %s \n", music->Genre);
    printf("COMMENT        : %s \n", music->Comment);
    printf("\n................................................................\n\n");
    return success;
}

/* Function to read the tag type from the MP3 file */
tags tagreader(FILE *fname)
{
    char tag[4 + 1];
    fread(tag, 4, 1, fname);  // Read the tag identifier (4 characters)
    tag[4] = '\0'; // Null-terminate the string
    // Match the tag identifier to determine which tag it corresponds to
    if (strcmp(tag, "TIT2") == 0)
        return m_title;
    else if (strcmp(tag, "TALB") == 0)
        return m_album;
    else if (strcmp(tag, "TPE1") == 0)
        return m_artist;
    else if (strcmp(tag, "TYER") == 0)
        return m_year;
    else if (strcmp(tag, "TCON") == 0)
        return m_genre;
    else if (strcmp(tag, "COMM") == 0)
        return m_comment;
}

/* Function to read the size of each tag */
Status tagsizereader(FILE *fname, int *size)
{
    char ch;
    *size = 0;
    // Read 4 bytes and combine them to get the tag size
    for (int i = 0; i < 4; i++)
    {
        fread(&ch, 1, 1, fname);
        *size = *size << (i * 8) | ch;
    }
    *size = *size - 1; // Subtract 1 to get the actual size
}

/* Function to read the content of the tag into a string */
Status readtaginfo(FILE *fname, char **name, int size)
{
    char *buffer = malloc(size + 1); // Allocate buffer to hold tag data
    fread(buffer, size, 1, fname); // Read the tag data
    buffer[size] = '\0'; // Null-terminate the buffer
    *name = malloc(size + 1); // Allocate memory for the name field
    strcpy(*name, buffer); // Copy the buffer to the name field
    free(buffer); // Free the temporary buffer
}
