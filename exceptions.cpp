#include "exceptions.h"

Exception::Exception() {
    type = basic;
}

Exception::Exception(ExType type) {
    this->type = type;
}

Exception::Exception(ExType type,const QString & message) {
    this->type = type;
    setMessage(message);
}

void Exception::setMessage(const QString & message) {
    this->message = message;
}

ExType Exception::getType() {
    return type;
}

QString Exception::toString() const {
    return message;
}

IllegalArgumentException::IllegalArgumentException():Exception(IllegalArgument) {}
IllegalArgumentException::IllegalArgumentException(const QString & message):Exception(IllegalArgument,message) {}

NullPointerException::NullPointerException():Exception(NullPointer) {}
NullPointerException::NullPointerException(const QString & message):Exception(NullPointer,message) {}
