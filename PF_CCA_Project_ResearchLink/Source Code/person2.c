/* person2.c  (Person 2)
 * Self-guided track (niche, roadmap, progress), learning links
 * and the research resource library.
 *
 * Team rules: typing is only done with readInt() and readLine(), and
 * functions take the position in the array, not the ID.
 */
#include "researchlink.h"

/* ================= 1. roadmap data =================
 * 4 niches x 4 stages. Stage names are the same for every niche, topics differ.
 * students[s].niche = 0 means no niche chosen yet, 1 to 4 are the niches below.
 * students[s].roadmapStage = number of stages already finished (0 to 4). */
char *p2_nicheNames[NICHE_COUNT] = {
    "Quantum Computing",
    "Biology / Genomics",
    "AI / Machine Learning",
    "Chemistry / Physics"
};

char *p2_stageNames[STAGE_COUNT] = {
    "Foundations", "Core Concepts", "Advanced Topics", "Mini Project"
};

char *p2_stageTopics[NICHE_COUNT][STAGE_COUNT] = {
    { "Complex numbers, vectors, qubits and the Bloch sphere",
      "Quantum gates, superposition, entanglement, measurement",
      "Grover's search, Deutsch-Jozsa, intro to VQE",
      "Simulate a 2-qubit circuit and report the results" },
    { "Cell biology, DNA, RNA and the central dogma",
      "Genes, mutations and sequencing basics",
      "CRISPR and simple bioinformatics tools",
      "Analyse a gene sequence (for example GC content)" },
    { "Python basics, statistics and probability",
      "Regression, classification, train/test split",
      "Neural networks, overfitting, evaluation metrics",
      "Train a small classifier on a public dataset" },
    { "Atomic structure, units and error analysis",
      "Thermodynamics, electrochemistry, optics",
      "Spectroscopy and experiment design",
      "Design, run and write up a small experiment" }
};

/* Name of niche n (1 to 4). Anything else gives "None". Other files use this too. */
const char *nicheName(int n)
{
    if (n < 1 || n > NICHE_COUNT)
        return "None";
    return p2_nicheNames[n - 1];
}

/* Draw a progress bar 20 characters wide, for example  [#####---------------] 25% */
void p2_printBar(int percent)
{
    int i, filled = percent / 5;            /* 100% = 20 characters */

    printf("[");
    for (i = 0; i < 20; i++) {
        if (i < filled)
            printf("#");
        else
            printf("-");
    }
    printf("] %d%%", percent);
}

/* One line showing the student's niche and progress.
 * Person 1 calls this in the mentor's review screen. */
void showProgressSummary(int s)
{
    int percent = students[s].roadmapStage * 100 / STAGE_COUNT;

    if (students[s].niche == 0) {
        printf("  Self-guided track: not started\n");
        return;
    }
    printf("  Self-guided track: %s  ", nicheName(students[s].niche));
    p2_printBar(percent);
    printf("  (%d of %d stages done)\n", students[s].roadmapStage, STAGE_COUNT);
}

/* Let the student pick a niche. Returns 1 if one was chosen, 0 if cancelled. */
int p2_chooseNiche(int s)
{
    int i, choice;

    printf("\n--- CHOOSE YOUR NICHE ---\n");
    for (i = 0; i < NICHE_COUNT; i++)
        printf("%d. %s\n", i + 1, p2_nicheNames[i]);
    printf("0. Cancel\n");

    choice = readInt("> ");
    if (choice == 0)
        return 0;
    if (choice < 1 || choice > NICHE_COUNT) {
        printf("Invalid choice.\n");
        return 0;
    }

    students[s].niche = choice;
    students[s].roadmapStage = 0;
    printf("You chose %s. Your roadmap starts at stage 1.\n", nicheName(choice));
    return 1;
}

/* Show the 4 stages with DONE / NEXT marks, the progress bar and a next step. */
void showRoadmap(int s)
{
    int i, done = students[s].roadmapStage;
    int n = students[s].niche;

    if (n == 0) {
        printf("You have not chosen a niche yet.\n");
        return;
    }

    printf("\n--- ROADMAP: %s ---\n", nicheName(n));
    for (i = 0; i < STAGE_COUNT; i++) {
        if (i < done)
            printf("[DONE] ");
        else if (i == done)
            printf("[NEXT] ");
        else
            printf("[    ] ");
        printf("Stage %d: %-16s - %s\n", i + 1, p2_stageNames[i], p2_stageTopics[n - 1][i]);
    }

    printf("\nProgress: ");
    p2_printBar(done * 100 / STAGE_COUNT);
    printf("\n");

    if (done >= STAGE_COUNT)
        printf("Next step: roadmap complete! Apply to a program or ask a mentor to review your work.\n");
    else
        printf("Next step: work on \"%s\" - %s.\n", p2_stageNames[done], p2_stageTopics[n - 1][done]);
}

