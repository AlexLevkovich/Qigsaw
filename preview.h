#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include "qpreviewfiledialog.h"

class QLabel;
class QBoxLayout;

class Preview : public QWidget, public QFilePreview {
public:
    Preview(QWidget *parent=0);
    ~Preview();

    void previewUrl(const QUrl &u);
    QWidget * widget();
 private:
     QLabel * pImageLabel;
     QLabel * pTextLabel;
     QBoxLayout * layout;
};

#endif // PREVIEW_H
