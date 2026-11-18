/* demo.c - dummy data so every teammate can test without waiting for the others.
 * Menu option 9 in main.c calls seedDemoData(). Remove option 9 before handing in.
 *
 * Demo logins:  student 1001 pin 1111 ... 1004 pin 4444
 *               mentor  2001 pin 5555 ... 2003 pin 7777
 *               admin   pin 1234
 */
#include "researchlink.h"

static void d_set(char *dst, int size, const char *src)
{
    snprintf(dst, (size_t)size, "%s", src);
}

static void d_student(const char *name, int pin, int grade, const char *field,
                      const char *t1, const char *t2, const char *t3,
                      int hours, int mentorId)
{
    Student *s = &students[studentCount];

    memset(s, 0, sizeof *s);
    s->id = 1001 + studentCount;
    d_set(s->name, NAME_LEN, name);
    s->pin = pin;
    s->grade = grade;
    d_set(s->field, NAME_LEN, field);
    d_set(s->tags[0], NAME_LEN, t1);
    d_set(s->tags[1], NAME_LEN, t2);
    d_set(s->tags[2], NAME_LEN, t3);
    s->weeklyHours = hours;
    s->matchedMentorId = mentorId;
    s->active = 1;
    studentCount++;
}

static void d_mentor(const char *name, int pin, const char *expertise,
                     const char *t1, const char *t2, const char *t3,
                     int hours, int capacity, int mentees)
{
    Mentor *m = &mentors[mentorCount];

    memset(m, 0, sizeof *m);
    m->id = 2001 + mentorCount;
    d_set(m->name, NAME_LEN, name);
    m->pin = pin;
    d_set(m->expertise, NAME_LEN, expertise);
    d_set(m->tags[0], NAME_LEN, t1);
    d_set(m->tags[1], NAME_LEN, t2);
    d_set(m->tags[2], NAME_LEN, t3);
    m->weeklyHours = hours;
    m->capacity = capacity;
    m->currentMentees = mentees;
    m->active = 1;
    mentorCount++;
}

static void d_program(const char *title, int minGrade, const char *field,
                      int seats, int taken)
{
    Program *p = &programs[programCount];

    memset(p, 0, sizeof *p);
    p->id = 3001 + programCount;
    d_set(p->title, NAME_LEN, title);
    p->minGrade = minGrade;
    d_set(p->requiredField, NAME_LEN, field);
    p->seats = seats;
    p->seatsTaken = taken;
    programCount++;
}

static void d_app(int studentId, int mentorId, int programId, int score, Status st)
{
    Application *a = &apps[appCount++];

    a->studentId = studentId;
    a->mentorId = mentorId;
    a->programId = programId;
    a->score = score;
    a->status = st;
}

static void d_paper(const char *title, const char *field, const char *tag, int year)
{
    Paper *p = &papers[paperCount];

    memset(p, 0, sizeof *p);
    p->id = 4001 + paperCount;
    d_set(p->title, TEXT_LEN, title);
    d_set(p->field, NAME_LEN, field);
    d_set(p->tag, NAME_LEN, tag);
    p->year = year;
    paperCount++;
}

void seedDemoData(void)
{
    if (studentCount || mentorCount || programCount || appCount || paperCount) {
        printf("Data already exists - demo data NOT loaded.\n");
        return;
    }

    d_student("Ayesha Khan",  1111, 11, "Biology",          "genetics", "lab",    "python", 6, 2001);
    d_student("Bilal Ahmed",  2222, 12, "Computer Science", "python",   "ml",     "stats",  8, 2003);
    d_student("Sara Malik",   3333, 10, "Chemistry",        "lab",      "safety", "data",   5, -1);
    d_student("Usman Tariq",  4444, 12, "Physics",          "math",     "python", "optics", 7, 2002);

    d_mentor("Dr. Hina Raza",     5555, "Biology",          "genetics", "lab",    "microscopy", 10, 3, 1);
    d_mentor("Dr. Omar Siddiqui", 6666, "Physics",          "math",     "optics", "python",      8, 2, 1);
    d_mentor("Dr. Zainab Ali",    7777, "Computer Science", "python",   "ml",     "stats",      12, 4, 1);

    d_program("Summer Genomics Lab",      10, "Biology",          5, 1);
    d_program("AI for Science Bootcamp",  11, "Computer Science", 3, 1);

    d_app(1001, 2001, 3001, 85, ACCEPTED);
    d_app(1002, 2003, 3002, 78, ACCEPTED);
    d_app(1003, 2001, 3001, 60, PENDING);
    d_app(1004, 2002, 3002, 92, SHORTLISTED);
    d_app(1003, 2003, 3002, 35, REJECTED);

    d_paper("Introduction to Gene Editing",   "Biology",          "genetics", 2021);
    d_paper("Neural Networks for Beginners",  "Computer Science", "ml",       2022);
    d_paper("Safe Laboratory Practices",      "Chemistry",        "safety",   2019);

    printf("Demo data loaded: %d students, %d mentors, %d programs, %d applications, %d papers.\n",
           studentCount, mentorCount, programCount, appCount, paperCount);
}
