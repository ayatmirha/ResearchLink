/* common.c - shared helpers used by everybody. Frozen (see researchlink.h). */
#include "researchlink.h"
#include <ctype.h>

/* Read one line of text (without the newline) into dst. */
void readLine(const char *prompt, char *dst, int size)
{
    printf("%s", prompt);
    if (fgets(dst, size, stdin) == NULL) {
        dst[0] = '\0';
        return;
    }
    dst[strcspn(dst, "\n")] = '\0';
}

/* Ask until the user types a whole number. Returns 0 if input has ended,
 * so every menu loop (where 0 = Back/Exit) stops instead of looping forever. */
int readInt(const char *prompt)
{
    char buf[64];
    int  v;

    for (;;) {
        printf("%s", prompt);
        if (fgets(buf, sizeof buf, stdin) == NULL)
            return 0;
        if (sscanf(buf, "%d", &v) == 1)
            return v;
        printf("Please enter a whole number.\n");
    }
}

int findStudentById(int id)
{
    int i;
    for (i = 0; i < studentCount; i++)
        if (students[i].id == id) return i;
    return -1;
}

int findMentorById(int id)
{
    int i;
    for (i = 0; i < mentorCount; i++)
        if (mentors[i].id == id) return i;
    return -1;
}

int findProgramById(int id)
{
    int i;
    for (i = 0; i < programCount; i++)
        if (programs[i].id == id) return i;
    return -1;
}

int findPaperById(int id)
{
    int i;
    for (i = 0; i < paperCount; i++)
        if (papers[i].id == id) return i;
    return -1;
}

const char *statusName(Status s)
{
    switch (s) {
        case PENDING:     return "Pending";
        case SHORTLISTED: return "Shortlisted";
        case ACCEPTED:    return "Accepted";
        case REJECTED:    return "Rejected";
    }
    return "Unknown";
}

/* Case-insensitive strcmp. */
int compareNoCase(const char *a, const char *b)
{
    while (*a && *b) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0) return d;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

/* 1 if `word` appears anywhere inside `text` (ignoring case), else 0. */
int containsNoCase(const char *text, const char *word)
{
    size_t n = strlen(word), i, j;

    if (n == 0) return 1;
    for (i = 0; text[i] != '\0'; i++) {
        for (j = 0; j < n && text[i + j] != '\0' &&
                    tolower((unsigned char)text[i + j]) == tolower((unsigned char)word[j]); j++)
            ;
        if (j == n) return 1;
    }
    return 0;
}
