//
//\person3.c - PERSON 3: admin menu, search, sort, reports, save/load, addProgram
// *
// * Sorting is DISPLAY-ONLY: we sort an array of indexes, never the real
// * students[]/mentors[]/apps[] arrays. That way scoreMatrix and the indexes
// * returned by login stay valid for everyone else.
// */
#include "researchlink.h"

/* ====================================================================
 *  Private printing helpers
 * ==================================================================== */
static void p3_printStudent(int i)
{
    Student *s = &students[i];

    printf("  [%d] %-20s grade %-2d %-18s %2d hrs/wk  mentor: ",
           s->id, s->name, s->grade, s->field, s->weeklyHours);
    if (s->matchedMentorId < 0) printf("none\n");
    else                        printf("%d\n", s->matchedMentorId);
}

static void p3_printMentor(int i)
{
    Mentor *m = &mentors[i];

    printf("  [%d] %-20s %-18s %2d hrs/wk  mentees: %d/%d\n",
           m->id, m->name, m->expertise, m->weeklyHours,
           m->currentMentees, m->capacity);
}

static void p3_printProgram(int i)
{
    Program *p = &programs[i];

    printf("  [%d] %-26s min grade %-2d field: %-16s seats: %d/%d\n",
           p->id, p->title, p->minGrade, p->requiredField,
           p->seatsTaken, p->seats);
}

static void p3_printApp(int i)
{
    Application *a = &apps[i];
    int s = findStudentById(a->studentId);
    int m = findMentorById(a->mentorId);
    int p = findProgramById(a->programId);

    printf("  %-18s mentor: %-18s program: %-24s score: %3d  %s\n",
           s >= 0 ? students[s].name : "?",
           m >= 0 ? mentors[m].name : "-",
           p >= 0 ? programs[p].title : "-",
           a->score, statusName(a->status));
}

/* ====================================================================
 *  View everything
 * ==================================================================== */
static void p3_viewStudents(void)
{
    int i, n = 0;

    printf("\n--- ALL STUDENTS ---\n");
    for (i = 0; i < studentCount; i++)
        if (students[i].active) { p3_printStudent(i); n++; }
    if (n == 0) printf("  (none)\n");
}

static void p3_viewMentors(void)
{
    int i, n = 0;

    printf("\n--- ALL MENTORS ---\n");
    for (i = 0; i < mentorCount; i++)
        if (mentors[i].active) { p3_printMentor(i); n++; }
    if (n == 0) printf("  (none)\n");
}

static void p3_viewPrograms(void)
{
    int i;

    printf("\n--- ALL PROGRAMS ---\n");
    for (i = 0; i < programCount; i++)
        p3_printProgram(i);
    if (programCount == 0) printf("  (none)\n");
}

static void p3_viewApps(void)
{
    int i;

    printf("\n--- ALL APPLICATIONS ---\n");
    for (i = 0; i < appCount; i++)
        p3_printApp(i);
    if (appCount == 0) printf("  (none)\n");
}

/* ====================================================================
 *  Searching
 * ==================================================================== */
void searchById(void)
{
    int type, id, i, found = 0;

    printf("\n--- SEARCH BY ID ---\n1. Student\n2. Mentor\n3. Program\n");
    type = readInt("Search in> ");
    if (type < 1 || type > 3) { printf("Invalid choice.\n"); return; }
    id = readInt("Enter ID: ");

    if (type == 1) {
        i = findStudentById(id);
        if (i >= 0 && students[i].active) { p3_printStudent(i); found = 1; }
    } else if (type == 2) {
        i = findMentorById(id);
        if (i >= 0 && mentors[i].active) { p3_printMentor(i); found = 1; }
    } else {
        i = findProgramById(id);
        if (i >= 0) { p3_printProgram(i); found = 1; }
    }
    if (!found) printf("No record found with ID %d.\n", id);
}

