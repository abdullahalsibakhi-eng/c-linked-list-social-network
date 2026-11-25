# Linked-List Social Network in C

A small terminal-based social-network simulation demonstrating singly linked lists, sorted insertion, case-insensitive username lookup, stack operations, and dynamic memory management.

Originally developed as coursework by Abdullah Alsibakhi. This standalone portfolio edition retains the assignment's data structures and function interface and adds input validation, a new demo program, tests, and build instructions. See [ATTRIBUTION.md](ATTRIBUTION.md).

## Features

- Register users in alphabetical order and find them by username.
- Add and remove alphabetically sorted friend names.
- Add posts to a stack and remove the newest post.
- Display posts in pages of three, with an option to continue.
- Load fictional users from a simple CSV file.
- Release allocated users, friend nodes, and posts on exit.

Usernames are normalized from ASCII uppercase to lowercase. Duplicate usernames and friend names are ignored; self-friend links are ignored. Friend lists are one-way lists of names, not reciprocal relationships, and names do not need to correspond to registered users.

## Requirements

A C11 compiler (Clang or GCC) and Make. On macOS, install Apple's Command Line Tools if they are not already installed. Linux is also supported by the source; this version was tested on macOS.

## Build and run

Open a terminal in the extracted project folder:

```sh
make
./build/social-network
```

To start with the included fictional sample:

```sh
./build/social-network examples/users.csv
```

Choose a numbered menu option. Type `0` to exit. End-of-input also exits cleanly. This is an in-memory demo: edits are not saved back to CSV.

## Tests

```sh
make test
make sanitize
```

The tests cover sorted insertion, case-insensitive lookup, duplicates, friend deletion, last-in-first-out posts, maximum input lengths, invalid/null inputs, malformed CSV rows, empty CSV fields, and pagination at end-of-input. The sanitizer target runs the same checks with AddressSanitizer and UndefinedBehaviorSanitizer.

Both test targets and the demo build passed on the development Mac. No code-coverage percentage or exhaustive memory-safety guarantee is claimed.

## Input limits and demo credentials

- Usernames: 1–29 bytes, without whitespace, commas, or double quotes.
- Demo passwords: 1–14 bytes.
- Posts: 1–249 bytes.
- Overlong values are rejected rather than silently truncated.

The inherited user structure contains a plaintext password field. It is retained for coursework compatibility only: **this program is not an authentication system and must not be used with real passwords**. The new demo intentionally does not implement login or pretend to provide secure account access.

## Simple CSV format

```csv
username,password,friend1,friend2,friend3,post1,post2
alice,demo-only,bob,,,Older post,Newer post
```

The first row is a header. Every data row must contain a username, demo password, and three friend columns; those friend columns may be empty. Additional columns are posts, pushed left-to-right, so the last post becomes the newest.

This is deliberately a simple unquoted format, not a general-purpose CSV parser. Quoted values, embedded commas/newlines, invalid names, oversized fields, and oversized rows are unsupported. Invalid rows are skipped. A row must fit in the 4,096-byte input buffer; the demo reads at most 10,000 data rows. The loader stops safely at EOF. Allocation failures may leave a partially loaded dataset; the legacy API has no detailed error result.

## Project layout

- `a3_nodes.h`: user, friend, and post structures inherited from the starter interface
- `a3_functions.h`: function declarations
- `a3_functions.c`: list operations and hardened CSV loading
- `main.c`: standalone interactive demo
- `tests/test_functions.c`: automated checks
- `examples/users.csv`: fictional sample data
- `Makefile`: build, test, sanitizer, and clean commands

## Changes from the assignment copy

Removed the student number, replaced unchecked copies with validated-length copies, made the public header self-contained, handled invalid inputs and EOF, prevented duplicate names, and replaced the supplied CSV-loader implementation with a hardened adaptation. The original assignment files and original dataset were not modified or bundled here.

Before making this coursework-derived repository public, check your course's rules and permission to redistribute starter material. A private repository is an option while that is unresolved. No license has been added because ownership and redistribution permission for the supplied material have not been established.
