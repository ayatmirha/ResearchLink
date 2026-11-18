/* person1.c  (Person 1)
 * Registration, login, update/delete profile, match score,
 * score matrix, mentor recommendations, apply, mentor review,
 * auto-match and the recursive count of unmatched students.
 */
#include "researchlink.h"
#include <ctype.h>

/* match score is out of 100:
 *   field + skill tags : up to 50  (same field 35, each shared tag 5)
 *   availability       : up to 30
 *   grade              : up to 20  */
#define SAME_FIELD_POINTS  35
#define TAG_POINTS          5
#define MAX_HOURS_POINTS   30
#define AUTO_MATCH_MIN     40     /* auto-match skips pairs below this score */
#define TOP_MENTORS         5     /* how many mentors we recommend */

char *p1_fieldNames[4] = {
    "Biology", "Computer Science", "Chemistry", "Physics"
};


/* ================= 1. helpers for typing ================= */

/* asks again and again until the number is between min and max */
int p1_askNumber(char *question, int min, int max)
{
    int number;

    while (1) {
        number = readInt(question);
        if (number >= min && number <= max)
            return number;
        printf("Please enter a number from %d to %d.\n", min, max);
    }
}

/* asks again and again until the text is not empty and fits in 'size' characters.
 * the answer is copied into 'answer' */
void p1_askText(char *question, char *answer, int size)
{
    char typed[200];
    int i, length, onlySpaces;

    while (1) {
        readLine(question, typed, 200);

        onlySpaces = 1;
        for (i = 0; typed[i] != '\0'; i++) {
            if (typed[i] != ' ')
                onlySpaces = 0;
        }
        length = strlen(typed);

        if (onlySpaces == 0 && length < size) {
            strcpy(answer, typed);
            return;
        }
        if (onlySpaces == 1)
            printf("This cannot be empty.\n");
        else
            printf("Too long, use at most %d characters.\n", size - 1);
    }
}

int p1_askPin(void)
{
    return p1_askNumber("PIN (4 digits, 1000-9999): ", 1000, 9999);
}

/* the user picks a field from the list or types their own */
void p1_pickField(char *answer)
{
    int i, choice;

    printf("Choose a field:\n");
    for (i = 0; i < 4; i++)
        printf("  %d. %s\n", i + 1, p1_fieldNames[i]);
    printf("  5. Other (type it)\n");

    choice = p1_askNumber("Field number: ", 1, 5);
    if (choice <= 4)
        strcpy(answer, p1_fieldNames[choice - 1]);
    else
        p1_askText("Type your field: ", answer, NAME_LEN);
}

/* asks for the 3 skill tags */
void p1_askTags(char tags[MAX_TAGS][NAME_LEN])
{
    int i;
    char question[60];

    for (i = 0; i < MAX_TAGS; i++) {
        sprintf(question, "Skill tag %d of %d (e.g. python): ", i + 1, MAX_TAGS);
        p1_askText(question, tags[i], NAME_LEN);
    }
}

/* returns 1 if an active student (isMentor = 0) or mentor (isMentor = 1) already has this name */
int p1_nameExists(int isMentor, char *name)
{
    int i;

    if (isMentor == 1) {
        for (i = 0; i < mentorCount; i++) {
            if (mentors[i].active == 1 && compareNoCase(mentors[i].name, name) == 0)
                return 1;
        }
    } else {
        for (i = 0; i < studentCount; i++) {
            if (students[i].active == 1 && compareNoCase(students[i].name, name) == 0)
                return 1;
        }
    }
    return 0;
}


/* ================= 2. registration and login ================= */

/* We fill the next free slot (students[studentCount]) but the student only
 * counts once studentCount goes up at the end. So if the user cancels
 * halfway, nothing is saved. */