void searchByName(void)
{
    char key[NAME_LEN];
    int  type, i, found = 0;

    printf("\n--- SEARCH BY NAME ---\n1. Student\n2. Mentor\n3. Program (title)\n");
    type = readInt("Search in> ");
    if (type < 1 || type > 3) { printf("Invalid choice.\n"); return; }
    readLine("Name contains: ", key, NAME_LEN);
    if (key[0] == '\0') { printf("Nothing to search for.\n"); return; }

    if (type == 1) {
        for (i = 0; i < studentCount; i++)
            if (students[i].active && containsNoCase(students[i].name, key)) {
                p3_printStudent(i); found++;
            }
    } else if (type == 2) {
        for (i = 0; i < mentorCount; i++)
            if (mentors[i].active && containsNoCase(mentors[i].name, key)) {
                p3_printMentor(i); found++;
            }
    } else {
        for (i = 0; i < programCount; i++)
            if (containsNoCase(programs[i].title, key)) {
                p3_printProgram(i); found++;
            }
    }
    printf("%d match(es) found.\n", found);
}

/* ====================================================================
 *  Sorting (bubble sort on an index array - real arrays are untouched)
 * ==================================================================== */

/* Returns 1 if record a must come AFTER record b for this sort type. */
static int p3_after(int type, int a, int b)
{
    switch (type) {
        case 1: return compareNoCase(students[a].name, students[b].name) > 0;
        case 2: return students[a].grade > students[b].grade;
        case 3: return students[a].weeklyHours < students[b].weeklyHours;  /* most first */
        case 4: return compareNoCase(mentors[a].name, mentors[b].name) > 0;
        case 5: return mentors[a].capacity < mentors[b].capacity;          /* biggest first */
        case 6: return apps[a].score < apps[b].score;                      /* best first */
    }
    return 0;
}

static void p3_bubbleSort(int type, int idx[], int n)
{
    int i, j, tmp;

    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if (p3_after(type, idx[j], idx[j + 1])) {
                tmp = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = tmp;
            }
}

void sortRecords(void)
{
    int idx[MAX_APPS];
    int choice, n = 0, i;

    printf("\n--- SORT RECORDS ---\n");
    printf("1. Students by name\n2. Students by grade\n3. Students by weekly hours\n");
    printf("4. Mentors by name\n5. Mentors by capacity\n6. Applications by score\n0. Back\n");
    choice = readInt("> ");
    if (choice == 0) return;
    if (choice < 1 || choice > 6) { printf("Invalid choice.\n"); return; }

    if (choice <= 3) {
        for (i = 0; i < studentCount; i++) if (students[i].active) idx[n++] = i;
    } else if (choice <= 5) {
        for (i = 0; i < mentorCount; i++) if (mentors[i].active) idx[n++] = i;
    } else {
        for (i = 0; i < appCount; i++) idx[n++] = i;
    }
    if (n == 0) { printf("Nothing to sort.\n"); return; }

    p3_bubbleSort(choice, idx, n);

    printf("\n--- SORTED RESULT ---\n");
    for (i = 0; i < n; i++) {
        if (choice <= 3)      p3_printStudent(idx[i]);
        else if (choice <= 5) p3_printMentor(idx[i]);
        else                  p3_printApp(idx[i]);
    }
}

/* ====================================================================
 *  Reports
 * ==================================================================== */
