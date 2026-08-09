#ifndef UTILS_H
#define UTILS_H

void clearScreen(void);

void pauseScreen(void);

void readLine(char *buffer, int size);

void trimString(char *str);

int readInt(const char *prompt);

float readFloat(const char *prompt);

#endif