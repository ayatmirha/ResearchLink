/* researchlink.h  --  THE CONTRACT  (shared by everyone, FROZEN)
 *
 * Nobody edits this file alone. If you need a change (new field, new
 * function), tell the group chat, agree, ONE person edits it, and everybody
 * copies the new version. After a struct change, delete researchlink.txt.
 *
 * ---------------- TEAM CONVENTIONS (read these!) ----------------
 *  1. IDs are unique and never reused:
 *       students 1001, 1002, ...   mentors 2001, ...
 *       programs 3001, ...         papers  4001, ...
 *     Suggested new id = base + current count  (e.g. 1001 + studentCount).
 *  2. Arrays are never compacted. "Deleting" a student/mentor means
 *     active = 0; the record stays in its slot.
 *  3. Application.studentId/mentorId/programId, Student.matchedMentorId and
 *     Student.readingList store IDs, NOT array indexes.
 *     loginStudent()/loginMentor() return an array INDEX (or -1).
 *     Convert with findStudentById() / findMentorById() / findProgramById().
 *  4. scoreMatrix[s][m] is indexed by array INDEX of student s, mentor m.
 *  5. Keyboard input: use ONLY readInt() and readLine() (common.c).
 *     Never call scanf() directly - it leaves '\n' in the buffer and breaks
 *     everyone else's input.
 *  6. Program.requiredField == "any" means no field restriction.
 *  7. Helper functions that are private to your file must be `static` and
 *     start with your prefix: p1_ / p2_ / p3_. This avoids name clashes
 *     when the files are combined.
 *  8. Only Person 3 / main.c has main(). Do not write another main().
 * ---------------------------------------------------------------
 */
#ifndef RESEARCHLINK_H
#define RESEARCHLINK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS   100
#define MAX_MENTORS    50
#define MAX_PROGRAMS   10
#define MAX_APPS       500
#define MAX_PAPERS     100          /* ADDED (Person 2's resource library) */
#define MAX_READING    10           /* ADDED (reading list size per student) */
#define MAX_TAGS       3
#define NICHE_COUNT    4            /* ADDED: number of research niches (Person 2) */
#define STAGE_COUNT    4            /* ADDED: roadmap stages per niche (Person 2) */
#define LONG_LEN       300          /* ADDED: abstract length */
#define NAME_LEN       50
#define TEXT_LEN       100
#define PIN_ATTEMPTS   3
#define ADMIN_PIN      1234         /* ADDED (used by adminMenu) */
#define DATA_FILE      "researchlink.txt"   /* ADDED (used by saveAll/loadAll) */

/* ---------- Application status ---------- */
typedef enum { PENDING, SHORTLISTED, ACCEPTED, REJECTED } Status;

/* ---------- Shared structs ---------- */
typedef struct {
    int  id;
    char name[NAME_LEN];
    int  pin;
    int  grade;                          /* e.g. 9-12 or semester */
    char field[NAME_LEN];                /* field of interest */
    char tags[MAX_TAGS][NAME_LEN];       /* 3 skill tags */
    int  weeklyHours;
    int  matchedMentorId;                /* mentor ID, -1 if unmatched */
    int  roadmapStage;                   /* 0-4, used by Person 2 */
    int  active;                         /* 0 = deleted */
    int  readingList[MAX_READING];       /* ADDED: paper IDs (Person 2) */
    int  readingCount;                   /* ADDED: how many are used */
    int  niche;                          /* ADDED: 0 = none chosen, 1..NICHE_COUNT */
    int  readStatus[MAX_READING];        /* ADDED: 0 To Read, 1 Reading, 2 Done (parallel to readingList) */
} Student;

typedef struct {
    int  id;
    char name[NAME_LEN];
    int  pin;
    char expertise[NAME_LEN];
    char tags[MAX_TAGS][NAME_LEN];
    int  weeklyHours;
    int  capacity;                       /* max mentees */
    int  currentMentees;
    int  active;
} Mentor;