void showReports(void)
{
    int  i, activeS = 0, activeM = 0, unmatched = 0, full = 0, best = -1;
    int  statusCount[4] = {0, 0, 0, 0};
    long scoreSum = 0;

    for (i = 0; i < studentCount; i++) {
        if (!students[i].active) continue;
        activeS++;
        if (students[i].matchedMentorId < 0) unmatched++;
    }
    for (i = 0; i < mentorCount; i++) {
        if (!mentors[i].active) continue;
        activeM++;
        if (mentors[i].capacity > 0 && mentors[i].currentMentees >= mentors[i].capacity) full++;
        if (best < 0 || mentors[i].currentMentees > mentors[best].currentMentees) best = i;
    }
    for (i = 0; i < appCount; i++) {
        int st = (int)apps[i].status;
        if (st >= 0 && st <= 3) statusCount[st]++;
        scoreSum += apps[i].score;
    }

    printf("\n========== REPORTS ==========\n");
    printf("Students : %d active (%d records in total)\n", activeS, studentCount);
    printf("Mentors  : %d active (%d records in total)\n", activeM, mentorCount);
    printf("Unmatched students      : %d\n", unmatched);
    printf("Mentors at full capacity: %d\n", full);

    printf("\nApplications (%d in total)\n", appCount);
    printf("  Pending     : %d\n", statusCount[PENDING]);
    printf("  Shortlisted : %d\n", statusCount[SHORTLISTED]);
    printf("  Accepted    : %d\n", statusCount[ACCEPTED]);
    printf("  Rejected    : %d\n", statusCount[REJECTED]);
    if (appCount > 0) printf("  Average score: %.1f\n", (double)scoreSum / appCount);
    else              printf("  Average score: n/a\n");

    printf("\nProgram seat usage\n");
    if (programCount == 0) printf("  (no programs)\n");
    for (i = 0; i < programCount; i++) {
        double pct = programs[i].seats > 0
                   ? 100.0 * programs[i].seatsTaken / programs[i].seats : 0.0;
        printf("  %-26s %d/%d seats (%.0f%%)\n",
               programs[i].title, programs[i].seatsTaken, programs[i].seats, pct);
    }

    printf("\nBusiest mentor: ");
    if (best >= 0 && mentors[best].currentMentees > 0)
        printf("%s (%d mentee(s))\n", mentors[best].name, mentors[best].currentMentees);
    else
        printf("no mentor has mentees yet\n");
    printf("=============================\n");
}

/* ====================================================================
 *  Admin creates a program
 * ==================================================================== */
void addProgram(void)
{
    Program *p;

    if (programCount >= MAX_PROGRAMS) {
        printf("Program list is full (%d).\n", MAX_PROGRAMS);
        return;
    }
    p = &programs[programCount];
    memset(p, 0, sizeof *p);
    p->id = 3001 + programCount;

    readLine("Program title: ", p->title, NAME_LEN);
    p->minGrade = readInt("Minimum grade: ");
    readLine("Required field (type 'any' for no restriction): ", p->requiredField, NAME_LEN);
    p->seats = readInt("Number of seats: ");

    if (p->title[0] == '\0' || p->seats <= 0) {
        printf("Title missing or seats not positive - program NOT added.\n");
        return;
    }
    if (p->requiredField[0] == '\0') strcpy(p->requiredField, "any");
    programCount++;
    printf("Program added with ID %d.\n", p->id);
}

/* ====================================================================
 *  Saving and loading (text file)
 *
 *  Every value is written on its own line as  Label: value  so the file
 *  can be opened in Notepad and read by anybody. Layout:
 *     first line  = RESEARCHLINK DATA FILE
 *     then for each list (students, mentors, programs, applications, papers):
 *     a line with the count, followed by that many records.
 *  When loading, a wrong first line, a wrong count or a missing value
 *  means the file is damaged, and we start with empty data.
 * ==================================================================== */
#define P3_HEADER "RESEARCHLINK DATA FILE"

/* reads one line, takes the text after "Label: " and puts it in value.
 * returns 1 if it worked, 0 if the line is missing or has no ':' */
static int p3_getText(FILE *f, char *value, int size)
{
    char line[500];
    char *colon;
    int i, length;

    if (fgets(line, sizeof line, f) == NULL)
        return 0;
    colon = strchr(line, ':');
    if (colon == NULL)
        return 0;
    colon++;
    if (*colon == ' ')
        colon++;

    length = strlen(colon);
    while (length > 0 && (colon[length - 1] == '\n' || colon[length - 1] == '\r'))
        length--;
    if (length >= size)
        return 0;
    for (i = 0; i < length; i++)
        value[i] = colon[i];
    value[length] = '\0';
    return 1;
}

