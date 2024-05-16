#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>

// Функции для выполнения каждого из пунктов меню
int callbackGame(void *data, int argc, char **argv, char **azColName) {
    int *scores = (int*)data;
    int correct_answer = atoi(argv[6]);
    int user_answer;

    printf("Current score: %d\n", scores[0]);
    printf("Question: %s\n", argv[1]);
    printf("1) %s\n", argv[2]);
    printf("2) %s\n", argv[3]);
    printf("3) %s\n", argv[4]);
    printf("4) %s\n", argv[5]);
    printf("Enter your answer (1, 2, 3, or 4): ");
    scanf("%d", &user_answer);

    if (user_answer != correct_answer) {
        printf("Incorrect answer. The correct answer was %d.\n", correct_answer);
        return 1;
    }

    if (strcmp(argv[7], "easy") == 0) {
        scores[0] += 50;
    } else if (strcmp(argv[7], "medium") == 0) {
        scores[0] += 100;
    } else if (strcmp(argv[7], "hard") == 0) {
        scores[0] += 200;
    }

    return 0;
}

void start_game(sqlite3 *db) {
    char *err_msg = 0;
    char *sql;
    int rc;
    int game_over = 0;
    int scores[2] = {0, 0}; // scores[0] is current_score, scores[1] is max_score

    char *difficulties[3] = {"easy", "medium", "hard"};

    for (int i = 0; i < 3; i++) {
        printf("Starting %s questions...\n", difficulties[i]);

        sql = sqlite3_mprintf("SELECT * FROM questions WHERE difficulty = '%q' ORDER BY RANDOM() LIMIT 5", difficulties[i]);

        rc = sqlite3_exec(db, sql, callbackGame, scores, &err_msg);

        sqlite3_free(sql);

        if (rc == SQLITE_ABORT) {
            printf("Game over\n");
            game_over = 1;
            break;
        }
    }

    if (scores[0] > scores[1]) {
        scores[1] = scores[0];
    }
    printf("Your final score: %d\n", scores[0]);
    printf("Max score: %d\n", scores[1]);

    if (game_over) {
        printf("You lost the game. Better luck next time!\n");
    } else {
        printf("Congratulations! You won the game!\n");
    }
    printf("\n");
}

int callbackList(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void list_questions(sqlite3 *db) {
    char *err_msg = 0;
    char *sql = "SELECT * FROM Questions";

    int rc = sqlite3_exec(db, sql, callbackList, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to fetch data\n");
        printf("SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
    }
}

void add_question(sqlite3 *db) {
    char question[256];
    char answer_1[256];
    char answer_2[256];
    char answer_3[256];
    char answer_4[256];
    int correct_answer;
    char difficulty[10];
    char sql[1024];
    char *err_msg = 0;

    printf("Enter the question: ");
    scanf(" %[^\n]", question);
    printf("Enter answer 1: ");
    scanf(" %[^\n]", answer_1);
    printf("Enter answer 2: ");
    scanf(" %[^\n]", answer_2);
    printf("Enter answer 3: ");
    scanf(" %[^\n]", answer_3);
    printf("Enter answer 4: ");
    scanf(" %[^\n]", answer_4);

    do {
        printf("Enter the correct answer (1, 2, 3, or 4): ");
        scanf("%d", &correct_answer);
    } while(correct_answer < 1 || correct_answer > 4);

    do {
        printf("Enter the difficulty (easy, medium, or hard): ");
        scanf(" %[^\n]", difficulty);
    } while (strcmp(difficulty, "easy") != 0 && strcmp(difficulty, "medium") != 0 && strcmp(difficulty, "hard") != 0);

    sprintf(sql,
            "INSERT INTO questions(question, answer_1, answer_2, answer_3, answer_4, correct_answer, difficulty) "
            "VALUES('%s', '%s', '%s', '%s', '%s', '%d', '%s')",
            question, answer_1, answer_2, answer_3, answer_4, correct_answer, difficulty);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to insert question\n");
        printf("SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
    } else {
        printf("Question added successfully\n");
    }
}

void delete_question(sqlite3 *db) {
    int id;
    char sql[256];
    char *err_msg = 0;

    printf("Enter the ID of the question to delete: ");
    scanf("%d", &id);

    sprintf(sql, "DELETE FROM questions WHERE id = %d", id);

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("Failed to delete question\n");
        printf("SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
    } else {
        printf("Question deleted successfully\n");
    }
}

// Функция для отображения меню и обработки выбора пользователя
_Noreturn void display_menu(sqlite3 *db) {
    int choice;

    while (1) {
        printf("1) Start game\n");
        printf("2) List questions\n");
        printf("3) Add question\n");
        printf("4) Delete question\n");
        printf("5) Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                start_game(db);
                break;
            case 2:
                list_questions(db);
                break;
            case 3:
                add_question(db);
                break;
            case 4:
                delete_question(db);
                break;
            case 5:
                sqlite3_close(db);
                exit(0);
            default:
                printf("Invalid choice\n");
                break;
        }
    }
}

int main() {
    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open("MillionaireGame.sqlite", &db);

    printf("Welcome to Who Wants to Be a Millionaire!\n");
    display_menu(db);
}