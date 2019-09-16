// http://jonnyjd.github.io/libdiscid/
// http://jonnyjd.github.io/libdiscid/discid_8h.html

// http://www.freedb.org/freedb_discid_check.php
 

// Usage: discid <path to cd rom>
// e.g.:  discid /dev/cdrom
// Note:  Get the cdrom dev path with 'hwinfo --cdrom'

#include <stdlib.h>
#include <stdio.h>
#include "discid/discid.h"

int main(int argc, const char** argv)
{
    DiscId *disc = discid_new();

    if ( discid_read_sparse(disc, argv[1], 0) == 0 ) {
        fprintf(stderr, "Error: %s\n", discid_get_error_msg(disc));
        return 1;
    }

    printf("MusicBrainz DiscID  : %s\n", discid_get_id(disc));
    printf("MusicBrainz sub url : %s\n", discid_get_submission_url(disc));
    printf("FreeDB DiscID       : %s\n", discid_get_freedb_id(disc));

    discid_free(disc); 

    return 0;
}
