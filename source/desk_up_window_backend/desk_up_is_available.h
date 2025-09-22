#ifndef DESKUPISAVAILABLE_H
#define DESKUPISAVAILABLE_H

struct DeskUpisAvailable{
    const char * name;
    bool (*isAvailable)(void);
};

#endif