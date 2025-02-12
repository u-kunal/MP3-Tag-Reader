
#include <stdio.h>
#include "edit.h"

int main(int argc, char *argv[])
{
    Music music;

    // Check if the command has arguments
    if (argc > 1)
    {
        // Check if the operation is to edit the MP3 file
        if (checkOperation(argv) == Edit)
        {
            // Ensure enough arguments are provided for editing
            if (argc >= 4)
            {
                // Read and validate the MP3 file
                if (readandvalidate(&music, argv[4]) == success)
                {
                    // Edit the MP3 file's information based on the arguments
                    editInfo(&music, argv[2], argv[3]);
                }
            }
            else
            {
                // Inform user about incorrect arguments for editing
                printf("\n...................................\n");
                printf("\n    Please enter correct arguments \n");
                printf("\n......................................\n");
            }
        }
        // Check if the operation is to view the MP3 file's metadata
        else if (checkOperation(argv) == View)
        {
            // Read and validate the MP3 file
            if (readandvalidate(&music, argv[2]) == success)
            {
                // View the metadata information of the MP3 file
                viewInfo(&music);
            }
        }
        // Check if the operation is to display the help menu
        else if (checkOperation(argv) == Help)
        {
            // Display the help instructions
            printHelp();
        }
    }
    else
    {
        // In case of insufficient arguments, show error and usage instructions
        printf("\n........................................................\n\n");
        printf("ERROR: ./a.out : INVALID ARGUMENTS\n");
        printf("USAGE : \n");
        printf("To view use : ./a.out -v mp3filename\n");
        printf("To edit use : ./a.out -e  -t/-a/-A/-m/-y/-c mp3filename\n");
        printf("To get help use : ./a.out --help\n");
        printf("\n........................................................\n\n");
    }
}