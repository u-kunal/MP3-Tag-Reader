#include "edit.h"
#include <string.h>

/* Function to edit the information of the MP3 file */
Status editInfo(Music *music, char *ch, char *name)
{
    // Open the music file for reading
    if (openFiles(music) == success)
    {
        // Open a temporary file for writing the updated content
        FILE *fptr_dest = fopen("sample.mp3", "wb+");
        if (fptr_dest == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", "sample.mp3");
            return failure;
        }

        // Check if the MP3 file has a valid header and version
        if (checkheaderandversion(music->fptr_fname) == success)
        {
            // Read and validate the MP3 metadata
            if (readInfo(music) == success)
            {
                // Rename the tag based on the selected type and new name
                renametag(music, ch, name, fptr_dest);
                // Copy the edited content back to the original file
                copytoriginal(music->fptr_fname, fptr_dest);
            }
        }
    }
}

/* Function to rename a tag (e.g., title, artist, album, etc.) */
Status renametag(Music *music, char *ch, char *name, FILE *fptr_dest)
{
    // Copy the header from the original file to the destination file
    if (copyheader(music->fptr_fname, fptr_dest) == success)
    {
        // Check the type of edit operation based on the argument passed (-t, -a, -A, etc.)
        switch (checkedit(ch))
        {
        case t: // Edit song title
            copyContent(music->fptr_fname, music->pos[m_title], fptr_dest);
            copynewcon(music->fptr_fname, name, fptr_dest);
            copyremaining(music->fptr_fname, music->Title_size, fptr_dest);
            break;
        case a: // Edit artist name
            copyContent(music->fptr_fname, music->pos[m_album], fptr_dest);
            copynewcon(music->fptr_fname, name, fptr_dest);
            copyremaining(music->fptr_fname, music->Album_size, fptr_dest);
            break;
        case A: // Edit album name
            copyContent(music->fptr_fname, music->pos[m_artist], fptr_dest);
            copynewcon(music->fptr_fname, name, fptr_dest);
            copyremaining(music->fptr_fname, music->Artist_size, fptr_dest);
            break;
        case y: // Edit year
            copyContent(music->fptr_fname, music->pos[m_year], fptr_dest);
            copynewcon(music->fptr_fname, name, fptr_dest);
            copyremaining(music->fptr_fname, music->Year_size, fptr_dest);
            break;
        case m: // Edit genre
            copyContent(music->fptr_fname, music->pos[m_genre], fptr_dest);
            copynewcon(music->fptr_fname, name, fptr_dest);
            copyremaining(music->fptr_fname, music->Genre_size, fptr_dest);
            break;
        case c: // Edit comment
            copyContent(music->fptr_fname, music->pos[m_comment], fptr_dest);
            copynewcon(music->fptr_fname, name, fptr_dest);
            copyremaining(music->fptr_fname, music->Com_size, fptr_dest);
            break;
        default:
            break;
        }
    }
    return success;
}

/* Function to copy content from the original file up to the given position */
Status copyContent(FILE *fname, int pos, FILE *fptr_dest)
{
    char ch;
    // Copy bytes from the original file to the destination file up to the position
    for (int i = 10; i < pos; i++)
    {
        fread(&ch, 1, 1, fname);
        fwrite(&ch, 1, 1, fptr_dest);
    }
}

/* Function to copy the new tag value (name) into the destination file */
Status copynewcon(FILE *fname, char *name, FILE *fptr_dest)
{
    int len = strlen(name) + 1;  // Length of the new tag value, including the null-terminator
    char bytes[4];
    // Convert the length to 4 bytes (big-endian format) and write to the file
    for (int i = 0; i < 4; i++)
    {
        bytes[3 - i] = ((len >> 8 * i) & 0xFF);
    }
    fwrite(bytes, 4, 1, fptr_dest);
    
    // Skip 4 bytes in the original file and copy the 3-byte buffer
    fseek(fname, 4, SEEK_CUR);
    char buffer[3];
    fread(buffer, 3, 1, fname);
    fwrite(buffer, 3, 1, fptr_dest);
    
    // Write the new name (tag) into the destination file
    fwrite(name, len - 1, 1, fptr_dest);
    return success;
}

/* Function to copy the remaining content after the edited tag */
Status copyremaining(FILE *fname, int size, FILE *fptr_dest)
{
    fseek(fname, size, SEEK_CUR); // Move file pointer to the size of the tag
    char ch;
    // Copy the remaining bytes from the original file to the destination file
    while (fread(&ch, 1, 1, fname))
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return success;
}

/* Function to copy the header from the original MP3 file to the destination file */
Status copyheader(FILE *fname, FILE *fptr_dest)
{
    rewind(fname); // Move the file pointer to the beginning of the file
    char buffer[10];
    fread(buffer, 10, 1, fname);  // Read the first 10 bytes (MP3 header)
    fwrite(buffer, 10, 1, fptr_dest);  // Write the header to the destination file
    return success;
}

/* Function to copy the content from the temporary file back to the original file */
Status copytoriginal(FILE *fname, FILE *fptr_dest)
{
    rewind(fname);     // Move the file pointer of the original file to the beginning
    rewind(fptr_dest); // Move the file pointer of the destination file to the beginning
    char ch;
    // Copy content from the destination (temporary file) back to the original file
    while (fread(&ch, 1, 1, fptr_dest) > 0)
    {
        fwrite(&ch, 1, 1, fname);
    }
    fclose(fptr_dest); // Close the temporary file
    remove("sample.mp3"); // Delete the temporary file
    return success;
}

/* Function to determine which tag to edit based on the provided argument */
edittags checkedit(char *ch)
{
    if (!strcmp(ch, "-t")) return t; // Edit title
    else if (!strcmp(ch, "-a")) return a; // Edit artist
    else if (!strcmp(ch, "-A")) return A; // Edit album
    else if (!strcmp(ch, "-y")) return y; // Edit year
    else if (!strcmp(ch, "-m")) return m; // Edit genre
    else if (!strcmp(ch, "-c")) return c; // Edit comment
}