/* same as above but the value is a whole number */
static int p3_getInt(FILE *f, int *number)
{
    char text[50];

    if (p3_getText(f, text, 50) == 0)
        return 0;
    *number = atoi(text);
    return 1;
}

static void p3_clearAll(void)
{
    memset(students, 0, sizeof students);
    memset(mentors,  0, sizeof mentors);
    memset(programs, 0, sizeof programs);
    memset(apps,     0, sizeof apps);
    memset(papers,   0, sizeof papers);
    studentCount = mentorCount = programCount = appCount = paperCount = 0;
}

/* ---------- writing ---------- */
static void p3_saveStudent(FILE *f, int i)
{
    int k;

    fprintf(f, "[Student]\n");
    fprintf(f, "ID: %d\nName: %s\nPIN: %d\nGrade: %d\nField: %s\n", students[i].id, students[i].name,
            students[i].pin, students[i].grade, students[i].field);
    for (k = 0; k < MAX_TAGS; k++)
        fprintf(f, "Tag: %s\n", students[i].tags[k]);
    fprintf(f, "WeeklyHours: %d\nMentorID: %d\nRoadmapStage: %d\nActive: %d\nNiche: %d\n",
            students[i].weeklyHours, students[i].matchedMentorId, students[i].roadmapStage,
            students[i].active, students[i].niche);
    fprintf(f, "ReadingCount: %d\n", students[i].readingCount);
    for (k = 0; k < students[i].readingCount; k++)
        fprintf(f, "PaperID: %d\nPaperStatus: %d\n", students[i].readingList[k], students[i].readStatus[k]);
}

static void p3_saveMentor(FILE *f, int i)
{
    int k;

    fprintf(f, "[Mentor]\n");
    fprintf(f, "ID: %d\nName: %s\nPIN: %d\nExpertise: %s\n", mentors[i].id, mentors[i].name,
            mentors[i].pin, mentors[i].expertise);
    for (k = 0; k < MAX_TAGS; k++)
        fprintf(f, "Tag: %s\n", mentors[i].tags[k]);
    fprintf(f, "WeeklyHours: %d\nCapacity: %d\nCurrentMentees: %d\nActive: %d\n",
            mentors[i].weeklyHours, mentors[i].capacity, mentors[i].currentMentees, mentors[i].active);
}

static void p3_saveProgram(FILE *f, int i)
{
    fprintf(f, "[Program]\n");
    fprintf(f, "ID: %d\nTitle: %s\nMinGrade: %d\nRequiredField: %s\nSeats: %d\nSeatsTaken: %d\n",
            programs[i].id, programs[i].title, programs[i].minGrade, programs[i].requiredField,
            programs[i].seats, programs[i].seatsTaken);
}

static void p3_saveApp(FILE *f, int i)
{
    fprintf(f, "[Application]\n");
    fprintf(f, "StudentID: %d\nMentorID: %d\nProgramID: %d\nScore: %d\nStatus: %d\n",
            apps[i].studentId, apps[i].mentorId, apps[i].programId, apps[i].score, (int)apps[i].status);
}

static void p3_savePaper(FILE *f, int i)
{
    fprintf(f, "[Paper]\n");
    fprintf(f, "ID: %d\nTitle: %s\nField: %s\nTag: %s\nYear: %d\n", papers[i].id, papers[i].title,
            papers[i].field, papers[i].tag, papers[i].year);
    fprintf(f, "Authors: %s\nKeywords: %s\nAbstract: %s\n", papers[i].authors, papers[i].keywords,
            papers[i].abstract);
    fprintf(f, "Difficulty: %d\nNiche: %d\nReadCount: %d\n", papers[i].difficulty, papers[i].niche,
            papers[i].readCount);
}

