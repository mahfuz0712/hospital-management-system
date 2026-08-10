#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/printing.h"
#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif


static void ensureReceiptsDirExists(void) {
#ifdef _WIN32
    system("if not exist data\\receipts mkdir data\\receipts >nul 2>nul");
#else
    system("mkdir -p data/receipts");
#endif
}

int writeBillReceiptToFile(const Bill *b, const char *filepath) {
    if (b == NULL || filepath == NULL) {
        return 0;
    }

    FILE *fp = fopen(filepath, "w");
    if (fp == NULL) {
        return 0;
    }


    fprintf(fp, "==========================================\n");
    fprintf(fp, "        HOSPITAL BILLING RECEIPT\n");
    fprintf(fp, "==========================================\n");
    fprintf(fp, "Bill ID          : %d\n", b->id);
    fprintf(fp, "Patient ID       : %d\n", b->patientId);
    fprintf(fp, "Date             : %s\n", b->date);
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "Consultation Fee : %10.2f\n", b->consultationFee);
    fprintf(fp, "Medicine Cost    : %10.2f\n", b->medicineCost);
    fprintf(fp, "Other Charges    : %10.2f\n", b->otherCharges);
    fprintf(fp, "------------------------------------------\n");
    fprintf(fp, "TOTAL AMOUNT     : %10.2f\n", b->totalAmount);
    fprintf(fp, "==========================================\n");
    fprintf(fp, "        Thank you for your visit!\n");
    fprintf(fp, "==========================================\n");

    fclose(fp);
    return 1;
}

int sendFileToDefaultPrinter(const char *filepath) {
#ifdef _WIN32

    HINSTANCE result = ShellExecuteA(NULL, "print", filepath, NULL, NULL, SW_HIDE);
    return ((INT_PTR)result > 32);
#else
    (void)filepath;

    return 0;
#endif
}

int printBillReceipt(const Bill *b, char *filepathOut, int filepathOutSize) {
    if (b == NULL) {
        return 0;
    }

    ensureReceiptsDirExists();

    char filepath[260];
    snprintf(filepath, sizeof(filepath), "data/receipts/bill_%d.txt", b->id);

    if (filepathOut != NULL && filepathOutSize > 0) {
        strncpy(filepathOut, filepath, (size_t)filepathOutSize - 1);
        filepathOut[filepathOutSize - 1] = '\0';
    }

    if (!writeBillReceiptToFile(b, filepath)) {
        return 0;
    }

    return sendFileToDefaultPrinter(filepath);
}