void registerStudent(void)
{
    int n = studentCount;

    if (studentCount >= MAX_STUDENTS) {
        printf("Sorry, the student list is full.\n");
        return;
    }

    printf("\n--- STUDENT REGISTRATION ---\n");

    p1_askText("Full name: ", students[n].name, NAME_LEN);
    if (p1_nameExists(0, students[n].name) == 1) {
        if (readInt("A student with this name already exists. Register anyway? (1 = yes, 0 = no): ") != 1) {
            printf("Registration cancelled.\n");
            return;
        }
    }
    students[n].pin = p1_askPin();
    students[n].grade = p1_askNumber("Grade (9-12): ", 9, 12);
    p1_pickField(students[n].field);
    p1_askTags(students[n].tags);
    students[n].weeklyHours = p1_askNumber("Weekly hours available (1-40): ", 1, 40);

    students[n].id = 1001 + n;
    students[n].matchedMentorId = -1;      /* -1 = no mentor yet */
    students[n].roadmapStage = 0;
    students[n].readingCount = 0;
    students[n].niche = 0;
    students[n].active = 1;

    studentCount++;
    buildScoreMatrix();
    printf("Registered! Your student ID is %d. Remember it with your PIN to log in.\n", students[n].id);
}

void registerMentor(void)
{
    int n = mentorCount;

    if (mentorCount >= MAX_MENTORS) {
        printf("Sorry, the mentor list is full.\n");
        return;
    }

    printf("\n--- MENTOR REGISTRATION ---\n");

    p1_askText("Full name: ", mentors[n].name, NAME_LEN);
    if (p1_nameExists(1, mentors[n].name) == 1) {
        if (readInt("A mentor with this name already exists. Register anyway? (1 = yes, 0 = no): ") != 1) {
            printf("Registration cancelled.\n");
            return;
        }
    }
    mentors[n].pin = p1_askPin();
    printf("Your area of expertise:\n");
    p1_pickField(mentors[n].expertise);
    p1_askTags(mentors[n].tags);
    mentors[n].weeklyHours = p1_askNumber("Weekly hours you can give (1-40): ", 1, 40);
    mentors[n].capacity = p1_askNumber("Mentee capacity (1-10): ", 1, 10);

    mentors[n].id = 2001 + n;
    mentors[n].currentMentees = 0;
    mentors[n].active = 1;

    mentorCount++;
    buildScoreMatrix();
    printf("Registered! Your mentor ID is %d. Remember it with your PIN to log in.\n", mentors[n].id);
}

/* returns the position of the student in students[], or -1 if login fails */
int loginStudent(void)
{
    int id, i, tries, pin;

    id = readInt("Student ID: ");
    i = findStudentById(id);
    if (i < 0 || students[i].active == 0) {      /* a deleted account is treated as not found */
        printf("No account found with that ID.\n");
        return -1;
    }

    for (tries = 1; tries <= PIN_ATTEMPTS; tries++) {
        pin = readInt("PIN: ");
        if (pin == students[i].pin) {
            printf("Welcome, %s!\n", students[i].name);
            return i;
        }
        printf("Wrong PIN. Attempts left: %d\n", PIN_ATTEMPTS - tries);
    }
    printf("Too many wrong attempts.\n");
    return -1;
}

int loginMentor(void)
{
    int id, i, tries, pin;

    id = readInt("Mentor ID: ");
    i = findMentorById(id);
    if (i < 0 || mentors[i].active == 0) {
        printf("No account found with that ID.\n");
        return -1;
    }

    for (tries = 1; tries <= PIN_ATTEMPTS; tries++) {
        pin = readInt("PIN: ");
        if (pin == mentors[i].pin) {
            printf("Welcome, %s!\n", mentors[i].name);
            return i;
        }
        printf("Wrong PIN. Attempts left: %d\n", PIN_ATTEMPTS - tries);
    }
    printf("Too many wrong attempts.\n");
    return -1;
}


/* ================= 3. update and delete profile ================= */