/* Move the student one stage forward (never past 4). */
void updateProgress(int s)
{
    int done = students[s].roadmapStage;

    if (students[s].niche == 0) {
        printf("Choose a niche first.\n");
        return;
    }
    if (done >= STAGE_COUNT) {
        printf("You have already finished every stage. Well done!\n");
        return;
    }

    printf("Current stage: %d - %s\n", done + 1, p2_stageNames[done]);
    if (readInt("Mark this stage as complete? (1 = yes, 0 = no): ") != 1) {
        printf("No change made.\n");
        return;
    }

    students[s].roadmapStage++;
    printf("Stage %d complete!  ", students[s].roadmapStage);
    p2_printBar(students[s].roadmapStage * 100 / STAGE_COUNT);
    printf("\n");
}

/* ================= 2. learning links ================= */
char *p2_videoLinks[NICHE_COUNT][STAGE_COUNT] = {
    { "https://www.youtube.com/results?search_query=quantum+computing+for+beginners+qubits",
      "https://www.youtube.com/results?search_query=quantum+gates+superposition+entanglement+explained",
      "https://www.youtube.com/results?search_query=grover+algorithm+and+VQE+explained",
      "https://www.youtube.com/results?search_query=build+a+quantum+circuit+qiskit+tutorial" },
    { "https://www.youtube.com/results?search_query=dna+rna+central+dogma+explained",
      "https://www.youtube.com/results?search_query=gene+mutation+and+dna+sequencing+basics",
      "https://www.youtube.com/results?search_query=crispr+and+bioinformatics+tools+tutorial",
      "https://www.youtube.com/results?search_query=gc+content+python+tutorial" },
    { "https://www.youtube.com/results?search_query=python+statistics+probability+for+machine+learning",
      "https://www.youtube.com/results?search_query=regression+and+classification+machine+learning+beginners",
      "https://www.youtube.com/results?search_query=neural+networks+overfitting+explained",
      "https://www.youtube.com/results?search_query=scikit+learn+classifier+project+tutorial" },
    { "https://www.youtube.com/results?search_query=atomic+structure+and+error+analysis",
      "https://www.youtube.com/results?search_query=thermodynamics+electrochemistry+optics+basics",
      "https://www.youtube.com/results?search_query=spectroscopy+explained",
      "https://www.youtube.com/results?search_query=how+to+design+a+science+experiment+and+write+a+report" }
};

char *p2_pageLinks[NICHE_COUNT][STAGE_COUNT] = {
    { "https://en.wikipedia.org/wiki/Qubit",
      "https://en.wikipedia.org/wiki/Quantum_logic_gate",
      "https://en.wikipedia.org/wiki/Quantum_algorithm",
      "https://en.wikipedia.org/wiki/Quantum_circuit" },
    { "https://en.wikipedia.org/wiki/Central_dogma_of_molecular_biology",
      "https://en.wikipedia.org/wiki/DNA_sequencing",
      "https://en.wikipedia.org/wiki/CRISPR",
      "https://en.wikipedia.org/wiki/GC-content" },
    { "https://en.wikipedia.org/wiki/Statistics",
      "https://en.wikipedia.org/wiki/Statistical_classification",
      "https://en.wikipedia.org/wiki/Artificial_neural_network",
      "https://en.wikipedia.org/wiki/Scikit-learn" },
    { "https://en.wikipedia.org/wiki/Atom",
      "https://en.wikipedia.org/wiki/Thermodynamics",
      "https://en.wikipedia.org/wiki/Spectroscopy",
      "https://en.wikipedia.org/wiki/Design_of_experiments" }
};

/* opens a link in the default browser (the start command only works on Windows) */
void p2_openLink(const char *link)
{
    char command[400];

    printf("Opening: %s\n", link);
    sprintf(command, "start \"\" \"%s\"", link);
    if (system(command) != 0)
        printf("Could not open the browser. Copy the link above and paste it into your browser.\n");
}

