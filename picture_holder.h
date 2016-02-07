#include <QLabel>
#include <QMenu>

class picture_holder : public QLabel
{
Q_OBJECT

public:
    picture_holder(QWidget *parent = 0);
    bool selectionStarted;
    QPixmap original ;
    QPixmap resetMap ;
    double scaleFactor = 1;

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    QRect selectionRect;

//private slots:
//    void saveSlot();

};