void p1_updateStudent(int i)
{
    int choice;

    do {
        printf("\n--- UPDATE PROFILE: %s ---\n", students[i].name);
        printf("1. Name\n2. PIN\n3. Grade (now %d)\n4. Field (now %s)\n", students[i].grade, students[i].field);
        printf("5. Skill tags (%s, %s, %s)\n6. Weekly hours (now %d)\n0. Done\n",
               students[i].tags[0], students[i].tags[1], students[i].tags[2], students[i].weeklyHours);
        choice = readInt("> ");

        switch (choice) {
        case 1:
            p1_askText("New name: ", students[i].name, NAME_LEN);
            printf("Name updated.\n");
            break;
        case 2:
            if (readInt("Current PIN: ") != students[i].pin) {
                printf("Wrong PIN. PIN not changed.\n");
            } else {
                students[i].pin = p1_askPin();
                printf("PIN updated.\n");
            }
            break;
        case 3:
            students[i].grade = p1_askNumber("New grade (9-12): ", 9, 12);
            printf("Grade updated.\n");
            break;
        case 4:
            p1_pickField(students[i].field);
            printf("Field updated.\n");
            break;
        case 5:
            p1_askTags(students[i].tags);
            printf("Skill tags updated.\n");
            break;
        case 6:
            students[i].weeklyHours = p1_askNumber("New weekly hours (1-40): ", 1, 40);
            printf("Weekly hours updated.\n");
            break;
        case 0:
            break;
        default:
            printf("Invalid choice.\n");
        }

        buildScoreMatrix();          /* scores depend on the profile so refresh them */
    } while (choice != 0);
}

void p1_updateMentor(int i)
{
    int choice, newCapacity;

    do {
        printf("\n--- UPDATE PROFILE: %s ---\n", mentors[i].name);
        printf("1. Name\n2. PIN\n3. Expertise (now %s)\n", mentors[i].expertise);
        printf("4. Skill tags (%s, %s, %s)\n5. Weekly hours (now %d)\n6. Mentee capacity (now %d)\n0. Done\n",
               mentors[i].tags[0], mentors[i].tags[1], mentors[i].tags[2],
               mentors[i].weeklyHours, mentors[i].capacity);
        choice = readInt("> ");

        switch (choice) {
        case 1:
            p1_askText("New name: ", mentors[i].name, NAME_LEN);
            printf("Name updated.\n");
            break;
        case 2:
            if (readInt("Current PIN: ") != mentors[i].pin) {
                printf("Wrong PIN. PIN not changed.\n");
            } else {
                mentors[i].pin = p1_askPin();
                printf("PIN updated.\n");
            }
            break;
        case 3:
            p1_pickField(mentors[i].expertise);
            printf("Expertise updated.\n");
            break;
        case 4:
            p1_askTags(mentors[i].tags);
            printf("Skill tags updated.\n");
            break;
        case 5:
            mentors[i].weeklyHours = p1_askNumber("New weekly hours (1-40): ", 1, 40);
            printf("Weekly hours updated.\n");
            break;
        case 6:
            newCapacity = p1_askNumber("New capacity (1-10): ", 1, 10);
            if (newCapacity < mentors[i].currentMentees) {
                printf("You already have %d mentees, so capacity cannot go below that.\n",
                       mentors[i].currentMentees);
            } else {
                mentors[i].capacity = newCapacity;
                printf("Capacity updated.\n");
            }
            break;
        case 0:
            break;
        default:
            printf("Invalid choice.\n");
        }

        buildScoreMatrix();
    } while (choice != 0);
}

/* isMentor = 0 -> students[index], isMentor = 1 -> mentors[index] */
void updateProfile(int isMentor, int index)
{
    if (isMentor == 1)
        p1_updateMentor(index);
    else
        p1_updateStudent(index);
}

/* an accepted application was using one seat of its program, so give it back */
void p1_freeSeat(int programId)
{
    int p = findProgramById(programId);

    if (p >= 0 && programs[p].seatsTaken > 0)
        programs[p].seatsTaken--;
}

/* "Delete" only sets active = 0, the record stays in the array.
 * Then we fix everything that was connected to it. */
void deleteProfile(int isMentor, int index)
{
    int i, id, m;

    if (readInt("This cannot be undone. Type 1 to delete your profile, 0 to cancel: ") != 1) {
        printf("Delete cancelled.\n");
        return;
    }

    if (isMentor == 0) {
        id = students[index].id;
        students[index].active = 0;

        /* the student's mentor has one mentee less now */
        if (students[index].matchedMentorId >= 0) {
            m = findMentorById(students[index].matchedMentorId);
            if (m >= 0 && mentors[m].currentMentees > 0)
                mentors[m].currentMentees--;
            students[index].matchedMentorId = -1;
        }
        /* close this student's applications and return any seat they held */
        for (i = 0; i < appCount; i++) {
            if (apps[i].studentId == id && apps[i].status != REJECTED) {
                if (apps[i].status == ACCEPTED)
                    p1_freeSeat(apps[i].programId);
                apps[i].status = REJECTED;
            }
        }
    } else {
        id = mentors[index].id;
        mentors[index].active = 0;
        mentors[index].currentMentees = 0;

        /* students of this mentor are unmatched again */
        for (i = 0; i < studentCount; i++) {
            if (students[i].matchedMentorId == id)
                students[i].matchedMentorId = -1;
        }
        /* close the applications that were sent to this mentor */
        for (i = 0; i < appCount; i++) {
            if (apps[i].mentorId == id && apps[i].status != REJECTED) {
                if (apps[i].status == ACCEPTED)
                    p1_freeSeat(apps[i].programId);
                apps[i].status = REJECTED;
            }
        }
    }

    buildScoreMatrix();
    printf("Your profile has been deleted.\n");
}


