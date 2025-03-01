#ifndef EDIT_H
#define EDIT_H

#include "types.h"      //Contains User-defined Data Types.


typedef struct _TagData
{
    /* Source MP3 file Info */
    FILE* fptr_mp3;

	/* Temporary MP3 file Info */
    FILE* fptr_temp;

	/* Updated value Info passed in the Command Line */
    char frame_Id [5];
    char frame_Id_value [100];
    uint frame_Id_size;
} TagData;

/* Editing MP3 details Function Prototype */

/* Perform Validation of the Arguments passed and store in the Structure */
Status read_and_validate_mp3_file_args (char* argv[], TagData* mp3tagData);

/* Perform the Editing Tag details Operation */
Status edit_tag (char* argv[], TagData* mp3tagData);

/* Copy the Frame Contents from the Source to Temporary MP3 file */
Status copy_data (TagData* mp3tagData, char flag);

/* Copy the contents after the Frames have been copied from Source to Temporary MP3 file */
Status copy_remaining_data (TagData* mp3tagData);

/* Copy the complete Data from the Temporary to Source MP3 file */
Status copy_back_to_src (TagData* mp3tagData);

#endif
