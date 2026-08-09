#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/printing.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

/* Makes sure data/receipts/ exists before we try to write into
 * it. Cross-platform: the Windows branch is what actually runs
 * in the real MSYS2-MinGW build; the POSIX branch only exists so
 * the project still compiles cleanly on non-Windows machines. */
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

    /* Same layout as displayBillReceipt() in billing.c, just
     * aimed at a file instead of stdout, so the printed paper
     * looks exactly like the on-screen receipt. */
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
    /* ShellExecute with the "print" verb hands the file to
     * whatever the OS has registered as the default printer --
     * this works identically for a normal office printer and for
     * a POS/receipt printer, since POS printers are installed as
     * a Windows printer driver too. ShellExecute returns a value
     * greater than 32 on success; anything <= 32 is an error code. */
    HINSTANCE result = ShellExecuteA(NULL, "print", filepath, NULL, NULL, SW_HIDE);
    return ((INT_PTR)result > 32);
#else
    (void)filepath;
    /* Non-Windows build: no Windows printer driver to hand this
     * off to. The receipt file has already been written by
     * writeBillReceiptToFile(), so it can still be opened/printed
     * manually -- nothing is lost, this just can't auto-print. */
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