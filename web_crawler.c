
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Structure to hold the fetched HTML content
struct Memory {
    char *response;
    size_t size;
};

// Callback function to write fetched data into the Memory structure
size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->response, mem->size + real_size + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, real_size);
    mem->size += real_size;
    mem->response[mem->size] = 0;

    return real_size;
}

// Function to fetch webpage content
char *fetch_page(const char *url) {
    CURL *curl;
    CURLcode res;

    struct Memory chunk = {NULL, 0};

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "CURL error: %s\n", curl_easy_strerror(res));
        free(chunk.response);
        curl_easy_cleanup(curl);
        return NULL;
    }

    curl_easy_cleanup(curl);
    return chunk.response;
}

// Function to extract and print links from HTML content
void extract_links(const char *html) {
    const char *tag = "href="";
    const char *pos = html;
    while ((pos = strstr(pos, tag)) != NULL) {
        pos += strlen(tag);
        const char *end = strchr(pos, '"');
        if (end) {
            printf("Link: %.*s\n", (int)(end - pos), pos);
            pos = end;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <URL>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *url = argv[1];
    printf("Fetching page: %s\n", url);

    char *html = fetch_page(url);
    if (html) {
        printf("Extracting links...\n");
        extract_links(html);
        free(html);
    } else {
        fprintf(stderr, "Failed to fetch the page.\n");
    }

    return EXIT_SUCCESS;
}
