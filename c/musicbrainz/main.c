// https://curl.haxx.se/libcurl/c/getinmemory.html

// http://musicbrainz.org/ws/2/release/?query=discid:gMeA7b6MNjRzWC293AG6IyYMli4-

// http://musicbrainz.org/ws/2/cdstub/?query=discid:gMeA7b6MNjRzWC293AG6IyYMli4-


// http://musicbrainz.org/ws/2/release/?query=barcode:602517807372&fmt=json

// http://musicbrainz.org/ws/2/recording/?query=reid:3e9e79bb-86f3-3755-8061-bf2a46a7dece&fmt=json

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "curl/curl.h"

#define MAX_TITLE_LENGTH 128
#define MAX_DATE_LENGTH 11

typedef struct ALBUM_INFO_STRUCT
{
    char title[MAX_TITLE_LENGTH];
    char date[MAX_DATE_LENGTH];
    
} ALBUM_INFO;

struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);

    if(ptr == NULL) 
    {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main(int argc, const char** argv)
{
    CURL *curl = curl_easy_init();

    if (curl) 
    {
        CURLcode res;
        
        struct MemoryStruct chunk;
 
        chunk.memory = malloc(1);
        chunk.size = 0;
        
        curl_easy_setopt(curl, CURLOPT_URL, "http://musicbrainz.org/ws/2/discid/gMeA7b6MNjRzWC293AG6IyYMli4-?fmt=json");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 Edge/18.18362");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
        }
        else
        {
             long res_status;
             curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &res_status);
             printf("Return code: %d\r\n", res_status);
             
             printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
             printf("body: %s\n", chunk.memory);
             
        }
 
        free(chunk.memory);
 
        curl_easy_cleanup(curl);
    }

    return 0;
}

