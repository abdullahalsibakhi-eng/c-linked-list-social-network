#include "a3_functions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void lists(void)
{
    user_t *users = NULL;
    users = add_user(users, "Zoe", "demo");
    users = add_user(users, "Alice", "demo");
    users = add_user(users, "Bob", "demo");
    users = add_user(users, "ALICE", "duplicate");
    assert(strcmp(users->username, "alice") == 0);
    assert(strcmp(users->next->username, "bob") == 0);
    assert(strcmp(users->next->next->username, "zoe") == 0);
    assert(users->next->next->next == NULL);
    user_t *alice = find_user(users, "ALICE");
    assert(alice == users && !find_user(users, "missing"));
    add_friend(alice, "Zoe"); add_friend(alice, "Bob");
    add_friend(alice, "BOB"); add_friend(alice, "Alice");
    assert(strcmp(alice->friends->username, "bob") == 0);
    assert(strcmp(alice->friends->next->username, "zoe") == 0);
    assert(!alice->friends->next->next);
    assert(delete_friend(alice, "ZOE"));
    assert(delete_friend(alice, "bob"));
    assert(!delete_friend(alice, "bob"));
    add_post(alice, "first"); add_post(alice, "second");
    assert(strcmp(alice->posts->content, "second") == 0);
    assert(delete_post(alice));
    assert(strcmp(alice->posts->content, "first") == 0);
    assert(delete_post(alice)); assert(!delete_post(alice));
    add_post(alice, "cleanup"); add_friend(alice, "cleanup");
    teardown(users);
}

static void boundaries(void)
{
    char name[31], password[16], post[251];
    memset(name, 'a', sizeof(name)); name[30] = 0;
    memset(password, 'p', sizeof(password)); password[15] = 0;
    memset(post, 'x', sizeof(post)); post[250] = 0;
    assert(!add_user(NULL, name, "demo"));
    assert(!add_user(NULL, "alice", password));
    assert(!create_post(post));
    name[29] = 0; password[14] = 0; post[249] = 0;
    user_t *user = add_user(NULL, name, password); assert(user);
    add_post(user, post); assert(user->posts && strlen(user->posts->content) == 249);
    assert(!add_user(NULL, NULL, "demo")); assert(!create_friend(NULL));
    assert(!find_user(user, NULL)); assert(!delete_friend(NULL, "x"));
    assert(!delete_post(NULL)); add_friend(NULL, "x"); add_post(NULL, "x");
    display_posts_by_n(user, 0); display_posts_by_n(user, -1);
    teardown(user); teardown(NULL);
}

static void csv(void)
{
    FILE *file = tmpfile(); assert(file);
    fputs("username,password,friend1,friend2,friend3,post1,post2\n"
          "ALICE,demo,,Bob,,Older,Newer\n"
          "bad\n"
          "alice,demo,,,,Must not replace\n"
          "bob,demo,,,,Hello\n"
          "broken,way-too-long-password,,,,No\n", file);
    rewind(file);
    user_t *users = read_CSV_and_create_users(file, 100);
    fclose(file);
    assert(users && users->next && !users->next->next);
    user_t *alice = find_user(users, "alice");
    assert(alice->friends && !alice->friends->next);
    assert(strcmp(alice->friends->username, "bob") == 0);
    assert(strcmp(alice->posts->content, "Newer") == 0);
    assert(strcmp(alice->posts->next->content, "Older") == 0);
    assert(!find_user(users, "broken"));
    teardown(users);
    assert(!read_CSV_and_create_users(NULL, 1));
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "eof") == 0) {
        user_t *u = add_user(NULL, "demo", "demo");
        add_post(u, "first"); add_post(u, "second");
        display_posts_by_n(u, 1); teardown(u); return 0;
    }
    lists(); boundaries(); csv();
    puts("All linked-list, boundary and CSV tests passed.");
    return 0;
}