/* ================= 4. match score ================= */

/* Comparing fields and skill tags.
 * Both texts are made lowercase first. Then they count as "the same" if the
 * shorter one is the beginning of the longer one, so
 *     "chem"  and "Chemistry"            -> same
 *     "python" and "Python 3"            -> same
 *     "physics" and "chemistry"          -> not same
 * Very short texts (less than 3 letters) must be exactly equal,
 * otherwise "ai" would match "airflow". */

/* copies text into small[] in lowercase */
void p1_makeLower(char *text, char *small)
{
    int i;

    for (i = 0; text[i] != '\0' && i < NAME_LEN - 1; i++)
        small[i] = tolower(text[i]);
    small[i] = '\0';
}

/* returns 1 if the two texts are "the same" by the rule above */
int p1_phrasesMatch(char *a, char *b)
{
    char lowerA[NAME_LEN];
    char lowerB[NAME_LEN];
    int lengthA, lengthB, shorter;

    p1_makeLower(a, lowerA);
    p1_makeLower(b, lowerB);
    lengthA = strlen(lowerA);
    lengthB = strlen(lowerB);

    shorter = lengthA;
    if (lengthB < shorter)
        shorter = lengthB;

    if (shorter == 0)
        return 0;
    if (shorter < 3)
        return strcmp(lowerA, lowerB) == 0;
    return strncmp(lowerA, lowerB, shorter) == 0;
}

/* part 1: field. 35 points if the student's field matches the mentor's expertise */
int p1_fieldPoints(int s, int m)
{
    if (p1_phrasesMatch(students[s].field, mentors[m].expertise) == 1)
        return SAME_FIELD_POINTS;
    return 0;
}

/* part 2: skills. 5 points for every student tag that the mentor also has */
int p1_tagPoints(int s, int m)
{
    int i, j, shared = 0;

    for (i = 0; i < MAX_TAGS; i++) {
        for (j = 0; j < MAX_TAGS; j++) {
            if (p1_phrasesMatch(students[s].tags[i], mentors[m].tags[j]) == 1) {
                shared++;
                break;               /* one student tag is counted only once */
            }
        }
    }
    return shared * TAG_POINTS;
}

/* part 3: availability. Full 30 if the mentor has at least as many hours as
 * the student. If not, a part of it: mentor 4 h, student 8 h -> 15 points. */
int p1_hoursPoints(int s, int m)
{
    int studentHours = students[s].weeklyHours;
    int mentorHours = mentors[m].weeklyHours;

    if (studentHours <= 0)
        return 0;
    if (mentorHours >= studentHours)
        return MAX_HOURS_POINTS;
    return mentorHours * MAX_HOURS_POINTS / studentHours;
}

/* part 4: grade. Higher grade students get more points */
int p1_gradePoints(int s)
{
    int grade = students[s].grade;

    if (grade >= 12) return 20;
    if (grade == 11) return 15;
    if (grade == 10) return 10;
    return 5;
}

/* score from 0 to 100 for student at position s and mentor at position m */
int calculateMatchScore(int s, int m)
{
    int score;

    if (s < 0 || s >= studentCount || m < 0 || m >= mentorCount)
        return 0;
    if (students[s].active == 0 || mentors[m].active == 0)
        return 0;

    score = p1_fieldPoints(s, m) + p1_tagPoints(s, m) + p1_hoursPoints(s, m) + p1_gradePoints(s);
    if (score > 100)
        score = 100;
    return score;
}

