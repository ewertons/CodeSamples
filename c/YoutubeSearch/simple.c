#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define BASE_SEARCH_URL "https://www.youtube.com/results?search_query="

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
static size_t curl_write_function_cb(void *content, size_t size, size_t nmemb, void *user_ctx)
{
	printf("{{{{%s}}}} %d", (char*)content, i++);
	return size * nmemb;
}

static void youtube_search(const char* search_query)
{
	char* search_query_url;
	
	if ((search_query_url = create_search_request_url(search_query)) == NULL)
	{
		
	}
	else
	{
		CURL *curl;
		CURLcode res;

		curl = curl_easy_init();

		if(curl) 
		{
			curl_easy_setopt(curl, CURLOPT_URL, search_query_url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function_cb);

			res = curl_easy_perform(curl);

			if(res != CURLE_OK)
			{
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}

			curl_easy_cleanup(curl);
		}
		
		free(search_query_url);
	}
}

int main(void)
{
	youtube_search("pink floyd");
	return 0;
}

