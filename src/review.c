#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/review.h"

Review* loadReviews(void) {
    FILE *fp = fopen(REVIEWS_FILE, "rb");
    if (fp == NULL) {
        return NULL;
    }

    Review *head = NULL;
    Review *tail = NULL;
    Review temp;

    while (fread(&temp, sizeof(Review), 1, fp) == 1) {
        Review *node = (Review*) malloc(sizeof(Review));
        if (node == NULL) {
            printf("Memory allocation failed while loading reviews.\n");
            break;
        }
        *node = temp;
        node->next = NULL;

        if (head == NULL) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }

    fclose(fp);
    return head;
}

void saveReviews(Review *head) {
    FILE *fp = fopen(REVIEWS_FILE, "wb");
    if (fp == NULL) {
        printf("Error: could not open %s for writing.\n", REVIEWS_FILE);
        return;
    }

    for (Review *curr = head; curr != NULL; curr = curr->next) {
        fwrite(curr, sizeof(Review), 1, fp);
    }

    fclose(fp);
}

void freeReviews(Review *head) {
    Review *curr = head;
    while (curr != NULL) {
        Review *next = curr->next;
        free(curr);
        curr = next;
    }
}

Review* findReviewById(Review *head, int id) {
    for (Review *curr = head; curr != NULL; curr = curr->next) {
        if (curr->id == id) {
            return curr;
        }
    }
    return NULL;
}

Review* findReviewByPatientAndDoctor(Review *head, int patientId, int doctorId) {
    for (Review *curr = head; curr != NULL; curr = curr->next) {
        if (curr->patientId == patientId && curr->doctorId == doctorId) {
            return curr;
        }
    }
    return NULL;
}

Review* addOrUpdateReview(Review *head, int patientId, int doctorId,
                           int rating, const char *comment, const char *date) {

    Review *existing = findReviewByPatientAndDoctor(head, patientId, doctorId);
    if (existing != NULL) {
        existing->rating = rating;

        strncpy(existing->comment, comment, REVIEW_COMMENT_LEN - 1);
        existing->comment[REVIEW_COMMENT_LEN - 1] = '\0';

        strncpy(existing->date, date, REVIEW_DATE_LEN - 1);
        existing->date[REVIEW_DATE_LEN - 1] = '\0';

        return head;
    }

    Review *node = (Review*) malloc(sizeof(Review));
    if (node == NULL) {
        printf("Memory allocation failed while adding review.\n");
        return head;
    }

    int maxId = 0;
    for (Review *curr = head; curr != NULL; curr = curr->next) {
        if (curr->id > maxId) maxId = curr->id;
    }
    node->id = maxId + 1;

    node->patientId = patientId;
    node->doctorId = doctorId;
    node->rating = rating;

    strncpy(node->comment, comment, REVIEW_COMMENT_LEN - 1);
    node->comment[REVIEW_COMMENT_LEN - 1] = '\0';

    strncpy(node->date, date, REVIEW_DATE_LEN - 1);
    node->date[REVIEW_DATE_LEN - 1] = '\0';

    node->next = NULL;

    if (head == NULL) {
        return node;
    }

    Review *curr = head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = node;

    return head;
}

Review* deleteReview(Review *head, int id) {
    Review *curr = head;
    Review *prev = NULL;

    while (curr != NULL) {
        if (curr->id == id) {
            if (prev == NULL) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return head;
        }
        prev = curr;
        curr = curr->next;
    }

    return head;
}

void displayReviewsByDoctorId(Review *head, int doctorId) {
    int found = 0;

    printf("%-5s %-10s %-7s %-11s %s\n", "ID", "PatientID", "Rating", "Date", "Comment");
    printf("-----------------------------------------------------------------------------\n");

    for (Review *curr = head; curr != NULL; curr = curr->next) {
        if (curr->doctorId == doctorId) {
            printf("%-5d %-10d %-7d %-11s %s\n",
                   curr->id, curr->patientId, curr->rating, curr->date, curr->comment);
            found = 1;
        }
    }

    if (!found) {
        printf("No reviews found for doctor ID %d.\n", doctorId);
    }
}

void displayAllReviews(Review *head) {
    if (head == NULL) {
        printf("No reviews found.\n");
        return;
    }

    printf("%-5s %-10s %-10s %-7s %-11s %s\n", "ID", "PatientID", "DoctorID", "Rating", "Date", "Comment");
    printf("-----------------------------------------------------------------------------\n");

    for (Review *curr = head; curr != NULL; curr = curr->next) {
        printf("%-5d %-10d %-10d %-7d %-11s %s\n",
               curr->id, curr->patientId, curr->doctorId, curr->rating, curr->date, curr->comment);
    }
}

float averageRatingForDoctor(Review *head, int doctorId, int *outCount) {
    int count = 0;
    int sum = 0;

    for (Review *curr = head; curr != NULL; curr = curr->next) {
        if (curr->doctorId == doctorId) {
            sum += curr->rating;
            count++;
        }
    }

    if (outCount) *outCount = count;
    return (count == 0) ? 0.0f : ((float)sum / (float)count);
}

float overallHospitalRating(Review *head, int *outCount) {
    int count = 0;
    int sum = 0;

    for (Review *curr = head; curr != NULL; curr = curr->next) {
        sum += curr->rating;
        count++;
    }

    if (outCount) *outCount = count;
    return (count == 0) ? 0.0f : ((float)sum / (float)count);
}