/* shows how the score was made */
void p1_printBreakdown(int s, int m)
{
    printf("  Why %s scores %d with %s:\n", students[s].name, calculateMatchScore(s, m), mentors[m].name);
    printf("    Field match   : %2d / %d  (%s vs %s)\n", p1_fieldPoints(s, m), SAME_FIELD_POINTS,
           students[s].field, mentors[m].expertise);
    printf("    Shared skills : %2d / %d\n", p1_tagPoints(s, m), TAG_POINTS * MAX_TAGS);
    printf("    Availability  : %2d / %d  (student %d h, mentor %d h)\n", p1_hoursPoints(s, m),
           MAX_HOURS_POINTS, students[s].weeklyHours, mentors[m].weeklyHours);
    printf("    Grade level   : %2d / 20  (grade %d)\n", p1_gradePoints(s), students[s].grade);
}


/* ================= score matrix (2D array) ================= */

/* fills scoreMatrix with the score of every student and mentor pair */
void buildScoreMatrix(void)
{
    int s, m;

    for (s = 0; s < studentCount; s++) {
        for (m = 0; m < mentorCount; m++)
            scoreMatrix[s][m] = calculateMatchScore(s, m);
    }
}

/* prints the matrix as a table, rows = students, columns = mentors.
 * A * marks the best mentor of each student. */
