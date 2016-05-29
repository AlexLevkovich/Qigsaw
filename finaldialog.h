#ifndef FINALDIALOG_H
#define FINALDIALOG_H

#include <QDialog>
#include <time.h>

namespace Ui {
    class FinalDialog;
}

class MainWindow;

class FinalDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(FinalDialog)
public:
    explicit FinalDialog(time_t time,MainWindow *parent = 0);
    virtual ~FinalDialog();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::FinalDialog *m_ui;
    MainWindow * mainWindow;

private slots:
    void on_cancelButton_clicked();
    void on_exitButton_clicked();
    void on_openButton_clicked();
    void on_newButton_clicked();
};

#endif // FINALDIALOG_H