typedef struct {
    int  id;
    char title[NAME_LEN];
    int  minGrade;
    char requiredField[NAME_LEN];        /* "any" = no restriction */
    int  seats;
    int  seatsTaken;
} Program;

typedef struct {
    int    studentId;
    int    mentorId;
    int    programId;
    int    score;                        /* 0-100 */
    Status status;
} Application;

/* ADDED: research paper for the resource library (Person 2) */
typedef struct {
    int  id;
    char title[TEXT_LEN];
    char field[NAME_LEN];
    char tag[NAME_LEN];
    int  year;
    char authors[TEXT_LEN];              /* ADDED */
    char keywords[TEXT_LEN];             /* ADDED: comma separated words */
    char abstract[LONG_LEN];             /* ADDED */
    int  difficulty;                     /* ADDED: 1 Beginner, 2 Intermediate, 3 Advanced */
    int  niche;                          /* ADDED: 1..NICHE_COUNT (which library it belongs to) */
    int  readCount;                      /* ADDED: how many students marked it Done/Reading */
} Paper;

/* ---------- Global data (defined once in globals.c) ---------- */
extern Student     students[MAX_STUDENTS];
extern Mentor      mentors[MAX_MENTORS];
extern Program     programs[MAX_PROGRAMS];
extern Application apps[MAX_APPS];
extern Paper       papers[MAX_PAPERS];
extern int studentCount, mentorCount, programCount, appCount, paperCount;
extern int scoreMatrix[MAX_STUDENTS][MAX_MENTORS];   /* student-by-mentor scores */

/* ---------- Shared helpers (common.c, frozen) ---------- */
void        readLine(const char *prompt, char *dst, int size); /* safe text input */
int         readInt(const char *prompt);       /* retries until a number; 0 on end of input */
int         findStudentById(int id);           /* array index or -1 (ignores active flag) */
int         findMentorById(int id);
int         findProgramById(int id);
int         findPaperById(int id);
const char *statusName(Status s);              /* "Pending", "Shortlisted", ... */
int         containsNoCase(const char *text, const char *word);  /* 1 if found */
int         compareNoCase(const char *a, const char *b);         /* like strcmp, ignores case */

/* ---------- PERSON 1 (person1.c): access, matching, mentors ---------- */
void registerStudent(void);
void registerMentor(void);
int  loginStudent(void);                 /* returns index or -1 */
int  loginMentor(void);
void updateProfile(int isMentor, int index);
void deleteProfile(int isMentor, int index);
int  calculateMatchScore(int s, int m);
void buildScoreMatrix(void);
void printScoreMatrix(void);
void recommendMentors(int s);
void applyToProgram(int s);
void mentorReviewApplications(int m);
void autoMatch(void);
int  countUnmatched(int i);              /* recursive */

/* ---------- PERSON 2 (person2.c): self-guided track + resource library ---------- */
void selfGuidedMenu(int s);
void showRoadmap(int s);
void updateProgress(int s);
void searchPapers(void);
void recommendPapers(int s);
void readingListMenu(int s);
void addPaper(void);
void libraryMenu(int s);                 /* ADDED: menu that ties the library together */
const char *nicheName(int n);            /* ADDED: 1..NICHE_COUNT -> "Quantum Computing" ... ("None" for 0) */
void showProgressSummary(int s);         /* ADDED: niche + progress bar + %, for mentors/admin to call */

/* ---------- PERSON 3 (person3.c, demo.c, main.c): admin, sorting, reports, files ---------- */
void adminMenu(void);
void sortRecords(void);
void searchById(void);
void searchByName(void);
void showReports(void);
int  saveAll(void);                      /* 1 = ok, 0 = failed */
int  loadAll(void);                      /* 1 = loaded, 0 = no file / bad file */
void addProgram(void);                   /* ADDED: admin creates programs */
void seedDemoData(void);                 /* ADDED: dummy data for testing (demo.c) */

#endif /* RESEARCHLINK_H */
