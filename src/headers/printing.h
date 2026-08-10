#ifndef PRINTING_H
#define PRINTING_H

#include "billing.h"


int writeBillReceiptToFile(const Bill *b, const char *filepath);


int sendFileToDefaultPrinter(const char *filepath);


int printBillReceipt(const Bill *b, char *filepathOut, int filepathOutSize);

#endif