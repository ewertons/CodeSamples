#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <curl/curl.h>
#include <myhtml/api.h>

#define BASE_SEARCH_URL "https://www.youtube.com/results?search_query="
#define __RESULT_ERROR__ __LINE__
#define __RESULT_OK__ 0

typedef enum YOUTUBE_MEDIA_TYPE_TAG
{
    YOUTUBE_MEDIA_TYPE_UNKNOWN,
    YOUTUBE_MEDIA_TYPE_WATCH,
    YOUTUBE_MEDIA_TYPE_PLAYLIST,
    YOUTUBE_MEDIA_TYPE_CHANNEL
} YOUTUBE_MEDIA_TYPE;

typedef struct YOUTUBE_MEDIA_DESCRIPTION_TAG
{
        YOUTUBE_MEDIA_TYPE type;
	char* title;
        char* address;
	bool is_relative_address;
} YOUTUBE_MEDIA_DESCRIPTION;

static char* create_search_request_url(const char* search_query)
{
	char* result;
	
	if ((result = (char*)malloc(sizeof(char) * (strlen(BASE_SEARCH_URL) + strlen(search_query)))) == NULL)
	{
	}
	else
	{
		char* modified_search_query;
		size_t length = strlen(search_query) + 1;
		
		if ((modified_search_query = (char*)malloc(length * sizeof(char))) == NULL)
		{
			free(result);
			result = NULL;
		}
		else
		{
			size_t i;
			memcpy(modified_search_query, search_query, length);
			
			for (i = 0; i < length; i++)
			{
				if (modified_search_query[i] == ' ')
				{
					modified_search_query[i] = '+';
				}
			}
			
			(void)sprintf(result, "%s%s", BASE_SEARCH_URL, modified_search_query);
			
			free(modified_search_query);
		}
	}
	
	return result;
}

static i = 0;
static size_t curl_write_function_cb(void* data, size_t size, size_t nmemb, void *user_ctx)
{
	char** content = (char**)user_ctx;
	size_t data_length = size * nmemb;
	size_t original_length = (*content != NULL ? strlen(*content) : 0);
	size_t new_length = original_length + data_length;

	if ((*content = realloc(*content, new_length)) == NULL)
	{
		
	}
	else
	{
		memcpy(*content + original_length, data, data_length);
	}
	
	return data_length;
}

static char* http_get(const char* url)
{
	char* result = NULL;
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();

	if(curl) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		{
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			free(result);
			result = NULL;
		}

		curl_easy_cleanup(curl);
	}

	return result;
}

static char* find_media_item_result(char* content)
{
	char* result;

	if ((result = strstr(content, "<a")) != NULL)
	{
		char* tag_end = strstr(result, ">");
		char* target_type = strstr(result, "yt-uix-tile-address");

		if (target_type == NULL || target_type > tag_end)
		{
			result = NULL;
		}
	}

	return result;
}

static YOUTUBE_MEDIA_DESCRIPTION* youtube_media_description_create(const char* address, size_t address_length, const char* title, size_t title_length)
{
    YOUTUBE_MEDIA_DESCRIPTION* result;
    YOUTUBE_MEDIA_TYPE media_type;

        if (strstr(address, "/watch?") != NULL)
        {
            media_type = YOUTUBE_MEDIA_TYPE_WATCH;
        }
        else if (strstr(address, "/playlist?") != NULL)
        {
            media_type = YOUTUBE_MEDIA_TYPE_PLAYLIST;
        }
        else if (strstr(address, "/channel/") != NULL)
        {
            media_type = YOUTUBE_MEDIA_TYPE_CHANNEL;
        }
        else
        {
            media_type = YOUTUBE_MEDIA_TYPE_UNKNOWN;
        }


    if (media_type == YOUTUBE_MEDIA_TYPE_UNKNOWN)
    {
        result = NULL;
    }
    else
    {
    if ((result = (YOUTUBE_MEDIA_DESCRIPTION*)malloc(sizeof(YOUTUBE_MEDIA_DESCRIPTION))) == NULL)
    {
        printf("Failed allocating YOUTUBE_MEDIA_DESCRIPTION\r\n");
    }
    else if ((result->title = (char*)malloc(sizeof(char) * (title_length + 1))) == NULL)
    {
         printf("Failed setting YOUTUBE_MEDIA_DESCRIPTION title\r\n");
         free(result);
         result = NULL;
    }
    else if ((result->address = (char*)malloc(sizeof(char) * (address_length + 1))) == NULL)
    {
        printf("Failed setting YOUTUBE_MEDIA_DESCRIPTION address\r\n");
        free(result->title);
        free(result);
        result = NULL;
    }
    else
    {
        result->title[title_length] = '\0';
        result->address[address_length] = '\0';

        memcpy(result->title, title, title_length);
        memcpy(result->address, address, address_length);

        result->type = media_type;

        if (strstr(result->address, "youtube.com") != NULL)
        {
            result->is_relative_address = false;
        }
        else
        {
            result->is_relative_address = true;
        }
    }
    }

    return result;
}

