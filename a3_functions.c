/* Abdullah Alsibakhi — coursework adapted into a standalone C demonstration.
 * See README.md for starter-code attribution and input limits.
 */
#include "a3_functions.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static bool valid_text(const char *text, size_t capacity)
{
    return text != NULL && text[0] != '\0' && strlen(text) < capacity;
}

static bool normalize_username(const char *text, char out[30])
{
    if (!valid_text(text, 30)) return false;
    size_t i;
    for (i = 0; text[i]; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (c <= ' ' || c == ',' || c == '"') return false;
        out[i] = c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : (char)c;
    }
    out[i] = '\0';
    return true;
}

user_t *find_user(user_t *users, const char *username)
{
    char normalized[30];
    if (!normalize_username(username, normalized)) return NULL;
    for (; users; users = users->next)
        if (strcmp(users->username, normalized) == 0) return users;
    return NULL;
}

user_t *add_user(user_t *users, const char *username, const char *password)
{
    char normalized[30];
    if (!normalize_username(username, normalized) ||
        !valid_text(password, sizeof(((user_t *)0)->password)) ||
        find_user(users, normalized)) return users;
    user_t *node = calloc(1, sizeof(*node));
    if (!node) return users;
    strcpy(node->username, normalized);
    strcpy(node->password, password); /* Length validated above. Demo only. */
    user_t **link = &users;
    while (*link && strcmp((*link)->username, normalized) < 0)
        link = &(*link)->next;
    node->next = *link;
    *link = node;
    return users;
}

friend_t *create_friend(const char *username)
{
    char normalized[30];
    if (!normalize_username(username, normalized)) return NULL;
    friend_t *node = calloc(1, sizeof(*node));
    if (node) strcpy(node->username, normalized);
    return node;
}

void add_friend(user_t *user, const char *friend_name)
{
    if (!user) return;
    friend_t *node = create_friend(friend_name);
    if (!node) return;
    if (strcmp(node->username, user->username) == 0) { free(node); return; }
    friend_t **link = &user->friends;
    while (*link && strcmp((*link)->username, node->username) < 0)
        link = &(*link)->next;
    if (*link && strcmp((*link)->username, node->username) == 0) {
        free(node); return;
    }
    node->next = *link;
    *link = node;
}

_Bool delete_friend(user_t *user, char *friend_name)
{
    char normalized[30];
    if (!user || !normalize_username(friend_name, normalized)) return false;
    friend_t **link = &user->friends;
    while (*link && strcmp((*link)->username, normalized) != 0)
        link = &(*link)->next;
    if (!*link) return false;
    friend_t *old = *link;
    *link = old->next;
    free(old);
    return true;
}

post_t *create_post(const char *text)
{
    if (!valid_text(text, sizeof(((post_t *)0)->content))) return NULL;
    post_t *node = calloc(1, sizeof(*node));
    if (node) strcpy(node->content, text);
    return node;
}

void add_post(user_t *user, const char *text)
{
    if (!user) return;
    post_t *node = create_post(text);
    if (!node) return;
    node->next = user->posts;
    user->posts = node;
}

_Bool delete_post(user_t *user)
{
    if (!user || !user->posts) return false;
    post_t *old = user->posts;
    user->posts = old->next;
    free(old);
    return true;
}

void display_all_user_posts(user_t *user)
{
    if (!user) return;
    size_t number = 1;
    for (post_t *p = user->posts; p; p = p->next)
        printf("%zu - %s\n", number++, p->content);
}

void display_user_friends(user_t *user)
{
    if (!user) return;
    size_t number = 1;
    for (friend_t *f = user->friends; f; f = f->next)
        printf("%zu - %s\n", number++, f->username);
}

void display_posts_by_n(user_t *user, int number)
{
    if (!user || number <= 0) return;
    post_t *post = user->posts;
    size_t index = 1;
    while (post) {
        for (int i = 0; i < number && post; ++i) {
            printf("%zu - %s\n", index++, post->content);
            post = post->next;
        }
        if (!post) { puts("All posts have been displayed."); return; }
        printf("Show %d more posts? (Y/N): ", number);
        fflush(stdout);
        int choice = getchar();
        if (choice == EOF) return;
        int c = choice;
        while (c != '\n' && c != EOF) c = getchar();
        if (choice != 'y' && choice != 'Y') return;
    }
}

void teardown(user_t *users)
{
    while (users) {
        user_t *next = users->next;
        while (users->friends) {
            friend_t *old = users->friends;
            users->friends = old->next;
            free(old);
        }
        while (delete_post(users)) { }
        free(users);
        users = next;
    }
}

/* Preserve empty fields; this intentionally does not implement quoted CSV. */
static char *next_field(char **cursor)
{
    if (!*cursor) return NULL;
    char *field = *cursor;
    char *comma = strchr(field, ',');
    if (comma) { *comma = '\0'; *cursor = comma + 1; }
    else *cursor = NULL;
    return field;
}

user_t *read_CSV_and_create_users(FILE *file, int num_users)
{
    if (!file || num_users <= 0) return NULL;
    char line[4096];
    user_t *users = NULL;
    /* One header line is required. */
    if (!fgets(line, sizeof(line), file)) return NULL;
    if (!strchr(line, '\n') && !feof(file)) return NULL;
    for (int row = 0; row < num_users && fgets(line, sizeof(line), file); ++row) {
        if (!strchr(line, '\n') && !feof(file)) {
            int c;
            while ((c = fgetc(file)) != '\n' && c != EOF) { }
            continue;
        }
        line[strcspn(line, "\r\n")] = '\0';
        if (strchr(line, '"')) continue;
        char *cursor = line;
        char *name = next_field(&cursor);
        char *password = next_field(&cursor);
        char *friends[3];
        for (int i = 0; i < 3; ++i) friends[i] = next_field(&cursor);
        char normalized[30];
        bool valid = normalize_username(name, normalized) && valid_text(password, 15);
        for (int i = 0; i < 3; ++i) {
            char checked[30];
            if (!friends[i] || (*friends[i] && !normalize_username(friends[i], checked))) valid = false;
        }
        /* Validate all posts before adding any part of the row. */
        char *posts = cursor;
        while (cursor) {
            char *text = next_field(&cursor);
            if (strlen(text) >= 250) valid = false;
            /* Restore separators for the insertion pass. */
            if (cursor) cursor[-1] = ',';
        }
        if (!valid || find_user(users, name)) continue;
        users = add_user(users, name, password);
        user_t *user = find_user(users, name);
        if (!user) continue;
        for (int i = 0; i < 3; ++i) if (*friends[i]) add_friend(user, friends[i]);
        while (posts) {
            char *text = next_field(&posts);
            if (*text) add_post(user, text);
        }
    }
    return users;
}

void print_menu(void)
{
    puts("Linked-list social network demo");
    puts("1. Register user\n2. Add friend\n3. Remove friend\n4. Add post");
    puts("5. Remove newest post\n6. Show friends\n7. Show posts\n8. List users\n0. Exit");
}
