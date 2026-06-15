#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_LINE 1001

int keep_digits     = 0;
int keep_punct      = 0;
int to_lowercase    = 0;
int to_uppercase    = 0;
int replace_mode    = 0;
char *replace_str   = NULL;
char **prefixes     = NULL;
int  prefix_count   = 0;

void filter_line(char *line)
{
    int read_pos  = 0;
    int write_pos = 0;

    while (line[read_pos] != '\0') {
        char c = line[read_pos];

        int keep = 0;
        if (isalpha(c))               keep = 1;
        if (c == ' ')                 keep = 1;
        if (keep_digits && isdigit(c)) keep = 1;
        if (keep_punct  && ispunct(c)) keep = 1;

        if (keep) {
            line[write_pos] = c;
            write_pos++;
        }

        read_pos++;
    }

    line[write_pos] = '\0';
}

void convert_case(char *line)
{
    int i = 0;
    while (line[i] != '\0') {
        if (to_lowercase) line[i] = tolower(line[i]);
        if (to_uppercase) line[i] = toupper(line[i]);
        i++;
    }
}

int word_has_prefix_sensitive(char *word, int word_len)
{
    int i = 0;
    while (i < prefix_count) {
        int prefix_len = strlen(prefixes[i]);
        if (prefix_len <= word_len) {
            if (strncmp(word, prefixes[i], prefix_len) == 0) {
                return 1;
            }
        }
        i++;
    }
    return 0;
}

int word_has_prefix_insensitive(char *word, int word_len)
{
    int i = 0;
    while (i < prefix_count) {
        int prefix_len = strlen(prefixes[i]);
        if (prefix_len <= word_len) {
            if (strncasecmp(word, prefixes[i], prefix_len) == 0) {
                return 1;
            }
        }
        i++;
    }
    return 0;
}

void overwrite_word(char *line, int word_start, int word_len)
{
    int replace_len = strlen(replace_str);
    int chars_to_replace = word_len < replace_len ? word_len : replace_len;

    int i = 0;
    while (i < chars_to_replace) {
        char new_char      = replace_str[i];
        char original_char = line[word_start + i];

        if (replace_mode == 'r') {
            line[word_start + i] = new_char;
        } else {
            if (isalpha(new_char) && isalpha(original_char)) {
                if (isupper(original_char))
                    line[word_start + i] = toupper(new_char);
                else
                    line[word_start + i] = tolower(new_char);
            } else {
                line[word_start + i] = new_char;
            }
        }

        i++;
    }
}

void replace_words(char *line)
{
    int i   = 0;
    int len = strlen(line);

    while (i < len) {

        if (!isalnum(line[i])) {
            i++;
            continue;
        }

        int word_start = i;
        while (i < len && isalnum(line[i])) {
            i++;
        }
        int word_len = i - word_start;

        int should_replace = 0;

        if (prefix_count == 0) {
            should_replace = 1;
        } else if (replace_mode == 'r') {
            should_replace = word_has_prefix_sensitive(line + word_start, word_len);
        } else {
            should_replace = word_has_prefix_insensitive(line + word_start, word_len);
        }

        if (should_replace) {
            overwrite_word(line, word_start, word_len);
        }
    }
}

void strip_newline(char *line)
{
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }
}

int main(int argc, char *argv[])
{
    int option;
    while ((option = getopt(argc, argv, ":dpluR:r:")) != -1) {
        if      (option == 'd') keep_digits   = 1;
        else if (option == 'p') keep_punct    = 1;
        else if (option == 'l') to_lowercase  = 1;
        else if (option == 'u') to_uppercase  = 1;
        else if (option == 'r') { replace_mode = 'r'; replace_str = optarg; }
        else if (option == 'R') { replace_mode = 'R'; replace_str = optarg; }
        else if (option == ':') return 2;
        else                    return 1;
    }

    prefix_count = argc - optind;
    prefixes     = argv + optind;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        strip_newline(line);

        if (strlen(line) == 0) break;

        filter_line(line);
        convert_case(line);
        if (replace_mode != 0) replace_words(line);

        printf("%s\n", line);
    }

    return 0;
}