/* Menu of links for the student's niche, grouped by roadmap stage. */
void p2_resourcesMenu(int s)
{
    int n = students[s].niche;
    int stage, pick;

    if (n == 0) {
        printf("Choose a niche first.\n");
        return;
    }

    do {
        printf("\n--- LEARNING RESOURCES: %s ---\n", nicheName(n));
        for (stage = 0; stage < STAGE_COUNT; stage++) {
            printf("Stage %d - %s", stage + 1, p2_stageNames[stage]);
            if (stage == students[s].roadmapStage)
                printf("   <-- you are here");
            printf("\n");
            printf("  %d. Videos : %s\n", stage * 2 + 1, p2_videoLinks[n - 1][stage]);
            printf("  %d. Read   : %s\n", stage * 2 + 2, p2_pageLinks[n - 1][stage]);
        }

        pick = readInt("Number to open in your browser (0 = back): ");
        if (pick == 0)
            break;
        if (pick < 1 || pick > STAGE_COUNT * 2) {
            printf("Invalid choice.\n");
            continue;
        }

        stage = (pick - 1) / 2;
        if (pick % 2 == 1)                      /* 1, 3, 5, 7 = videos */
            p2_openLink(p2_videoLinks[n - 1][stage]);
        else                                    /* 2, 4, 6, 8 = web pages */
            p2_openLink(p2_pageLinks[n - 1][stage]);
    } while (pick != 0);
}

