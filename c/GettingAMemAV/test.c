#include <stdlib.h>
#include <stdio.h>

static void p(char* v)
{
        int i;
        for(i = 0; i<5; i++)
        {
                printf("[%d]", v[i]);
        }
        printf("\r\n");
}


int main()
{
        char* w;
        char* v = (char*)malloc(sizeof(char) * 6);

        sprintf(v, "abcde");

        p(v); 
        // Output: [97][98][99][100][101]
        
        free(v);

        p(v); 
        // Output: [0][0][0][0][0]
        // This shows how you can still access memory freed.
        // free() (at least on gcc 4x) sets the memory to 0 before releasing it. 

        v[1] = '!';
        // Shouldn't we get a A/V here?

        p(v);
        // Output: [0][33][0][0][0]
        // This shows how you can can still write on memory not allocated!

        w = (char*)malloc(sizeof(char) * 6);
        sprintf(w, "fghij");

        p(v);
        // Output: [102][103][104][105][106]
        // So the memory previously pointed by v is now alloacated again, and pointed by w.
        // Since the same block of memory was allocated, the address is still the same, and the v pointer "became" valid again.
        // For sure a scary situation.

        free(w);

        return 0;
}
