#ifndef REVIEW_H
#define REVIEW_H

#define REVIEW_COMMENT_LEN 200
#define REVIEW_DATE_LEN 11  
#define REVIEWS_FILE "data/reviews.dat"

typedef struct Review {
    int id;
    int patientId;
    int doctorId;
    int rating;   /* 1-5 */
    char comment[REVIEW_COMMENT_LEN];
    char date[REVIEW_DATE_LEN];
    struct Review *next;
} Review;

/* ---- Lifecycle ---- */
Review* loadReviews(void);
void saveReviews(Review *head);
void freeReviews(Review *head);

/* ---- CRUD ---- */

/* Adds a new review, OR, if this patient already reviewed this
 * doctor, updates the existing review in place instead of
 * appending a duplicate. Returns the (possibly unchanged) head
 * pointer, following the same convention as addPatient/addDoctor/
 * addAppointment/addBill. */
Review* addOrUpdateReview(Review *head, int patientId, int doctorId,
                           int rating, const char *comment, const char *date);

Review* deleteReview(Review *head, int id);

/* ---- Search ---- */
Review* findReviewById(Review *head, int id);
Review* findReviewByPatientAndDoctor(Review *head, int patientId, int doctorId);

/* ---- Reporting ---- */

/* LINEAR search/filter: prints every review belonging to the
 * given doctor id. Mirrors displayAppointmentsByDoctorId() in
 * appointment.c. */
void displayReviewsByDoctorId(Review *head, int doctorId);
void displayAllReviews(Review *head);

/* Average rating (0.0 if none) and review count for one doctor.
 * This is the "Rating System" figure used to rank doctors of the
 * same specialization against each other. */
float averageRatingForDoctor(Review *head, int doctorId, int *outCount);

/* Average rating across EVERY review in the system -- the
 * overall "hospital rating" a patient can view. */
float overallHospitalRating(Review *head, int *outCount);

#endif