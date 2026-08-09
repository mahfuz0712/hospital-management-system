/* ============================================================
 * printing.h
 * ------------------------------------------------------------
 * Sends a Bill receipt to a physical output device: either a
 * POS receipt printer or a regular printer. Both look identical
 * to Windows once installed as a printer driver, so from this
 * program's point of view "print to POS machine" and "print to
 * printer" are the SAME operation: format the receipt as plain
 * text, then hand it to whatever printer Windows currently has
 * set as default.
 *
 * How it works (Windows / MSYS2-MinGW build only):
 *   1. writeBillReceiptToFile() formats the Bill exactly like
 *      displayBillReceipt() in billing.c, but writes it to a
 *      .txt file under data/receipts/ instead of the screen.
 *   2. sendFileToDefaultPrinter() calls the Windows ShellExecute
 *      API with the "print" verb, which asks Windows to open
 *      that file with whatever program is registered to print
 *      .txt files and send it straight to the default printer.
 *
 * On a non-Windows build (e.g. compiling on Linux just to check
 * the code compiles) step 2 cannot reach a real printer, so it
 * safely reports failure while the receipt file itself is still
 * written -- nothing is lost, it can be printed manually.
 * ============================================================ */

#ifndef PRINTING_H
#define PRINTING_H

#include "billing.h"

/* Writes a formatted plain-text receipt for the given bill to
 * filepath. Returns 1 on success, 0 on failure (e.g. could not
 * open the file for writing). */
int writeBillReceiptToFile(const Bill *b, const char *filepath);

/* Sends the given file to the system's default printer.
 * Returns 1 if the print job was successfully handed off to the
 * OS, 0 otherwise. This does NOT guarantee the physical printer
 * produced paper (that depends on the OS/driver/hardware) --
 * only that the print command was launched without error. */
int sendFileToDefaultPrinter(const char *filepath);

/* Convenience wrapper used by the menus: writes the bill to
 * data/receipts/bill_<id>.txt and immediately sends it to the
 * default printer. Returns 1 if BOTH steps succeeded, 0 if
 * either step failed. If filepathOut is not NULL, the path that
 * was written is copied into it (up to filepathOutSize bytes) so
 * the caller can show/offer it to the user regardless of whether
 * the automatic print succeeded. */
int printBillReceipt(const Bill *b, char *filepathOut, int filepathOutSize);

#endif