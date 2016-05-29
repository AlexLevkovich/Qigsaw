#ifndef QPREVIEWFILEDIALOG_H
#define QPREVIEWFILEDIALOG_H

#include <QFileDialog>
#include <QItemSelection>
#include <QUrl>

class QTreeView;
class QLabel;
class QWidget;
class QGridLayout;

class QFilePreview {
public:
    QFilePreview() {}
    ~QFilePreview() {}
    virtual void previewUrl(const QUrl & url) = 0;
    virtual QWidget * widget() = 0;
};

class QPreviewFileDialog : public QFileDialog {
    Q_OBJECT
public:
    QPreviewFileDialog(QFilePreview * preview,QWidget *parent = 0);
    ~QPreviewFileDialog();
protected slots:
    void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
private:
    QTreeView *list;
    QFilePreview * preview;
    QGridLayout * gl;
signals:

};

#endif // QPREVIEWFILEDIALOG_H
