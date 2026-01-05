#ifndef PROTOCOL_H
#define PROTOCOL_H

#pragma once
#include <QString>

struct Command {
    QString verb;
    QString rest;
};

class Protocol {
public:
    static Command parseLine(const QString& line);
};



#endif // PROTOCOL_H
