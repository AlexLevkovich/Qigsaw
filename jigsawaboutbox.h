#ifndef JIGSAWABOUTBOX_H
#define JIGSAWABOUTBOX_H

#include <QDialog>

namespace Ui {
    class JigsawAboutBox;
}

class JigsawAboutBox : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(JigsawAboutBox)
public:
    explicit JigsawAboutBox(QWidget *parent = 0);
    virtual ~JigsawAboutBox();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::JigsawAboutBox *m_ui;

private slots:
    void on_closeButton_clicked();
};

#endif // JIGSAWABOUTBOX_H