static void parse_youtube_search_result_page(char* content, YOUTUBE_MEDIA_DESCRIPTION*** descriptions, size_t* count)
{
    YOUTUBE_MEDIA_DESCRIPTION** results = NULL;
    size_t l_count = 0;

    myhtml_t *myhtml = myhtml_create();
    myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);
    
    myhtml_tree_t *tree = myhtml_tree_create();
    myhtml_tree_init(tree, myhtml);

    myhtml_parse(tree, MyENCODING_UTF_8, content, strlen(content));

    myhtml_collection_t *collection = myhtml_get_nodes_by_name(tree, NULL, "a", 1, NULL);

    size_t i;
    for (i = 0; i < collection->length; i++)
    {
        size_t href_length;
        size_t title_length;
        const char* href = NULL;
        const char* title = NULL;

        myhtml_tree_attr_t* attr = myhtml_node_attribute_first(collection->list[i]);

        while (attr != NULL)
        {
            size_t key_length;
            size_t value_length;
            const char* key = myhtml_attribute_key(attr, &key_length);
            const char* value = myhtml_attribute_value(attr, &value_length);

            if (key != NULL && value != NULL)
            {
                if (strcmp("href", key) == 0)
                {
                    href = value;
                    href_length = value_length;
                }
                else if (strcmp("title", key) == 0)
                {
                    title = value;
                    title_length = value_length;
                }

                if (href != NULL && title != NULL)
                {
                    YOUTUBE_MEDIA_DESCRIPTION* new_item;

                    if ((new_item = youtube_media_description_create(href, href_length, title, title_length)) != NULL)
                    {
                        if ((results = (YOUTUBE_MEDIA_DESCRIPTION**)realloc(results, sizeof(YOUTUBE_MEDIA_DESCRIPTION*) * (l_count + 1))) == NULL)
                        {
                            printf("Failed allocating space for YOUTUBE_MEDIA_DESCRIPTION in list of descriptions\r\n"); 
                            free(new_item);
                        }
                        else
                        {
                            results[l_count] = new_item;
                            l_count++;
                        }
                    }

                    break;
                }
            }

            attr = myhtml_attribute_next(attr);
        }
    }

    *descriptions = results;
    *count = l_count;
}

static int youtube_search(const char* search_query, YOUTUBE_MEDIA_DESCRIPTION*** results, size_t* count)
{
	int result;
	char* url;

	if ((url = create_search_request_url(search_query)) == NULL)
	{
		result = __RESULT_ERROR__;
	}
	else
	{
		char* page_content;
		
		if ((page_content = http_get(url)) == NULL)
		{
			result = __RESULT_ERROR__;
		}
		else
		{
			parse_youtube_search_result_page(page_content, results, count);
			free(page_content);
			result = __RESULT_OK__;
		}

		free(url);
	}

	return result;
}

static void youtube_destroy_search_results(YOUTUBE_MEDIA_DESCRIPTION** results, size_t count)
{
	if (results != NULL)
	{
		size_t i;
		for (i = 0; i < count; i++)
		{
			free(results[i]->title);
			free(results[i]->address);
			free(results[i]);
		}
		free(results);
	}	
}

#define YOUTUBE_BASE_URL "https://www.youtube.com"
#define YOUTUBE_BASE_URL_LENGTH 23

static char* youtube_get_absolute_address(YOUTUBE_MEDIA_DESCRIPTION* yt_media_description)
{
    char* result;

    if (yt_media_description == NULL)
    {
        printf("Invalid argument\r\n");
    }
    else if (yt_media_description->is_relative_address)
    {
        size_t result_length = YOUTUBE_BASE_URL_LENGTH + strlen(yt_media_description->address) + 1;

        if ((result = (char*)malloc(sizeof(char) * result_length)) == NULL)
        {
        }
        else
        {
            sprintf(result, "%s%s", YOUTUBE_BASE_URL, yt_media_description->address);
        }
    }
    else
    {
        size_t result_length = strlen(yt_media_description->address) + 1;

        if ((result = (char*)malloc(sizeof(char) * result_length)) == NULL)
        {
        }
        else
        {
            sprintf(result, "%s", yt_media_description->address);
        }
    }

    return result; 
}

int main(void)
{
    YOUTUBE_MEDIA_DESCRIPTION** items;
    size_t count;
    youtube_search("pink floyd", &items, &count);
    printf("Found %d results:\r\n", count);

    size_t i;
    for (i = 0; i < count; i++)
    {
        char* url = youtube_get_absolute_address(items[i]);
        printf("%s    %s\r\n", items[i]->title, url);
        free(url);
    }

    youtube_destroy_search_results(items, count);
    return 0;
}

