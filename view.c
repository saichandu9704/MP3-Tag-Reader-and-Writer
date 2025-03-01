#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "view.h"

OperationType check_operation (char* argv[])
{
    if (strcmp (argv [1], "-v") == 0)
    {
        return p_view;
    }
    else if (strcmp (argv [1], "-e") == 0)
    {
        return p_edit;
    }
    else if (strcmp (argv [1], "--help") == 0)
    {
        return p_help;
    }
    else
    {
        return p_unsupported;
    }
}

Status read_and_validate_mp3_file (char* argv[], TagInfo* mp3tagInfo)
{
    if (argv [2] == NULL)
    {
        printf ("INFO: For Viewing the Tags -> ./mp3_tag_reader -v <file_name.mp3>\n");
        return p_failure;
    }
    else
    {
        if(strstr(argv[2],".mp3")!=NULL)
        {
            mp3tagInfo->fptr_mp3 = fopen (argv [2], "r");		//Open the file in Read Only Mode.
            if (mp3tagInfo->fptr_mp3 != NULL)
            {
                
                char str[3];
                fread (str, 1, 3, mp3tagInfo->fptr_mp3);		
                if ((strcmp (str, "ID3")) == 0)				
                {
                    printf ("     Version ID: v2.3     \n");
                    printf ("---------------------------------------------\n");
                }
                else		
                {
                    printf ("ERROR: The file Signature is not matching with that of a '.mp3' file.\n");
                    printf ("INFO: For Viewing the Tags -> ./mp3_tag_reader -v <file_name.mp3>\n");
                    return p_failure;
                }
            }
            else		
            {
                printf ("ERROR: Unable to Open the %s file.\n", argv [2]);
                printf ("INFO: For Viewing the Tags -> ./mp3_tag_reader -v <file_name.mp3>\n");
                return p_failure;
            }
        }
        else
        {
            printf("ERROR: The File is Not .mp3 File\n");
            return p_failure;
        }

        fclose (mp3tagInfo->fptr_mp3);						//Close the file opened previously.
    }

    //No p-failure returned above, then return p_success.
    return p_success;
}

Status view_tag (char* argv[], TagInfo* mp3tagInfo)
{
    mp3tagInfo->fptr_mp3 = fopen (argv [2], "r");		//Open the file in Read Only Mode.
    fseek (mp3tagInfo->fptr_mp3, 10L, SEEK_SET);		//The first 10 bytes of the '.mp3' file is ID3 Header and need to be skipped.
	Status ret;

	//Function call with the necessary parameters to display the Information about Title tag.
    ret = get_and_display_data ("Title: ", "TIT2", mp3tagInfo->frame_Id, &mp3tagInfo->title_tag_size, mp3tagInfo->title_tag, mp3tagInfo->fptr_mp3);
    if (ret == p_failure)		//If the function doesn't display the data successfully, the process of Displaying will terminate.
    {
        printf ("ERROR: Title Frame ID Contents cannot be displayed.\n");
        return p_failure;
    }

	
    ret = get_and_display_data ("Artist: ", "TPE1", mp3tagInfo->frame_Id, &mp3tagInfo->artist_tag_size, mp3tagInfo->artist_tag, mp3tagInfo->fptr_mp3);
    if (ret == p_failure)		
    {
        printf ("ERROR: Artist Frame ID Contents cannot be displayed.\n");
        return p_failure;
    }

	
    ret = get_and_display_data ("Album: ", "TALB", mp3tagInfo->frame_Id, &mp3tagInfo->album_tag_size, mp3tagInfo->album_tag, mp3tagInfo->fptr_mp3);
    if (ret == p_failure)	
    {
        printf ("ERROR: Album Frame ID Contents cannot be displayed.\n");
        return p_failure;
    }


    ret = get_and_display_data ("Year: ", "TYER", mp3tagInfo->frame_Id, &mp3tagInfo->year_size, mp3tagInfo->year, mp3tagInfo->fptr_mp3);
    if (ret == p_failure)		
    {
        printf ("ERROR: Year Frame ID Contents cannot be displayed.\n");
        return p_failure;
    }

	
    ret = get_and_display_data ("Content: ", "TCON", mp3tagInfo->frame_Id, &mp3tagInfo->content_type_size, mp3tagInfo->content_type, mp3tagInfo->fptr_mp3);
    if (ret == p_failure)		
    {
        printf ("ERROR: Content Type Frame ID Contents cannot be displayed.\n");
        return p_failure;
    }

	
    ret = get_and_display_data ("Comments: ", "COMM", mp3tagInfo->frame_Id, &mp3tagInfo->comment_size, mp3tagInfo->comments, mp3tagInfo->fptr_mp3);
    if (ret == p_failure)		
    {
        printf ("ERROR: Content Type Frame ID Contents cannot be displayed.\n");
        return p_failure;
    }

    printf ("---------------------------------------------\n");
    fclose (mp3tagInfo->fptr_mp3);				//To close the Source MP3 file.

    
    return p_success;
}


Status get_and_display_data (const char* str_frame, const char* str_Id, char* frame_Id, uint* tag_size, char* tag, FILE* fptr)
{
    int count;
	//printf ("MP3 position = %ld.\n", ftell (fptr));
    count = fread (frame_Id, 1, 4, fptr);           //To read the 4 characters of the Source MP3 file for the Frame ID tag present.				
    if (count < 4)										
    {
        printf ("ERROR: Unable to read from the MP3 file.\n");
        return p_failure;
    }
    else
    {
        if ((strcmp (frame_Id, str_Id)) == 0)		
        {
            count = fread (tag_size, 4, 1, fptr);		

            if (count < 1)								
            {
                printf ("ERROR: Unable to read from the MP3 file.\n");
                return p_failure;
            }
            else					
            {
                char* pos = (char*) tag_size;
                for (int i = 0; i < ((sizeof (uint)) / 2); i++)
                {
                    pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
                    pos [(sizeof (uint)) - i - 1] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
                    pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
                }

               // printf ("%sSize = %u.\n", str_frame, *tag_size);
                tag = (char*) malloc (((*tag_size) * sizeof (char)));	//To allocate the memory dynamically for storing the contents of the Frame ID.	

                fseek (fptr, 3L, SEEK_CUR);	
                //printf ("Start: %ld.\n", ftell (fptr));

                count = fread (tag, 1, (*tag_size - 1), fptr);		
                if (count < (*tag_size - 1))						
                {
                    printf ("ERROR: Unable to read from the MP3 file.\n");
                    return p_failure;
                }
                else
                {
                    tag [*tag_size - 1] = '\0';						
                    printf ("%-20s%-30s\n", str_frame, tag);			
                }
            }
        }
        else		
        {
            printf ("ERROR: %s Frame ID not found.\n", str_Id);
            return p_failure;
        }
    }

    free (tag);			
    tag = NULL;			

    return p_success;
}





