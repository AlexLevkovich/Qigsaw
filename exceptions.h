#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <QString>

enum ExType {
    basic,
    IllegalArgument,
    NullPointer
};

class Exception {
public:
    Exception();
    Exception(ExType type);
    Exception(ExType type,const QString & message);
    void setMessage(const QString & message);
    ExType getType();

    QString toString() const;
protected:
    ExType type;
    QString message;
};

class IllegalArgumentException: public Exception {
public:
    IllegalArgumentException();
    IllegalArgumentException(const QString & message);
};

class NullPointerException: public Exception {
public:
    NullPointerException();
    NullPointerException(const QString & message);
};

#endif // _EXCEPTIONS_H_
