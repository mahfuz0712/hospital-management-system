#ifndef AUTH_H
#define AUTH_H

#include "user.h"


User* ensureDefaultAdmin(User *head);


User* login(User *head);

#endif