/* Small standalone demo; not an authentication system. Use fictional data. */
#include "a3_functions.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool input(const char *prompt, char *text, size_t size)
{
    for (;;) {
        fputs(prompt, stdout);
        fflush(stdout);
        if (!fgets(text, (int)size, stdin)) return false;
        char *newline = strchr(text, '\n');
        if (newline) { *newline = '\0'; return true; }
        int c = getchar();
        if (c == '\n' || c == EOF) return true;
        while (c != '\n' && c != EOF) c = getchar();
        puts("Input too long. Try again.");
        if (c == EOF) return false;
    }
}

int main(int argc, char **argv)
{
    user_t *users = NULL;
    if (argc > 2) { fprintf(stderr, "Usage: %s [sample.csv]\n", argv[0]); return 1; }
    if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        if (!file) { perror("Cannot open CSV"); return 1; }
        users = read_CSV_and_create_users(file, 10000);
        fclose(file);
    }
    puts("Educational demo — use fictional usernames and passwords only.");
    char choice[16], name[30], text[250], password[15];
    while (true) {
        print_menu();
        if (!input("Choice: ", choice, sizeof(choice)) || strcmp(choice, "0") == 0) break;
        if (strcmp(choice, "8") == 0) {
            for (user_t *u = users; u; u = u->next) puts(u->username);
            continue;
        }
        if (strlen(choice) != 1 || choice[0] < '1' || choice[0] > '7') {
            puts("Choose an option from 0 to 8."); continue;
        }
        if (!input("Username: ", name, sizeof(name))) break;
        if (choice[0] == '1') {
            if (find_user(users, name)) { puts("That username already exists."); continue; }
            if (!input("Demo password (not a real password): ", password, sizeof(password))) break;
            users = add_user(users, name, password);
            puts(find_user(users, name) ? "User added." : "Could not add user. Check input or available memory.");
            continue;
        }
        user_t *user = find_user(users, name);
        if (!user) { puts("User not found."); continue; }
        switch (choice[0]) {
        case '2':
            if (!input("Friend username: ", text, sizeof(text))) goto done;
            add_friend(user, text);
            puts("Current friends (duplicates, self-links and invalid names are ignored):");
            display_user_friends(user); break;
        case '3':
            if (!input("Friend username: ", text, sizeof(text))) goto done;
            puts(delete_friend(user, text) ? "Friend removed." : "Friend not found."); break;
        case '4': {
            if (!input("Post: ", text, sizeof(text))) goto done;
            post_t *before = user->posts;
            add_post(user, text);
            puts(user->posts != before ? "Post added." : "Could not add post."); break;
        }
        case '5': puts(delete_post(user) ? "Newest post removed." : "No posts to remove."); break;
        case '6': display_user_friends(user); break;
        case '7': display_posts_by_n(user, 3); break;
        }
    }
done:
    teardown(users);
    return 0;
}