void printScoreMatrix(void)
{
    int s, m, best, activeStudents = 0, activeMentors = 0;

    buildScoreMatrix();
    for (s = 0; s < studentCount; s++) {
        if (students[s].active == 1)
            activeStudents++;
    }
    for (m = 0; m < mentorCount; m++) {
        if (mentors[m].active == 1)
            activeMentors++;
    }
    if (activeStudents == 0 || activeMentors == 0) {
        printf("The matrix needs at least one student and one mentor.\n");
        return;
    }

    printf("\n=== SCORE MATRIX (rows = students, columns = mentor IDs, * = best mentor) ===\n");
    printf("%-5s %-14s", "ID", "Student");
    for (m = 0; m < mentorCount; m++) {
        if (mentors[m].active == 1)
            printf(" %4d ", mentors[m].id);
    }
    printf("\n");

    for (s = 0; s < studentCount; s++) {
        if (students[s].active == 0)
            continue;

        /* find the best mentor of this student */
        best = -1;
        for (m = 0; m < mentorCount; m++) {
            if (mentors[m].active == 0)
                continue;
            if (best < 0 || scoreMatrix[s][m] > scoreMatrix[s][best])
                best = m;
        }

        printf("%-5d %-14.14s", students[s].id, students[s].name);
        for (m = 0; m < mentorCount; m++) {
            if (mentors[m].active == 0)
                continue;
            printf(" %4d", scoreMatrix[s][m]);
            if (m == best && scoreMatrix[s][m] > 0)
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
    }

    printf("\nMentors: ");
    for (m = 0; m < mentorCount; m++) {
        if (mentors[m].active == 1)
            printf("%d = %s   ", mentors[m].id, mentors[m].name);
    }
    printf("\n");
}


/* ================= ranked mentor recommendations ================= */

/* how many more mentees this mentor can take */
int p1_freeSpots(int m)
{
    return mentors[m].capacity - mentors[m].currentMentees;
}

/* Tie rule. For student s, should mentor a be above mentor b?
 * Higher score wins. If equal, more free spots wins.
 * If still equal, the lower mentor ID wins. */
int p1_mentorBetter(int s, int a, int b)
{
    if (scoreMatrix[s][a] != scoreMatrix[s][b])
        return scoreMatrix[s][a] > scoreMatrix[s][b];
    if (p1_freeSpots(a) != p1_freeSpots(b))
        return p1_freeSpots(a) > p1_freeSpots(b);
    return mentors[a].id < mentors[b].id;
}

/* puts the positions of all active mentors in order[], best first (bubble sort).
 * returns how many mentors there are */
int p1_rankMentors(int s, int order[])
{
    int count = 0, i, j, temp;

    for (i = 0; i < mentorCount; i++) {
        if (mentors[i].active == 1) {
            order[count] = i;
            count++;
        }
    }

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - 1 - i; j++) {
            if (p1_mentorBetter(s, order[j + 1], order[j]) == 1) {
                temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }
    return count;
}

/* prints the first 'limit' mentors of the ranking */
void p1_printRanking(int s, int order[], int count, int limit)
{
    int i, m;

    printf("%-5s %-6s %-18s %-17s %-6s %s\n", "Rank", "ID", "Mentor", "Expertise", "Score", "Free spots");
    for (i = 0; i < count && i < limit; i++) {
        m = order[i];
        printf("%-5d %-6d %-18.18s %-17.17s %-6d ", i + 1, mentors[m].id, mentors[m].name,
               mentors[m].expertise, scoreMatrix[s][m]);
        if (p1_freeSpots(m) <= 0)
            printf("FULL\n");
        else
            printf("%d\n", p1_freeSpots(m));
    }
}

/* shows the best mentors for the student at position s */
void recommendMentors(int s)
{
    int order[MAX_MENTORS], count, id, m;

    buildScoreMatrix();
    count = p1_rankMentors(s, order);
    if (count == 0) {
        printf("There are no mentors yet.\n");
        return;
    }

    printf("\n--- TOP MENTORS FOR %s ---\n", students[s].name);
    p1_printRanking(s, order, count, TOP_MENTORS);

    id = readInt("Enter a mentor ID to see why it scored that way (0 = skip): ");
    if (id != 0) {
        m = findMentorById(id);
        if (m < 0 || mentors[m].active == 0)
            printf("No mentor with that ID.\n");
        else
            p1_printBreakdown(s, m);
    }
}


/* ================= 5. apply to a program ================= */

void applyToProgram(int s)
{
    int i, programId, p, mentorId, m, count, order[MAX_MENTORS];

    if (appCount >= MAX_APPS) {
        printf("Sorry, no more applications can be stored.\n");
        return;
    }
    if (programCount == 0) {
        printf("No programs are open yet. Ask the admin to add one.\n");
        return;
    }
    if (students[s].matchedMentorId >= 0) {
        printf("You already have a mentor, so you cannot apply again.\n");
        return;
    }

    printf("\n--- OPEN PROGRAMS ---\n");
    printf("%-6s %-26s %-10s %-17s %s\n", "ID", "Title", "Min grade", "Field", "Seats left");
    for (i = 0; i < programCount; i++) {
        printf("%-6d %-26.26s %-10d %-17.17s %d\n", programs[i].id, programs[i].title, programs[i].minGrade,
               programs[i].requiredField, programs[i].seats - programs[i].seatsTaken);
    }

    programId = readInt("Program ID to apply to (0 = cancel): ");
    if (programId == 0)
        return;
    p = findProgramById(programId);
    if (p < 0) {
        printf("No program with that ID.\n");
        return;
    }

    /* eligibility checks */
    if (students[s].grade < programs[p].minGrade) {
        printf("Not eligible: this program needs grade %d or above (you are in grade %d).\n",
               programs[p].minGrade, students[s].grade);
        return;
    }
    if (compareNoCase(programs[p].requiredField, "any") != 0 &&
        compareNoCase(programs[p].requiredField, students[s].field) != 0) {
        printf("Not eligible: this program is only for %s students.\n", programs[p].requiredField);
        return;
    }
    if (programs[p].seatsTaken >= programs[p].seats) {
        printf("Sorry, this program is full. Try the Self-guided track (option 5) and apply again later.\n");
        return;
    }
    for (i = 0; i < appCount; i++) {
        if (apps[i].studentId == students[s].id && apps[i].programId == programs[p].id &&
            apps[i].status != REJECTED) {
            printf("You already applied to this program (status: %s).\n", statusName(apps[i].status));
            return;
        }
    }

    /* choose a mentor */
    buildScoreMatrix();
    count = p1_rankMentors(s, order);
    if (count == 0) {
        printf("There are no mentors yet.\n");
        return;
    }
    printf("\nYou are eligible! Pick a mentor (best matches first):\n");
    p1_printRanking(s, order, count, count);

    mentorId = readInt("Mentor ID (0 = cancel): ");
    if (mentorId == 0)
        return;
    m = findMentorById(mentorId);
    if (m < 0 || mentors[m].active == 0) {
        printf("No mentor with that ID.\n");
        return;
    }
    if (p1_freeSpots(m) <= 0) {
        printf("That mentor is full. Please choose another mentor.\n");
        return;
    }

    /* save the application */
    apps[appCount].studentId = students[s].id;
    apps[appCount].mentorId = mentors[m].id;
    apps[appCount].programId = programs[p].id;
    apps[appCount].score = scoreMatrix[s][m];
    apps[appCount].status = PENDING;
    appCount++;

    printf("Application sent to %s for \"%s\" (match score %d). Status: Pending.\n",
           mentors[m].name, programs[p].title, scoreMatrix[s][m]);
}


/* ================= mentor review ================= */

/* puts the applications that this mentor can still decide on into list[]
 * (Pending or Shortlisted, and the student must still exist), best score first.
 * returns how many there are */
int p1_collectApps(int m, int list[])
{
    int i, j, temp, count = 0, s, a, b;

    for (i = 0; i < appCount; i++) {
        if (apps[i].mentorId != mentors[m].id)
            continue;
        if (apps[i].status != PENDING && apps[i].status != SHORTLISTED)
            continue;
        s = findStudentById(apps[i].studentId);
        if (s < 0 || students[s].active == 0)
            continue;
        list[count] = i;
        count++;
    }

    /* bubble sort: higher score first, if equal then lower student ID first */
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - 1 - i; j++) {
            a = list[j];
            b = list[j + 1];
            if (apps[b].score > apps[a].score ||
                (apps[b].score == apps[a].score && apps[b].studentId < apps[a].studentId)) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
    return count;
}

/* when a student is accepted somewhere, their other open applications are closed */
void p1_closeOtherApps(int studentId, int keepApp)
{
    int i, closed = 0;

    for (i = 0; i < appCount; i++) {
        if (i == keepApp || apps[i].studentId != studentId)
            continue;
        if (apps[i].status == PENDING || apps[i].status == SHORTLISTED) {
            apps[i].status = REJECTED;
            closed++;
        }
    }
    if (closed > 0)
        printf("(%d other application(s) of this student were closed.)\n", closed);
}

/* matches student at position s with mentor at position m */
void p1_setMatch(int s, int m)
{
    students[s].matchedMentorId = mentors[m].id;
    mentors[m].currentMentees++;
}

/* accepts application number a. Returns 1 if it worked, 0 if something stopped it
 * (a message is printed). Checks mentor capacity and program seats first. */
int p1_acceptApp(int a)
{
    int s = findStudentById(apps[a].studentId);
    int m = findMentorById(apps[a].mentorId);
    int p = findProgramById(apps[a].programId);

    if (s < 0 || m < 0 || p < 0) {
        printf("This application points to a missing record.\n");
        return 0;
    }
    if (students[s].matchedMentorId >= 0) {
        printf("This student already has a mentor.\n");
        return 0;
    }
    if (p1_freeSpots(m) <= 0) {
        printf("You are at full capacity (%d/%d mentees), so you cannot accept more.\n",
               mentors[m].currentMentees, mentors[m].capacity);
        return 0;
    }
    if (programs[p].seatsTaken >= programs[p].seats) {
        printf("The program \"%s\" has no seats left.\n", programs[p].title);
        return 0;
    }

    apps[a].status = ACCEPTED;
    programs[p].seatsTaken++;
    p1_setMatch(s, m);
    p1_closeOtherApps(students[s].id, a);
    return 1;
}

/* mentor at position m shortlists, accepts or rejects the applications sent to them */
void mentorReviewApplications(int m)
{
    int list[MAX_APPS], count, i, s, a, pick, action;

    while (1) {
        count = p1_collectApps(m, list);
        if (count == 0) {
            printf("No applications are waiting for your review.\n");
            return;
        }

        printf("\n--- APPLICATIONS FOR %s (capacity %d/%d) ---\n",
               mentors[m].name, mentors[m].currentMentees, mentors[m].capacity);
        for (i = 0; i < count; i++) {
            a = list[i];
            s = findStudentById(apps[a].studentId);
            printf("%d. %-16.16s (ID %d, grade %d, %s)  Score %d  [%s]\n", i + 1, students[s].name,
                   students[s].id, students[s].grade, students[s].field, apps[a].score,
                   statusName(apps[a].status));
            showProgressSummary(s);              /* Person 2's function, shows roadmap progress */
        }

        pick = readInt("Application number to review (0 = back): ");
        if (pick == 0)
            return;
        if (pick < 1 || pick > count) {
            printf("Invalid number.\n");
            continue;
        }
        a = list[pick - 1];

        printf("1. Shortlist\n2. Accept\n3. Reject\n0. Cancel\n");
        action = readInt("> ");
        switch (action) {
        case 1:
            if (apps[a].status == SHORTLISTED) {
                printf("Already shortlisted.\n");
            } else {
                apps[a].status = SHORTLISTED;
                printf("Student shortlisted.\n");
            }
            break;
        case 2:
            if (p1_acceptApp(a) == 1)
                printf("Accepted! The student is now your mentee.\n");
            break;
        case 3:
            apps[a].status = REJECTED;
            printf("Application rejected.\n");
            break;
        case 0:
            break;
        default:
            printf("Invalid choice.\n");
        }
    }
}


/* ================= auto-match =================
 * Again and again: find the best pair (unmatched student + mentor with a free
 * spot) and match them. Stop when no pair has at least AUTO_MATCH_MIN points.
 * Tie rule: higher score, then the mentor with more free spots,
 * then the student who comes first in the list. */
void autoMatch(void)
{
    int s, m, a, p, bestS, bestM, better, found, matched = 0;

    buildScoreMatrix();
    printf("\n--- AUTO-MATCH ---\n");
    printf("Unmatched students before: %d\n", countUnmatched(0));
    printf("Rule: best score first (minimum %d). Tie: more free mentor spots, then lower student ID.\n",
           AUTO_MATCH_MIN);

    while (1) {
        bestS = -1;
        bestM = -1;

        for (s = 0; s < studentCount; s++) {
            if (students[s].active == 0 || students[s].matchedMentorId >= 0)
                continue;
            for (m = 0; m < mentorCount; m++) {
                if (mentors[m].active == 0 || p1_freeSpots(m) <= 0)
                    continue;
                if (scoreMatrix[s][m] < AUTO_MATCH_MIN)
                    continue;

                better = 0;
                if (bestS < 0)
                    better = 1;                                  /* first pair we see */
                else if (scoreMatrix[s][m] > scoreMatrix[bestS][bestM])
                    better = 1;                                  /* higher score */
                else if (scoreMatrix[s][m] == scoreMatrix[bestS][bestM] &&
                         p1_freeSpots(m) > p1_freeSpots(bestM))
                    better = 1;                                  /* tie: more free spots */
                if (better == 1) {
                    bestS = s;
                    bestM = m;
                }
            }
        }
        if (bestS < 0)
            break;                                   /* no good pair left */

        /* did this student already apply to this mentor? then accept that application */
        found = -1;
        for (a = 0; a < appCount; a++) {
            if (apps[a].studentId == students[bestS].id && apps[a].mentorId == mentors[bestM].id &&
                (apps[a].status == PENDING || apps[a].status == SHORTLISTED)) {
                found = a;
                break;
            }
        }
        p = -1;
        if (found >= 0)
            p = findProgramById(apps[found].programId);

        if (found >= 0 && p >= 0 && programs[p].seatsTaken < programs[p].seats)
            p1_acceptApp(found);
        else
            p1_setMatch(bestS, bestM);

        printf("Matched %s -> %s (score %d)\n", students[bestS].name, mentors[bestM].name,
               scoreMatrix[bestS][bestM]);
        matched++;
    }

    printf("Matches made: %d\n", matched);
    printf("Unmatched students now: %d\n", countUnmatched(0));
    if (countUnmatched(0) > 0)
        printf("(Unmatched students can use the Self-guided track and be matched later.)\n");
}


/* ================= 6. recursion ================= */

/* Counts the unmatched active students from position i to the end.
 *   base case: i is past the last student -> 0
 *   otherwise: (1 if student i is unmatched, else 0) + count for the rest */
int countUnmatched(int i)
{
    int here = 0;

    if (i >= studentCount)
        return 0;

    if (students[i].active == 1 && students[i].matchedMentorId < 0)
        here = 1;
    return here + countUnmatched(i + 1);
}