int saveAll(void)
{
    FILE *f = fopen(DATA_FILE, "w");
    int i, ok;

    if (f == NULL) return 0;

    fprintf(f, "%s\n", P3_HEADER);
    fprintf(f, "Students: %d\n", studentCount);
    for (i = 0; i < studentCount; i++) p3_saveStudent(f, i);
    fprintf(f, "Mentors: %d\n", mentorCount);
    for (i = 0; i < mentorCount; i++) p3_saveMentor(f, i);
    fprintf(f, "Programs: %d\n", programCount);
    for (i = 0; i < programCount; i++) p3_saveProgram(f, i);
    fprintf(f, "Applications: %d\n", appCount);
    for (i = 0; i < appCount; i++) p3_saveApp(f, i);
    fprintf(f, "Papers: %d\n", paperCount);
    for (i = 0; i < paperCount; i++) p3_savePaper(f, i);

    ok = (ferror(f) == 0);
    if (fclose(f) != 0) ok = 0;
    return ok;
}

/* ---------- reading ---------- */
/* skips the "[Student]" style line */
static int p3_skipTitle(FILE *f)
{
    char line[500];

    return fgets(line, sizeof line, f) != NULL;
}

static int p3_loadStudent(FILE *f, int i)
{
    int k, ok;

    ok = p3_skipTitle(f) && p3_getInt(f, &students[i].id) && p3_getText(f, students[i].name, NAME_LEN)
      && p3_getInt(f, &students[i].pin) && p3_getInt(f, &students[i].grade)
      && p3_getText(f, students[i].field, NAME_LEN);
    for (k = 0; k < MAX_TAGS && ok; k++)
        ok = p3_getText(f, students[i].tags[k], NAME_LEN);
    ok = ok && p3_getInt(f, &students[i].weeklyHours) && p3_getInt(f, &students[i].matchedMentorId)
      && p3_getInt(f, &students[i].roadmapStage) && p3_getInt(f, &students[i].active)
      && p3_getInt(f, &students[i].niche) && p3_getInt(f, &students[i].readingCount);
    if (ok && (students[i].readingCount < 0 || students[i].readingCount > MAX_READING))
        ok = 0;
    for (k = 0; k < students[i].readingCount && ok; k++)
        ok = p3_getInt(f, &students[i].readingList[k]) && p3_getInt(f, &students[i].readStatus[k]);
    return ok;
}

static int p3_loadMentor(FILE *f, int i)
{
    int k, ok;

    ok = p3_skipTitle(f) && p3_getInt(f, &mentors[i].id) && p3_getText(f, mentors[i].name, NAME_LEN)
      && p3_getInt(f, &mentors[i].pin) && p3_getText(f, mentors[i].expertise, NAME_LEN);
    for (k = 0; k < MAX_TAGS && ok; k++)
        ok = p3_getText(f, mentors[i].tags[k], NAME_LEN);
    ok = ok && p3_getInt(f, &mentors[i].weeklyHours) && p3_getInt(f, &mentors[i].capacity)
      && p3_getInt(f, &mentors[i].currentMentees) && p3_getInt(f, &mentors[i].active);
    return ok;
}

static int p3_loadProgram(FILE *f, int i)
{
    return p3_skipTitle(f) && p3_getInt(f, &programs[i].id) && p3_getText(f, programs[i].title, NAME_LEN)
        && p3_getInt(f, &programs[i].minGrade) && p3_getText(f, programs[i].requiredField, NAME_LEN)
        && p3_getInt(f, &programs[i].seats) && p3_getInt(f, &programs[i].seatsTaken);
}

static int p3_loadApp(FILE *f, int i)
{
    int status;

    if (!(p3_skipTitle(f) && p3_getInt(f, &apps[i].studentId) && p3_getInt(f, &apps[i].mentorId)
          && p3_getInt(f, &apps[i].programId) && p3_getInt(f, &apps[i].score) && p3_getInt(f, &status)))
        return 0;
    if (status < PENDING || status > REJECTED)
        return 0;
    apps[i].status = (Status)status;
    return 1;
}

