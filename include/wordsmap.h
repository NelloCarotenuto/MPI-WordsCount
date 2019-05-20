/*
 * Defines types to implement a simple hashmap with a variable number of buckets each of which contains a list of words
 * whose digest's first (unsigned) character modulo the number of buckets equals to bucket's index.
 */
typedef struct words_map {
    struct node **lists;
} words_map;

typedef struct node {
    char *word;
    int count;
    struct node *next;
} node;

/*
 *  Create a new words map and return its address to the caller.
 */
words_map *create_map(void);

/*
 * Update the map at the specified address with the passed word. This function adds the word to the map if it does not
 * already exist in the latter or updates its count otherwise.
 */
void update_map(words_map *map, const char *word);

/*
 * Print the content of the specified map.
 */
void print_map(words_map *map);

/*
 * Create a new map containing the words of the maps passed as parameters and return its address.
 * This function doesn't deallocate the maps passed as arguments.
 */
words_map *merge_maps(int maps_count, ...);

/*
 * Deallocate the map at the specified address,
 */
void free_map(words_map *map);
