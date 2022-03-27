#include "showpngpicture.h"
#include "ui_showpngpicture.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>


ShowPngPicture::ShowPngPicture(int width, int height, QWidget* parent)
    : QWidget(parent), ui(new Ui::ShowPngPicture)
{
    ui->setupUi(this);
    picWidth = width;
    picHeight = height;

    ui->displayWidget->setFixedSize(picWidth + 20, picHeight + 20);

    p = new QImage(picWidth, picHeight, QImage::Format_RGB32);
    p->fill(64);
    row = 0;
    col = 0;
    line = (QRgb*)p->scanLine(row);
    reverseOrder = false;

    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveImage2File()));
    connect(ui->reverse, SIGNAL(clicked(bool)), this, SLOT(storeReverse(bool)));
}

ShowPngPicture::~ShowPngPicture() { delete ui; }

void ShowPngPicture::setPixel(const unsigned char* c, int items)
{
    int j;
    for (int i = 0; i < items; i++) {
        j = c[i];
        setNextPixel(j);
    }
}
void ShowPngPicture::saveImage2File()
{
    QString fileName;
    QString dir;
    bool ok;
    dir = QDir::homePath();

    fileName = QFileDialog::getSaveFileName(0, tr("Save Image"), dir, "*.png");
    if (!fileName.isEmpty()) {
        ok = saveImage(fileName);
        if (ok) {
            QMessageBox::information(
                0, "gr-display", QString("Image saved to file: ") + fileName);
        }
    }
}

void ShowPngPicture::setNextPixel(int i)
{

    line[col] = qRgb(i, i, i);
    if (!reverseOrder) {
        col++;
        if (col >= picWidth) {
            col = 0;
            row++;
            if (row >= picHeight)
                row = picHeight - 1; // Later scroll up
            line = (QRgb*)p->scanLine(row);
            update();
        }
    } else {
        col--;
        if (col < 0) {
            col = picWidth - 1;
            row--;
            if (row < 0)
                row = 0;
            line = (QRgb*)p->scanLine(row);
            update();
        }
    }
}
void ShowPngPicture::paintEvent(QPaintEvent*)
{

    if (p != nullptr)
        ui->displayWidget->setPixmap(QPixmap::fromImage(*p));
}
bool ShowPngPicture::saveImage(QString datei) { return p->save(datei, "PNG"); }
void ShowPngPicture::storeReverse(bool order)
{
    if (reverseOrder == order)
        return;
    reverseOrder = order;
    p->fill(64);
    if (reverseOrder) {
        row = picHeight - 1;
        col = picWidth - 1;
    } else {
        row = 0;
        col = 0;
    }
    line = (QRgb*)p->scanLine(row);
}


void ShowPngPicture::presetBottomUp(bool order)
{
    ui->reverse->setChecked(order);
    storeReverse(order);
}
