#ifndef NEWDIALOG_H
#define NEWDIALOG_H

#include <QDialog>
#include <QColor>
#include <QList>

class QAbstractButton;
class JigsawCutter;
class QShowEvent;

namespace Ui {
    class NewDialog;
}

class NewDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(NewDialog)
public:
    explicit NewDialog(QWidget *parent = 0);
    virtual ~NewDialog();

    QColor getSelectedColor() const;
    QString getSelectedPath() const;
    JigsawCutter * getSelectedCutter();
    int getPieceCount();
protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::NewDialog *m_ui;
    static const QColor defBackground;
    QColor selectedColor;
    QList<JigsawCutter *> cutters;
    static QString prevName;

private slots:
    void on_pathLineEdit_textChanged(QString );
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_cuttingCombo_activated(int index);
    void on_backGroundButton_clicked();
    void on_pathButton_clicked();
};

#endif // NEWDIALOG_H
