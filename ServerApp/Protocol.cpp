#include "Protocol.h"

Command Protocol::parseLine(const QString& line)
{
    Command c;
    const QString s = line.trimmed();
    int sp = s.indexOf(' ');
    c.verb = (sp < 0) ? s : s.left(sp);
    c.verb = c.verb.toUpper();
    c.rest = (sp < 0) ? QString() : s.mid(sp + 1);
    return c;
}
