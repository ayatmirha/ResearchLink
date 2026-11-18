/* main.c - program entry point and the student / mentor menus. */
#include "researchlink.h"

/* ---------- Menus ---------- */
static void studentMenu(void)
{
    int choice, s = loginStudent();

    if (s < 0) { printf("Login failed.\n"); return; }
    do {
        printf("\n--- STUDENT MENU ---\n");
        printf("1. Update profile\n2. Delete profile\n3. Recommended mentors\n");
        printf("4. Apply to program\n5. Self-guided track\n6. Resource library\n0. Back\n");
        choice = readInt("> ");
        switch (choice) {
            case 1: updateProfile(0, s); break;
            case 2: deleteProfile(0, s); return;
            case 3: recommendMentors(s); break;
            case 4: applyToProgram(s);   break;
            case 5: selfGuidedMenu(s);   break;
            case 6: libraryMenu(s);      break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/* NOTE: the original skeleton was cut off inside this menu, so options 3 and
 * 0 are a best guess. Adjust together with Person 1. */
static void mentorMenu(void)
{
    int choice, m = loginMentor();

    if (m < 0) { printf("Login failed.\n"); return; }
    do {
        printf("\n--- MENTOR MENU ---\n");
       printf("1. Update profile\n2. Delete profile\n3. Review applications\n4. Add recommended paper\n0. Back\n");
        choice = readInt("> ");
        switch (choice) {
            case 1: updateProfile(1, m);          break;
            case 2: deleteProfile(1, m);          return;
            case 3: mentorReviewApplications(m);  break;
            case 4: addPaper();                   break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/* ---------- Program start ---------- */
int main(void)
{
    int choice;

    if (loadAll()) {
        buildScoreMatrix();
        printf("Saved data loaded: %d students, %d mentors, %d applications.\n",
               studentCount, mentorCount, appCount);
    } else {
        printf("No saved data - starting empty.\n");
    }

    do {
        printf("\n===== RESEARCHLINK =====\n");
        printf("1. Student: register\n2. Student: login\n");
        printf("3. Mentor: register\n4. Mentor: login\n");
        printf("5. Admin\n");
        printf("9. Load demo data (testing only)\n");
        printf("0. Save and exit\n");
        choice = readInt("> ");
        switch (choice) {
            case 1: registerStudent(); break;
            case 2: studentMenu();     break;
            case 3: registerMentor();  break;
            case 4: mentorMenu();      break;
            case 5: adminMenu();       break;
            case 9: seedDemoData(); buildScoreMatrix(); break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    if (saveAll()) printf("Data saved. Goodbye!\n");
    else           printf("WARNING: could not save data to %s.\n", DATA_FILE);
    return 0;
}
