#ifndef SHOWPNGPICTURE_HH
#define SHOWPNGPICTURE_HH

#include <QWidget>

class QTimer;
class QImage;

namespace Ui {
class ShowPngPicture;
}

class ShowPngPicture : public QWidget
{
    Q_OBJECT

public:
    explicit ShowPngPicture(int width, int height, QWidget* parent = 0);
    ~ShowPngPicture();

    void presetBottomUp(bool order);

private:
    Ui::ShowPngPicture* ui;
    int picWidth;
    int picHeight;
    int col;
    int row;
    bool reverseOrder;

    QImage* p;
    QRgb* line;
    QWidget* displayWidget;
    //    QTimer *displayTimer;

    void setNextPixel(int);
    bool saveImage(QString);


public slots:
    void setPixel(const unsigned char*, int);
    void saveImage2File();
    void storeReverse(bool order);

protected:
    void paintEvent(QPaintEvent*);
};

#endif // SHOWPNGPICTURE_HH
