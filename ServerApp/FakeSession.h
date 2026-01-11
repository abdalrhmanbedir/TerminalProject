#ifndef FAKESESSION_H
#define FAKESESSION_H

#pragma once
#include <QString>

class FakeSession {
public:
    virtual ~FakeSession() = default;
    virtual bool isAuthenticated() const = 0;
    virtual void setAuthenticated(bool v) = 0;
};


#endif // FAKESESSION_H
