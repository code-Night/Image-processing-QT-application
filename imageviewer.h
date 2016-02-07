#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#ifndef QT_NO_PRINTER
//#include <QPrinter>
#endif
#include "picture_holder.h"
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer();
    bool loadFile(const QString &);
//    QPixmap original ;



private slots:
    void open();
    void print();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void about();
    void rotate_with_angle(int a);
    void rotate_custom_slot();
    void undo_slot();
    void redo_slot();
    void saveSlot();
    void reset_slot();
    void saveAs_slot();

private:
    void createActions();
    void zoomImg(double s);
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

//    QLabel *img ;
    picture_holder *img ;
    QScrollArea *scrollArea;
//    double scaleFactor = 1;
    bool ok ;
    int angle ;



    QAction *openAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *crop ;
    QAction *rotate_left_90 ;
    QAction *rotate_right_90 ;
    QAction *rotate_custom ;
    QAction *undo ;
    QAction *redo ;
    QAction *reset ;
    QAction *saveAs ;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *rotateMenu ;
    QMenu *helpMenu;

    QList <QPixmap> undo_stack ;
    QList <QPixmap> redo_stack ;



//    double scaleFactor =1;
};

#endif
