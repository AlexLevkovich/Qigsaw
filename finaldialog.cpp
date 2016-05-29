#include "finaldialog.h"
#include "ui_finaldialog.h"
#include "mainwindow.h"

#include <QApplication>
#include <QAction>

FinalDialog::FinalDialog(time_t time,MainWindow *parent) :
    QDialog(parent),
    m_ui(new Ui::FinalDialog) {
    m_ui->setupUi(this);

    mainWindow = parent;
    int days = time/86400;
    int hours = (time - (days*86400))/3600;
    int minutes = (time - ((days * 86400) + (hours * 3600)))/60;
    int seconds = time - ((days * 86400) + (hours * 3600) + (minutes * 60));
    m_ui->timeLabel->setText(m_ui->timeLabel->text()+" "+QString::number(days)+" days, "+QString::number(hours)+" hrs, "+QString::number(minutes)+" mins, "+QString::number(seconds)+" secs");
}

FinalDialog::~FinalDialog() {
    delete m_ui;
}

void FinalDialog::changeEvent(QEvent *e) {
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FinalDialog::on_newButton_clicked() {
    mainWindow->getNewAction()->trigger();
}

void FinalDialog::on_openButton_clicked() {
    mainWindow->getOpenAction()->trigger();
}

void FinalDialog::on_exitButton_clicked() {
    qApp->quit();
}

void FinalDialog::on_cancelButton_clicked() {
    reject();
}