/* The self-guided track menu. Asks for a niche the first time. */
void selfGuidedMenu(int s)
{
    int choice;

    if (students[s].matchedMentorId >= 0)
        printf("Note: you already have a mentor. This track is optional extra practice.\n");

    if (students[s].niche == 0) {
        printf("Pick a niche to start your self-guided roadmap.\n");
        if (p2_chooseNiche(s) == 0)
            return;
    }

    do {
        printf("\n--- SELF-GUIDED TRACK ---\n");
        showProgressSummary(s);
        printf("1. View roadmap\n2. Complete next stage\n3. Learning resources (videos and web pages)\n");
        printf("4. Change niche (progress restarts)\n0. Back\n");
        choice = readInt("> ");

        if (choice == 1) {
            showRoadmap(s);
        } else if (choice == 2) {
            updateProgress(s);
        } else if (choice == 3) {
            p2_resourcesMenu(s);
        } else if (choice == 4) {
            if (readInt("Changing niche resets your progress. Continue? (1 = yes, 0 = no): ") == 1)
                p2_chooseNiche(s);
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/* ================= 3. resource library =================
 * papers[] holds every paper. paper.niche (1 to 4) is the library it belongs to.
 * paper.difficulty: 1 Beginner, 2 Intermediate, 3 Advanced. */
#define TOP_PAPERS 5               /* how many recommendations to show */

char *p2_statusNames[3] = { "To Read", "Reading", "Done" };

char *p2_levelName(int difficulty)
{
    if (difficulty == 1) return "Beginner";
    if (difficulty == 2) return "Intermediate";
    if (difficulty == 3) return "Advanced";
    return "-";
}

/* ---------- typing helpers (same idea as in person1.c) ---------- */

/* Keep asking until the user types a number from min to max. */
int p2_askNumber(char *question, int min, int max)
{
    int number;

    while (1) {
        number = readInt(question);
        if (number >= min && number <= max)
            return number;
        printf("Please enter a number from %d to %d.\n", min, max);
    }
}

/* Keep asking until the user types some text (not empty, not too long). */
void p2_askText(char *question, char *answer, int size)
{
    char typed[500];
    int i, length, onlySpaces;

    while (1) {
        readLine(question, typed, sizeof typed);

        onlySpaces = 1;
        for (i = 0; typed[i] != '\0'; i++) {
            if (typed[i] != ' ')
                onlySpaces = 0;
        }
        length = (int)strlen(typed);

        if (onlySpaces == 0 && length < size) {
            strcpy(answer, typed);
            return;
        }
        if (onlySpaces == 1)
            printf("This cannot be empty.\n");
        else
            printf("Too long - use at most %d characters.\n", size - 1);
    }
}

/* ---------- starter papers ----------
 * Well-known real papers so the library is not empty. The summaries are
 * short descriptions in our own words, not the papers' official abstracts. */

/* Add one paper to papers[], unless a paper with the same title is already there. */
void p2_addStarter(char *title, char *authors, char *keywords,
                          char *summary, int year, int difficulty, int niche)
{
    int i, k, n = paperCount;

    for (i = 0; i < paperCount; i++) {
        if (compareNoCase(papers[i].title, title) == 0)
            return;                                  /* already there */
    }
    if (paperCount >= MAX_PAPERS)
        return;

    papers[n].id = 4001 + n;
    strcpy(papers[n].title, title);
    strcpy(papers[n].authors, authors);
    strcpy(papers[n].keywords, keywords);
    strcpy(papers[n].abstract, summary);
    strcpy(papers[n].field, nicheName(niche));
    papers[n].year = year;
    papers[n].difficulty = difficulty;
    papers[n].niche = niche;

    /* the tag is the first keyword (everything before the first comma) */
    for (k = 0; keywords[k] != '\0' && keywords[k] != ',' && k < NAME_LEN - 1; k++)
        papers[n].tag[k] = keywords[k];
    papers[n].tag[k] = '\0';
    papers[n].readCount = 0;

    paperCount++;
}

/* Called at the start of every library function. Does nothing if the papers are already there. */
void p2_loadStarterPapers(void)
{
    /* niche 1: Quantum Computing */
    p2_addStarter("Simulating Physics with Computers", "Feynman, R. P.",
        "quantum, simulation, computing, physics",
        "Asks why ordinary computers struggle to simulate nature and suggests computers built on quantum mechanics.",
        1982, 1, 1);
    p2_addStarter("A Fast Quantum Mechanical Algorithm for Database Search", "Grover, L. K.",
        "quantum, search, algorithm, grover",
        "A quantum algorithm that searches an unsorted list in about the square root of the usual number of steps.",
        1996, 2, 1);
    p2_addStarter("Quantum Computing in the NISQ Era and Beyond", "Preskill, J.",
        "quantum, nisq, noise, hardware",
        "Explains what noisy, medium-sized quantum computers can and cannot do, and why error correction matters.",
        2018, 2, 1);
    p2_addStarter("A Variational Eigenvalue Solver on a Photonic Quantum Processor", "Peruzzo, A. et al.",
        "quantum, vqe, variational, eigenvalue",
        "Introduces the variational quantum eigensolver, which finds a system's lowest energy by tuning a quantum circuit.",
        2014, 3, 1);

    /* niche 2: Biology / Genomics */
    p2_addStarter("Molecular Structure of Nucleic Acids", "Watson, J. D.; Crick, F. H. C.",
        "dna, double helix, structure, nucleic acids",
        "Proposes the double-helix structure of DNA and hints at how it can copy itself.",
        1953, 1, 2);
    p2_addStarter("Initial Sequencing and Analysis of the Human Genome", "International Human Genome Sequencing Consortium",
        "genome, human, sequencing, genomics",
        "Reports the first draft of the human genome and what it reveals about genes.",
        2001, 2, 2);
    p2_addStarter("Basic Local Alignment Search Tool", "Altschul, S. F. et al.",
        "blast, alignment, sequence, bioinformatics",
        "Describes BLAST, a fast method for finding similar DNA or protein sequences in a database.",
        1990, 2, 2);
    p2_addStarter("A Programmable Dual-RNA-Guided DNA Endonuclease in Adaptive Bacterial Immunity", "Jinek, M. et al.",
        "crispr, cas9, rna, gene editing",
        "Shows that Cas9 can be steered by RNA to cut chosen DNA, the basis of CRISPR gene editing.",
        2012, 3, 2);

    /* niche 3: AI / Machine Learning */
    p2_addStarter("A Few Useful Things to Know About Machine Learning", "Domingos, P.",
        "machine learning, overfitting, generalization, beginner",
        "Plain-language lessons on why learning algorithms succeed or fail, including overfitting.",
        2012, 1, 3);
    p2_addStarter("Deep Learning", "LeCun, Y.; Bengio, Y.; Hinton, G.",
        "deep learning, neural networks, representation",
        "A review of how multi-layer neural networks learn useful features from data.",
        2015, 2, 3);
    p2_addStarter("Random Forests", "Breiman, L.",
        "random forest, classification, ensemble, trees",
        "Describes combining many decision trees so the group predicts better than any single tree.",
        2001, 2, 3);
    p2_addStarter("Attention Is All You Need", "Vaswani, A. et al.",
        "transformer, attention, neural networks, language",
        "Introduces the Transformer, a network built on attention that powers modern language models.",
        2017, 3, 3);

    /* niche 4: Chemistry / Physics */
    p2_addStarter("There's Plenty of Room at the Bottom", "Feynman, R. P.",
        "nanotechnology, atoms, physics, talk",
        "A famous talk imagining the control of individual atoms and writing information at tiny scales.",
        1959, 1, 4);
    p2_addStarter("On a Heuristic Point of View Concerning the Production and Transformation of Light", "Einstein, A.",
        "photoelectric effect, light, quanta, physics",
        "Proposes that light comes in packets of energy, which explains the photoelectric effect.",
        1905, 2, 4);
    p2_addStarter("Electrochemical Photolysis of Water at a Semiconductor Electrode", "Fujishima, A.; Honda, K.",
        "electrochemistry, water splitting, semiconductor, photolysis",
        "Shows that light on a titanium dioxide electrode can split water into hydrogen and oxygen.",
        1972, 2, 4);
    p2_addStarter("The Nature of the Chemical Bond", "Pauling, L.",
        "chemical bond, quantum mechanics, molecules, chemistry",
        "Uses quantum mechanics to explain how atoms bond and what shapes molecules take.",
        1931, 3, 4);
}

/* ---------- paper links ----------
 * One direct link per starter paper, found by matching the title.
 * (The Paper struct is frozen, so the link is looked up here instead of stored.)
 * Papers added later by a mentor have no entry, so they get a Google Scholar
 * search link built from their title. */
static const char *p2_linkTitles[] = {
    "Simulating Physics with Computers",
    "A Fast Quantum Mechanical Algorithm for Database Search",
    "Quantum Computing in the NISQ Era and Beyond",
    "A Variational Eigenvalue Solver on a Photonic Quantum Processor",
    "Molecular Structure of Nucleic Acids",
    "Initial Sequencing and Analysis of the Human Genome",
    "Basic Local Alignment Search Tool",
    "A Programmable Dual-RNA-Guided DNA Endonuclease in Adaptive Bacterial Immunity",
    "A Few Useful Things to Know About Machine Learning",
    "Deep Learning",
    "Random Forests",
    "Attention Is All You Need",
    "There's Plenty of Room at the Bottom",
    "On a Heuristic Point of View Concerning the Production and Transformation of Light",
    "Electrochemical Photolysis of Water at a Semiconductor Electrode",
    "The Nature of the Chemical Bond"
};

static const char *p2_linkUrls[] = {
    "https://doi.org/10.1007/BF02650179",
    "https://arxiv.org/abs/quant-ph/9605043",
    "https://arxiv.org/abs/1801.00862",
    "https://arxiv.org/abs/1304.3061",
    "https://www.nature.com/articles/171737a0",
    "https://www.nature.com/articles/35057062",
    "https://doi.org/10.1016/S0022-2836(05)80360-2",
    "https://doi.org/10.1126/science.1225829",
    "https://homes.cs.washington.edu/~pedrod/papers/cacm12.pdf",
    "https://www.nature.com/articles/nature14539",
    "https://doi.org/10.1023/A:1010933404324",
    "https://arxiv.org/abs/1706.03762",
    "https://calteches.library.caltech.edu/1976/",
    "https://doi.org/10.1002/andp.19053220607",
    "https://doi.org/10.1038/238037a0",
    "https://doi.org/10.1021/ja01355a027"
};

/* Returns the link for the paper at position i (never NULL). */
static const char *p2_paperLink(int i)
{
    static char search[400];
    int k, n = (int)(sizeof p2_linkTitles / sizeof p2_linkTitles[0]);
    int pos;
    const char *t;

    for (k = 0; k < n; k++) {
        if (compareNoCase(papers[i].title, p2_linkTitles[k]) == 0)
            return p2_linkUrls[k];
    }

    /* no direct link known: search for the title (letters and digits kept, anything else becomes '+') */
    strcpy(search, "https://scholar.google.com/scholar?q=");
    pos = (int)strlen(search);
    for (t = papers[i].title; *t != '\0' && pos < (int)sizeof search - 2; t++) {
        if ((*t >= 'a' && *t <= 'z') || (*t >= 'A' && *t <= 'Z') || (*t >= '0' && *t <= '9'))
            search[pos++] = *t;
        else
            search[pos++] = '+';
    }
    search[pos] = '\0';
    return search;
}

/* ---------- printing papers ---------- */
void p2_printPaperHeader(void)
{
    printf("%-4s %-6s %-46s %-5s %-12s %s\n", "No.", "ID", "Title", "Year", "Level", "Niche");
}

/* Print one table row for the paper at position i. 'number' is the row number shown. */
void p2_printPaperRow(int number, int i)
{
    printf("%-4d %-6d %-46.46s %-5d %-12s %s\n", number, papers[i].id, papers[i].title,
           papers[i].year, p2_levelName(papers[i].difficulty), nicheName(papers[i].niche));
}

/* Print everything we know about the paper at position i. */
void p2_showPaper(int i)
{
    printf("\n%s\n", papers[i].title);
    if (papers[i].authors[0] == '\0')
        printf("  Authors     : -\n");
    else
        printf("  Authors     : %s\n", papers[i].authors);
    printf("  Year        : %d\n", papers[i].year);
    printf("  Level       : %s\n", p2_levelName(papers[i].difficulty));
    printf("  Niche       : %s\n", nicheName(papers[i].niche));
    if (papers[i].keywords[0] == '\0')
        printf("  Keywords    : -\n");
    else
        printf("  Keywords    : %s\n", papers[i].keywords);
    if (papers[i].abstract[0] == '\0')
        printf("  Summary     : (none)\n");
    else
        printf("  Summary     : %s\n", papers[i].abstract);
    printf("  Finished by : %d student(s)\n", papers[i].readCount);
    printf("  Link        : %s\n", p2_paperLink(i));
}

/* Ask for a paper ID and show its details. */
void p2_askDetails(void)
{
    int id, i;

    id = readInt("Enter a paper ID to read its details (0 = skip): ");
    if (id == 0)
        return;
    i = findPaperById(id);
    if (i < 0)
        printf("No paper with that ID.\n");
    else {
        p2_showPaper(i);
        if (readInt("Open this paper in your browser? (1 = yes, 0 = no): ") == 1)
            p2_openLink(p2_paperLink(i));
    }
}

/* Sort a list of paper positions, best score first (bubble sort).
 * list[] holds the positions, score[] holds the matching scores.
 * If two scores are equal, the newer paper comes first. */
void p2_sortPapers(int list[], int score[], int count)
{
    int i, j, temp, a, b, swap;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - 1 - i; j++) {
            a = list[j];
            b = list[j + 1];

            swap = 0;
            if (score[j + 1] > score[j])
                swap = 1;
            else if (score[j + 1] == score[j] && papers[b].year > papers[a].year)
                swap = 1;

            if (swap == 1) {
                temp = list[j];   list[j] = list[j + 1];     list[j + 1] = temp;
                temp = score[j];  score[j] = score[j + 1];   score[j + 1] = temp;
            }
        }
    }
}

/* ---------- search ---------- */

/* How well does paper i match the search words?
 * Words in the title = 10 points, in the keywords = 6, in the authors = 4,
 * in the summary = 2. The points of all matches are added together. */
int p2_relevance(int i, char *words)
{
    int score = 0;

    if (words[0] == '\0')
        return 1;                           /* no search words: every paper qualifies */

    if (containsNoCase(papers[i].title, words))
        score += 10;
    if (containsNoCase(papers[i].keywords, words) || containsNoCase(papers[i].tag, words) ||
        containsNoCase(papers[i].field, words))
        score += 6;
    if (containsNoCase(papers[i].authors, words))
        score += 4;
    if (containsNoCase(papers[i].abstract, words))
        score += 2;
    return score;
}

/* Search by title, author or keyword, with filters, most relevant first. */
void searchPapers(void)
{
    char words[200];
    int fromYear, toYear, level, niche, temp;
    int i, count = 0;
    int list[MAX_PAPERS], score[MAX_PAPERS];

    p2_loadStarterPapers();
    printf("\n--- SEARCH PAPERS ---\n");
    readLine("Search words (title, author or keyword; just Enter = all): ", words, sizeof words);
    fromYear = p2_askNumber("From year (0 = any): ", 0, 2100);
    toYear = p2_askNumber("To year (0 = any): ", 0, 2100);
    if (fromYear > 0 && toYear > 0 && fromYear > toYear) {      /* years the wrong way round: swap them */
        temp = fromYear;
        fromYear = toYear;
        toYear = temp;
    }
    level = p2_askNumber("Difficulty 1-3 (0 = any): ", 0, 3);
    niche = p2_askNumber("Niche 1-4 (0 = all niches): ", 0, NICHE_COUNT);

    for (i = 0; i < paperCount; i++) {
        if (fromYear > 0 && papers[i].year < fromYear) continue;
        if (toYear > 0 && papers[i].year > toYear) continue;
        if (level > 0 && papers[i].difficulty != level) continue;
        if (niche > 0 && papers[i].niche != niche) continue;

        score[count] = p2_relevance(i, words);
        if (score[count] > 0) {
            list[count] = i;
            count++;
        }
    }

    if (count == 0) {
        printf("No papers found. Try fewer words or a wider year range.\n");
        return;
    }
    p2_sortPapers(list, score, count);

    printf("\n%d paper(s) found, most relevant first:\n", count);
    p2_printPaperHeader();
    for (i = 0; i < count && i < 15; i++)
        p2_printPaperRow(i + 1, list[i]);
    if (count > 15)
        printf("(showing the top 15)\n");
    p2_askDetails();
}

/* ---------- browse one niche library ---------- */
void p2_browseNiche(void)
{
    int niche, i, count = 0;
    int list[MAX_PAPERS], score[MAX_PAPERS];

    printf("\n--- BROWSE A NICHE LIBRARY ---\n");
    for (i = 0; i < NICHE_COUNT; i++)
        printf("%d. %s\n", i + 1, nicheName(i + 1));
    niche = p2_askNumber("Niche (0 = back): ", 0, NICHE_COUNT);
    if (niche == 0)
        return;

    for (i = 0; i < paperCount; i++) {
        if (papers[i].niche == niche) {
            list[count] = i;
            score[count] = (4 - papers[i].difficulty) * 10;     /* easy papers get a higher score, so they come first */
            count++;
        }
    }
    if (count == 0) {
        printf("No papers in this library yet.\n");
        return;
    }
    p2_sortPapers(list, score, count);

    printf("\n%s library (%d papers, easiest first):\n", nicheName(niche), count);
    p2_printPaperHeader();
    for (i = 0; i < count; i++)
        p2_printPaperRow(i + 1, list[i]);
    p2_askDetails();
}

/* ---------- reading list ----------
 * students[s].readingList[k]  = ID of the k-th paper in the list
 * students[s].readStatus[k]   = 0 To Read, 1 Reading, 2 Done
 * papers[i].readCount         = how many students marked the paper Done */

/* Position of a paper ID in the student's reading list, or -1 if it is not there. */
int p2_findInList(int s, int paperId)
{
    int k;

    for (k = 0; k < students[s].readingCount; k++) {
        if (students[s].readingList[k] == paperId)
            return k;
    }
    return -1;
}

void p2_addToList(int s, int paperId)
{
    int i = findPaperById(paperId);
    int count = students[s].readingCount;

    if (i < 0) {
        printf("No paper with that ID.\n");
        return;
    }
    if (p2_findInList(s, paperId) >= 0) {
        printf("That paper is already in your list.\n");
        return;
    }
    if (count >= MAX_READING) {
        printf("Your reading list is full (%d papers). Remove one first.\n", MAX_READING);
        return;
    }

    students[s].readingList[count] = paperId;
    students[s].readStatus[count] = 0;                 /* new papers start as To Read */
    students[s].readingCount++;
    printf("Added \"%s\" (To Read).\n", papers[i].title);
}

/* Change the status of the paper at position pos in the reading list. */
void p2_setStatus(int s, int pos, int newStatus)
{
    int oldStatus = students[s].readStatus[pos];
    int i = findPaperById(students[s].readingList[pos]);

    if (i >= 0) {
        if (oldStatus != 2 && newStatus == 2)
            papers[i].readCount++;                     /* the student just finished it */
        if (oldStatus == 2 && newStatus != 2 && papers[i].readCount > 0)
            papers[i].readCount--;                     /* no longer counted as finished */
    }
    students[s].readStatus[pos] = newStatus;
}

/* Remove the paper at position pos. The papers after it move one place to the left. */
void p2_removeFromList(int s, int pos)
{
    int k;
    int i = findPaperById(students[s].readingList[pos]);

    if (students[s].readStatus[pos] == 2 && i >= 0 && papers[i].readCount > 0)
        papers[i].readCount--;

    for (k = pos; k < students[s].readingCount - 1; k++) {
        students[s].readingList[k] = students[s].readingList[k + 1];
        students[s].readStatus[k] = students[s].readStatus[k + 1];
    }
    students[s].readingCount--;
}

void readingListMenu(int s)
{
    int choice, k, i, pos, status, done;

    p2_loadStarterPapers();
    do {
        printf("\n--- MY READING LIST (%d/%d) ---\n", students[s].readingCount, MAX_READING);
        done = 0;
        if (students[s].readingCount == 0) {
            printf("(empty - add papers from the library)\n");
        } else {
            printf("%-4s %-6s %-46s %s\n", "No.", "ID", "Title", "Status");
            for (k = 0; k < students[s].readingCount; k++) {
                i = findPaperById(students[s].readingList[k]);
                if (i >= 0)
                    printf("%-4d %-6d %-46.46s %s\n", k + 1, students[s].readingList[k],
                           papers[i].title, p2_statusNames[students[s].readStatus[k]]);
                else
                    printf("%-4d %-6d %-46.46s %s\n", k + 1, students[s].readingList[k],
                           "(paper no longer exists)", p2_statusNames[students[s].readStatus[k]]);
                if (students[s].readStatus[k] == 2)
                    done++;
            }
            printf("Finished: %d of %d\n", done, students[s].readingCount);
        }

        printf("1. Add a paper by ID\n2. Change a paper's status\n3. Remove a paper\n0. Back\n");
        choice = readInt("> ");

        if (choice == 1) {
            p2_addToList(s, readInt("Paper ID: "));
        } else if (choice == 2) {
            if (students[s].readingCount == 0) {
                printf("Your list is empty.\n");
            } else {
                pos = p2_askNumber("List number: ", 1, students[s].readingCount);
                printf("1. To Read\n2. Reading\n3. Done\n");
                status = p2_askNumber("New status: ", 1, 3);
                p2_setStatus(s, pos - 1, status - 1);
                printf("Status updated.\n");
            }
        } else if (choice == 3) {
            if (students[s].readingCount == 0) {
                printf("Your list is empty.\n");
            } else {
                pos = p2_askNumber("List number to remove: ", 1, students[s].readingCount);
                p2_removeFromList(s, pos - 1);
                printf("Removed.\n");
            }
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

/* ---------- recommendations matched to the roadmap stage ---------- */
void recommendPapers(int s)
{
    int i, count = 0, stage, target, difference, id;
    int list[MAX_PAPERS], score[MAX_PAPERS];

    p2_loadStarterPapers();
    if (students[s].niche == 0) {
        printf("Choose a niche in the Self-guided track first (student menu option 5), so papers can match your roadmap.\n");
        return;
    }

    /* early stages need easy papers, later stages need harder ones */
    stage = students[s].roadmapStage;
    if (stage == 0)
        target = 1;
    else if (stage == 1)
        target = 2;
    else
        target = 3;

    for (i = 0; i < paperCount; i++) {
        if (papers[i].niche != students[s].niche)
            continue;
        difference = papers[i].difficulty - target;
        if (difference < 0)
            difference = -difference;
        list[count] = i;
        score[count] = 10 - 3 * difference;            /* exact level 10, one level off 7, two off 4 */
        count++;
    }
    if (count == 0) {
        printf("No papers in your niche library yet.\n");
        return;
    }
    p2_sortPapers(list, score, count);

    printf("\n--- RECOMMENDED FOR YOU ---\n");
    printf("Niche: %s | Roadmap: ", nicheName(students[s].niche));
    if (stage < STAGE_COUNT)
        printf("%s", p2_stageNames[stage]);
    else
        printf("Completed");
    printf(" | Best level: %s\n", p2_levelName(target));

    p2_printPaperHeader();
    for (i = 0; i < count && i < TOP_PAPERS; i++)
        p2_printPaperRow(i + 1, list[i]);

    id = readInt("Enter a paper ID to add it to your reading list (0 = skip): ");
    if (id != 0)
        p2_addToList(s, id);
}

/* ---------- add a paper (for mentors and admin) ---------- */
void addPaper(void)
{
    int n, i, k;

    p2_loadStarterPapers();
    if (paperCount >= MAX_PAPERS) {
        printf("The library is full (%d papers).\n", MAX_PAPERS);
        return;
    }

    /* Like in registration: fill the next free slot; it only counts when paperCount goes up. */
    n = paperCount;

    printf("\n--- ADD A RECOMMENDED PAPER ---\n");
    p2_askText("Title: ", papers[n].title, TEXT_LEN);
    for (i = 0; i < paperCount; i++) {
        if (compareNoCase(papers[i].title, papers[n].title) == 0) {
            printf("That paper is already in the library (ID %d).\n", papers[i].id);
            return;
        }
    }
    p2_askText("Authors: ", papers[n].authors, TEXT_LEN);
    p2_askText("Keywords (separated by commas): ", papers[n].keywords, TEXT_LEN);
    p2_askText("Short summary: ", papers[n].abstract, LONG_LEN);
    papers[n].year = p2_askNumber("Year (1800-2100): ", 1800, 2100);
    papers[n].difficulty = p2_askNumber("Difficulty (1 Beginner, 2 Intermediate, 3 Advanced): ", 1, 3);
    for (i = 0; i < NICHE_COUNT; i++)
        printf("%d. %s\n", i + 1, nicheName(i + 1));
    papers[n].niche = p2_askNumber("Niche library for this paper: ", 1, NICHE_COUNT);

    papers[n].id = 4001 + n;
    strcpy(papers[n].field, nicheName(papers[n].niche));
    for (k = 0; papers[n].keywords[k] != '\0' && papers[n].keywords[k] != ',' && k < NAME_LEN - 1; k++)
        papers[n].tag[k] = papers[n].keywords[k];       /* the tag is the first keyword */
    papers[n].tag[k] = '\0';
    papers[n].readCount = 0;

    paperCount++;
    printf("Paper added to the %s library with ID %d.\n", nicheName(papers[n].niche), papers[n].id);
}

/* ---------- the library menu (student menu option 6) ---------- */
void libraryMenu(int s)
{
    int choice;

    p2_loadStarterPapers();
    do {
        printf("\n--- RESOURCE LIBRARY (%d papers) ---\n", paperCount);
        printf("1. Browse a niche library\n2. Search papers\n3. Recommended for my roadmap stage\n");
        printf("4. My reading list\n0. Back\n");
        choice = readInt("> ");

        if (choice == 1)
            p2_browseNiche();
        else if (choice == 2)
            searchPapers();
        else if (choice == 3)
            recommendPapers(s);
        else if (choice == 4)
            readingListMenu(s);
        else if (choice != 0)
            printf("Invalid choice.\n");
    } while (choice != 0);
}
