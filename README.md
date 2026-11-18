# ResearchLink: Research Mentor Matching Platform (C)

**Programming Fundamentals (CT-175) | Complex Computing Activity (CCA) | Fall 2026**
Instructor: Ms. Maham Ashraf | Section E

ResearchLink is a research mentorship and management system written in C. It connects high school students with research mentors and helps students manage their research journey. The program has separate menus for students, mentors and administrators.

Students register, apply to research programs with a fixed number of seats, and get mentors ranked by a match score out of 100. Mentors review the applicants and shortlist, accept or reject them. Students who are not matched yet are not left out: they can follow a four-stage self-guided roadmap in a niche of their choice, read from a research paper library, and move into mentor matching at any time with their progress carried over. The administrator can add programs, search and sort records, run auto-match and view reports. All records are saved to a readable text file.

## Group Members

| Name | Roll Number | Part |
|------|-------------|------|
| Ayzal Mirha | CT-26205 | Person 1: registration, login, profiles, matching |
| Ayat Mirha | CT-26203 | Person 2: self-guided track, resource library |
| Misbah Khan | CT-26216 | Person 3: admin, sorting, searching, reports, file handling |

## Features

**Access and security**
- Main menu with Student, Mentor and Admin sub-menus (switch based)
- Student and mentor registration with input validation on every input
- Login with PIN check (limited attempts)
- Update and delete profile

**Mentor matching**
- Student profile (grade, field, 3 skill tags, weekly hours) and mentor profile (expertise, 3 skill tags, weekly hours, mentee capacity)
- Multiple research programs with fixed seats and eligibility rules (minimum grade, required field)
- Match score out of 100 (field 35 + skill tags 15 + availability 30 + grade 20)
- Student by mentor score matrix printed as a table
- Ranked mentor recommendations for students and ranked applicants for mentors
- Mentors shortlist, accept or reject, with a capacity and seat check
- Auto-match mode with a tie rule
- Application status: Pending, Shortlisted, Accepted, Rejected
- Recursive count of unmatched students

**Self-guided track**
- Unmatched students choose a niche (4 niches)
- 4-stage roadmap: Foundations, Core Concepts, Advanced Topics, Mini Project
- Progress bar with percentage and a next-step suggestion
- Progress carries over when the student applies for a mentor, and mentors can see it before accepting
- Learning links for every stage: a YouTube video search and a web page. The student picks a number from the menu and the program opens the link directly in the web browser

**Research resource library**
- Separate library for each niche (array of paper structs)
- Case-insensitive search by title, author or keyword, with relevance ranking
- Filter by year range and difficulty
- Papers recommended for the student's current roadmap stage
- Personal reading list with To Read, Reading and Done status
- Mentors and Admin can add papers

**Admin, sorting and reports**
- Search by ID and by name
- Sort students, mentors and applications
- Reports on applications, seat usage, mentor workload and unmatched students
- Save and load of all records to a text file, with missing-file and corrupt-file checks

## How to Compile and Run

Open a terminal in the `Source_Code` folder and run:

```
gcc main.c person1.c person2.c person3.c common.c globals.c demo.c -o researchlink
./researchlink
```

On Windows, use `researchlink.exe` instead of `./researchlink`, or open `CCP.dev` in Dev-C++ and press F11.

Data is saved to `researchlink.txt` in the same folder when you choose "Save and exit" (or Admin option 12). Delete this file to start with empty data. Main menu option 9 loads demo data for testing.

**Learning links:** in the self-guided track, Learning resources (option 3) lists a video link and a web page for each roadmap stage. When you type its number, the program opens that link in your default web browser (it uses the Windows `start` command, so this works on Windows; on other systems the program prints the link to copy instead).

**Test logins (after loading demo data with main menu option 9):** student 1001 / PIN 1111, mentor 2001 / PIN 5555, admin PIN 1234.

## Sample Input and Output

Registering a mentor (main menu option 3):

```
Full name: Dr Omar Farooq
PIN (4 digits, 1000-9999): 2222
Field number: 2
Skill tag 1 of 3 (e.g. python): python
Skill tag 2 of 3 (e.g. python): machine learning
Skill tag 3 of 3 (e.g. python): statistics
Weekly hours you can give (1-40): 6
Mentee capacity (1-10): 2
Registered! Your mentor ID is 2002. Remember it with your PIN to log in.
```

Invalid input is rejected and asked again:

```
PIN (4 digits, 1000-9999): 12
Please enter a number from 1000 to 9999.
```

Auto-match (Admin option 10):

```
--- AUTO-MATCH ---
Unmatched students before: 2
Matched Sara Khan -> Dr Omar Farooq (score 85)
Matched Zain Ahmed -> Dr Hina Baig (score 80)
Matches made: 2
Unmatched students now: 0
```

Score matrix (Admin option 11):

```
ID    Student        2001  2002  2003
1001  Ali Raza         55    92*   55
1002  Sara Khan        50    85*   50
1003  Zain Ahmed       40    35    80*
```

## How the Program Works

1. The program starts and loads the saved records from `researchlink.txt`, if the file exists.
2. The main menu offers Student, Mentor and Admin options, and every input is validated and asked again if it is wrong.
3. Students and mentors register and log in with an ID and PIN. Their menus let them manage profiles, apply, review applications, follow the roadmap or use the library.
4. The matching score is recalculated whenever a profile changes, so the score matrix and the rankings are always up to date.
5. The menus repeat until the user goes back or exits, and the records are saved when the program exits.

## Limitations and Future Improvements

- The number of students, mentors, programs, applications and papers is limited by fixed array sizes. In the future we can use dynamic memory allocation (`malloc` and `realloc`) so the lists grow as needed.
- A Graphical User Interface (GUI) with buttons, forms and menus could replace the text-based screens.

## PF Concepts Used

- Variables, data types and operators
- Input and output with formatted tables
- if / else and nested conditions (eligibility, scoring, validation)
- switch (all menus)
- Loops: for, while and do-while (menus, retry on bad input, traversal)
- 1D arrays (IDs, reading lists) and 2D arrays (score matrix, skill tags)
- Strings and string functions (names, PIN checks, search, tag matching)
- Functions with parameters and return values (one per task)
- Pointers and pass by reference (update and input functions)
- Recursion (`countUnmatched`)
- Structures and arrays of structures (Student, Mentor, Program, Application, Paper) and enums (application status)
- Sorting (bubble sort) and searching (linear search)
- File handling (save and load in a text file)
- Error handling (invalid input, duplicates, missing file, corrupt file)

## Folder Structure

```
PF_CCA_Project_GroupNo/
├── Source_Code/
│   ├── main.c
│   ├── person1.c
│   ├── person2.c
│   ├── person3.c
│   ├── common.c
│   ├── globals.c
│   ├── demo.c
│   ├── researchlink.h
│   └── CCP.dev
├── Project_Report.pdf
├── Flowcharts/
├── Screenshots/
└── README.md
```