static int p3_loadPaper(FILE *f, int i)
{
    return p3_skipTitle(f) && p3_getInt(f, &papers[i].id) && p3_getText(f, papers[i].title, TEXT_LEN)
        && p3_getText(f, papers[i].field, NAME_LEN) && p3_getText(f, papers[i].tag, NAME_LEN)
        && p3_getInt(f, &papers[i].year) && p3_getText(f, papers[i].authors, TEXT_LEN)
        && p3_getText(f, papers[i].keywords, TEXT_LEN) && p3_getText(f, papers[i].abstract, LONG_LEN)
        && p3_getInt(f, &papers[i].difficulty) && p3_getInt(f, &papers[i].niche)
        && p3_getInt(f, &papers[i].readCount);
}

int loadAll(void)
{
    FILE *f = fopen(DATA_FILE, "r");
    char line[500];
    int i, ok;
    int sc = 0, mc = 0, pc = 0, ac = 0, pa = 0;

    if (f == NULL) return 0;                       /* first run: no file yet */

    ok = (fgets(line, sizeof line, f) != NULL && strncmp(line, P3_HEADER, strlen(P3_HEADER)) == 0);

    ok = ok && p3_getInt(f, &sc) && sc >= 0 && sc <= MAX_STUDENTS;
    for (i = 0; i < sc && ok; i++) ok = p3_loadStudent(f, i);
    ok = ok && p3_getInt(f, &mc) && mc >= 0 && mc <= MAX_MENTORS;
    for (i = 0; i < mc && ok; i++) ok = p3_loadMentor(f, i);
    ok = ok && p3_getInt(f, &pc) && pc >= 0 && pc <= MAX_PROGRAMS;
    for (i = 0; i < pc && ok; i++) ok = p3_loadProgram(f, i);
    ok = ok && p3_getInt(f, &ac) && ac >= 0 && ac <= MAX_APPS;
    for (i = 0; i < ac && ok; i++) ok = p3_loadApp(f, i);
    ok = ok && p3_getInt(f, &pa) && pa >= 0 && pa <= MAX_PAPERS;
    for (i = 0; i < pa && ok; i++) ok = p3_loadPaper(f, i);
    fclose(f);

    if (!ok) {
        p3_clearAll();
        printf("Saved file is damaged - starting with empty data.\n");
        return 0;
    }
    studentCount = sc;
    mentorCount  = mc;
    programCount = pc;
    appCount     = ac;
    paperCount   = pa;
    return 1;
}

/* ====================================================================
 *  Admin menu
 * ==================================================================== */
void adminMenu(void)
{
    int attempt, ok = 0, choice;

    for (attempt = 1; attempt <= PIN_ATTEMPTS && !ok; attempt++) {
        if (readInt("Admin PIN: ") == ADMIN_PIN) ok = 1;
        else printf("Wrong PIN (%d/%d).\n", attempt, PIN_ATTEMPTS);
    }
    if (!ok) { printf("Access denied.\n"); return; }

    do {
        printf("\n--- ADMIN MENU ---\n");
        printf(" 1. View all students\n 2. View all mentors\n 3. View programs\n");
        printf(" 4. View applications\n 5. Search by ID\n 6. Search by name\n");
        printf(" 7. Sort records\n 8. Reports\n 9. Add program\n");
        printf("10. Run auto-match\n11. Show score matrix\n12. Save data now\n13. Add recommended paper\n 0. Back\n");
        choice = readInt("> ");
        switch (choice) {
            case 1:  p3_viewStudents(); break;
            case 2:  p3_viewMentors();  break;
            case 3:  p3_viewPrograms(); break;
            case 4:  p3_viewApps();     break;
            case 5:  searchById();      break;
            case 6:  searchByName();    break;
            case 7:  sortRecords();     break;
            case 8:  showReports();     break;
            case 9:  addProgram();      break;
            case 10: autoMatch();       break;
            case 11: printScoreMatrix(); break;
            case 12:
                if (saveAll()) printf("Data saved to %s.\n", DATA_FILE);
                else           printf("ERROR: could not save data.\n");
                break;
            case 13: addPaper();        break;
            case 0:  break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}
