#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/sha.h>

#include "wordsmap.h"


#define NUMBER_OF_LISTS 256
#define HASH_ALGORITHM EVP_sha1()
#define DIGEST_LENGTH 20


/*
 * Helper function that computes the digest of a word and stores it in the location pointed by the third argument.
 */
void digest_word(const unsigned char *word, size_t word_length, unsigned char **digest, unsigned int *digest_length);

/*
 * Helper function that returns a pointer to the node in the list at the specified index of the map referring to the
 * passed word, if exists, NULL otherwise.
 */
node *lookup(const words_map *map, int list_index, const char *word);


words_map *create_map() {
    words_map *map = malloc(sizeof(words_map *));
    map->lists = calloc(NUMBER_OF_LISTS, sizeof(node *));

    return map;
}

void update_map(words_map *map, const char *word) {
    unsigned int digest_length = DIGEST_LENGTH;
    unsigned char *digest = calloc(digest_length, sizeof(unsigned char));

    int length = strlen(word);

    digest_word((unsigned char *) word, length, &digest, &digest_length);

    int index = digest[0] % NUMBER_OF_LISTS;
    free(digest);

    node *item = lookup(map, index, word);

    if (!item) {
        item = calloc(1, sizeof(item));

        item->next = map->lists[index];
        map->lists[index] = item;

        item->word = calloc(length + 1, sizeof(char));
        strcpy(item->word, word);

        item->count = 1;
    } else {
        item->count++;
    }

}

void print_map(words_map *map) {

    for (int i = 0; i < NUMBER_OF_LISTS; i++) {
        node *item = map->lists[i];

        while (item) {
            printf("%-20s - %3d\n", item->word, item->count);
            item = item->next;
        }

    }

}


words_map *merge_maps(int maps_count, ...) {
    words_map *map = create_map();

    va_list maps_list;

    va_start(maps_list, maps_count);

    for (int i = 0; i < maps_count; i++) {
        words_map *arg_map = va_arg(maps_list, words_map *);

        for (int j = 0; j < NUMBER_OF_LISTS; j++) {
            node *item = arg_map->lists[j];

            while (item) {
                update_map(map, item->word);
                item = item->next;
            }
        }
    }

    va_end(maps_list);

    return map;
}

node *lookup(const words_map *map, int list_index, const char *word) {
    node *item = map->lists[list_index];

    if (!item) {
        return NULL;
    }

    while (strcmp(item->word, word)) {
        if (!item->next) {
            return NULL;
        }

        item = item->next;
    }

    return item;
}

void free_map(words_map *map) {
    for (int i = 0; i < NUMBER_OF_LISTS; i++) {
        node *item = map->lists[i];

        while (item) {
            node *temp = item;
            item = item->next;

            free(temp->word);
            free(temp);
        }
    }

    free(map);
}

void digest_word(const unsigned char *word, size_t word_length, unsigned char **digest, unsigned int *digest_length) {
    EVP_MD_CTX * digest_context = EVP_MD_CTX_create();

    if (!digest_context) {
        printf("Unable to create digest context for \"%s\".\n", word);
        exit(1);
    }

    if (!EVP_DigestInit_ex(digest_context, HASH_ALGORITHM, NULL)) {
        printf("Unable to set up digest context for \"%s\".\n", word);
        exit(1);
    }

    if(!EVP_DigestUpdate(digest_context, word, word_length)) {
        printf("Unable to update digest context for \"%s\".\n", word);
        exit(1);
    }

    *digest = (unsigned char *) OPENSSL_malloc(EVP_MD_size(HASH_ALGORITHM));

    if(*digest == NULL) {
        printf("Unable to compute digest for \"%s\".\n", word);
        exit(1);
    }

    if(!EVP_DigestFinal_ex(digest_context, *digest, digest_length)) {
        printf("Unable to finalize digest context for \"%s\".\n", word);
        exit(1);
    }

    EVP_MD_CTX_destroy(digest_context);
}
