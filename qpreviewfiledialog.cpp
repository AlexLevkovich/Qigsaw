#include "qpreviewfiledialog.h"
#include <QGridLayout>
#include <QTreeView>
#include <QDir>
#include <QList>

QPreviewFileDialog::QPreviewFileDialog(QFilePreview * preview,QWidget *parent)
    : QFileDialog(parent) {
    setOptions(QFileDialog::DontUseNativeDialog | options());

    gl = dynamic_cast<QGridLayout*>(layout());
    if (gl && preview && preview->widget()) {
            int col = gl->columnCount();
            gl->addWidget(preview->widget(), 0, col, gl->rowCount(), 1,Qt::AlignCenter);
            gl->setColumnStretch(col, 0);
    }

    list = findChild<QTreeView *>();
    if ((list != NULL) && preview && preview->widget()) {
        connect(list->selectionModel(),SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),this,SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
    }
    this->preview = preview;
}

QPreviewFileDialog::~QPreviewFileDialog() {
}

void QPreviewFileDialog::selectionChanged(const QItemSelection & /*selected*/, const QItemSelection & /*deselected*/) {
    QStringList selItems = selectedFiles();
    QUrl url;

    if (selItems.count() > 0) {
        url = QUrl::fromLocalFile(selItems.at(0));
    }

    preview->previewUrl(url);
}
