/* globals.c - the ONE place where the shared global data is defined.
 * Frozen. Do not edit (see researchlink.h). */
#include "researchlink.h"

Student     students[MAX_STUDENTS];
Mentor      mentors[MAX_MENTORS];
Program     programs[MAX_PROGRAMS];
Application apps[MAX_APPS];
Paper       papers[MAX_PAPERS];

int studentCount = 0, mentorCount = 0, programCount = 0, appCount = 0, paperCount = 0;
int scoreMatrix[MAX_STUDENTS][MAX_MENTORS];
