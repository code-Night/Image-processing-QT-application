#include <QtWidgets>
#ifndef QT_NO_PRINTER
//#include <QPrintDialog>
#endif

#include "imageviewer.h"
#include <iostream>

ImageViewer::ImageViewer()
{
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    img = new picture_holder(this) ;
    img->setBackgroundRole(QPalette::Base);
    img->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    img->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Base);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(img);

    setCentralWidget(scrollArea);
//    setCentralWidget(x);
    createActions();
    createMenus();

    resize(QGuiApplication::primaryScreen()->availableSize());
}


bool ImageViewer::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
//    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        img->setPixmap(QPixmap());
        img->adjustSize();
        return false;
    }
    img->setPixmap(QPixmap::fromImage(image));
    img->original = *(img->pixmap());
    img->resetMap = img->original ;
    img->scaleFactor = 1.0;

    printAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        img->adjustSize();

    setWindowFilePath(fileName);
    return true;
}



void ImageViewer::open()
{
    img->scaleFactor = 1;
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::print()
{
//    Q_ASSERT(img->pixmap());
//#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
//    QPrintDialog dialog(&printer, this);
//    if (dialog.exec()) {
//        QPainter painter(&printer);
//        QRect rect = painter.viewport();
//        QSize size = img->pixmap()->size();
//        size.scale(rect.size(), Qt::KeepAspectRatio);
//        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
//        painter.setWindow(img->pixmap()->rect());
//        painter.drawPixmap(0, 0, *img->pixmap());
//    }
//#endif
}
void ImageViewer::zoomImg(double s){
   img->scaleFactor *= s ;
   undo_stack.push_back(*(img->pixmap()));
   redo_stack.clear();
   QImage ci (img->original.toImage());
   QPixmap nwmp(QPixmap::fromImage(ci.scaled(ci.width()*img->scaleFactor , ci.height()*img->scaleFactor, Qt::IgnoreAspectRatio)));
   img->resize(nwmp.size());
   img->setPixmap(nwmp);
}
void ImageViewer::zoomIn()
{
    zoomImg(1.1);

}

void ImageViewer::zoomOut()
{

    zoomImg((double)(10.0/11.0));
}

void ImageViewer::normalSize()
{
    img->scaleFactor = 1 ;
    zoomImg(1.0);
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateActions();
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
//    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    printAct = new QAction(tr("&Print..."), this);
//    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
//    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (10%)"), this);
//    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (10%)"), this);
//    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
//    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
//    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    crop = new QAction(tr("Crop"),this);
    crop->setEnabled(false);
    connect(crop,SIGNAL(triggered()),this,SLOT(saveSlot()));

    rotate_left_90 = new QAction(tr("Rotate left"),this);
    rotate_left_90->setEnabled(false);
    rotate_right_90 = new QAction(tr("rotate Right"),this);
    rotate_right_90->setEnabled(false);
    QSignalMapper *sm = new QSignalMapper(this);
    connect(rotate_left_90,SIGNAL(triggered()),sm,SLOT(map()));
    connect(rotate_right_90,SIGNAL(triggered()),sm,SLOT(map()));
    sm->setMapping(rotate_left_90,90);
    sm->setMapping(rotate_right_90,-90);
    connect (sm, SIGNAL(mapped(int)), this, SLOT(rotate_with_angle(int))) ;

    rotate_custom = new QAction(tr("Custom rotation"),this);
    rotate_custom->setEnabled(false);
    connect(rotate_custom,SIGNAL(triggered()),this,SLOT(rotate_custom_slot()));

    undo = new QAction(tr("Undo"),this);
    connect(undo,SIGNAL(triggered()),this,SLOT(undo_slot()));

    redo = new QAction(tr("Redo"),this);
    connect(redo,SIGNAL(triggered()),this,SLOT(redo_slot()));

    reset = new QAction(tr("Reset"),this);
    connect(reset,SIGNAL(triggered()),this,SLOT(reset_slot()));
    reset->setEnabled(false);
    QToolBar *tb = new QToolBar ;

    saveAs = new QAction(tr("Save As"),this);
    connect(saveAs,SIGNAL(triggered()),this,SLOT(saveAs_slot()));

    tb->addAction(openAct);
    tb->addAction(zoomInAct);
    tb->addAction(zoomOutAct);
    tb->addAction(crop);
    tb->addAction(rotate_left_90);
    tb->addAction(rotate_right_90);
    tb->addAction(undo);
    tb->addAction(redo);
    tb->addAction(reset);

    addToolBar(tb);
}

void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAs);
//    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
//    viewMenu->addAction(normalSizeAct);
    viewMenu->addAction(crop);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    rotateMenu = new QMenu(tr("Rotate"),this);
    rotateMenu->addAction(rotate_left_90);
    rotateMenu->addAction(rotate_right_90);
    rotateMenu->addAction(rotate_custom);

    editMenu = new QMenu(tr("Edit"),this);
    editMenu->addAction(undo);
    editMenu->addAction(redo);
    editMenu->addAction(reset);



    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(editMenu);
    menuBar()->addMenu(rotateMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

void ImageViewer::updateActions()
{
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
    rotate_left_90->setEnabled(!fitToWindowAct->isChecked());
    rotate_right_90->setEnabled(!fitToWindowAct->isChecked());
    rotate_custom->setEnabled(!fitToWindowAct->isChecked());
    crop->setEnabled(!fitToWindowAct->isChecked());
    reset->setEnabled(!fitToWindowAct->isChecked());
}
void ImageViewer::saveSlot()
{
    undo_stack.push_back(*(img->pixmap()));
    redo_stack.clear();
    img->selectionStarted = true ;
    img->setCursor(Qt::CrossCursor);
}
void ImageViewer::scaleImage(double factor)
{
//    Q_ASSERT(img->pixmap());
//    scaleFactor *= factor;
//    img->resize(scaleFactor * img->pixmap()->size());
//    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
//    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
//    zoomInAct->setEnabled(scaleFactor < 3.0);
//    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
void ImageViewer::rotate_with_angle(int a){
    std::cout << "in rot with angle  " << a << std::endl ;
    if(a!=0){
        std::cout << "in rotate" << std::endl ;
        undo_stack.push_back(*(img->pixmap()));
        redo_stack.clear();
        QPixmap pixmap(*(img->pixmap()));
        QMatrix rm;
        rm.rotate(a);
        pixmap = pixmap.transformed(rm);
        if(a == 90 || a == -90){
            std::cout << "rotating left and right " << std::endl ;
            img->setPixmap(pixmap);
            img->adjustSize();
        }else{
            int xoffset = (pixmap.width() - img->original.width()) / 2;
            int yoffset = (pixmap.height() - img->original.height()) / 2;
            pixmap = pixmap.copy(xoffset, yoffset, img->original.width(), img->original.height());
            img->setPixmap(pixmap);
        }
        img->original = *(img->pixmap());
    }
}
void ImageViewer::rotate_custom_slot(){
    std::cout << "in rot" << std::endl ;
    int ang = QInputDialog::getInt(this, tr("Custom Rotation"), tr("Enter angle"), 0, -2147483647, 2147483647, 1,&ok);
    rotate_with_angle(ang);
}
void ImageViewer::undo_slot(){
    std::cout << "in undo slot :D" << std::endl ;
    if(undo_stack.size() == 0)
        return ;
    img->resize(undo_stack.back().size());
    redo_stack.push_back(*(img->pixmap()));
    img->setPixmap(undo_stack.back());\

    undo_stack.pop_back();
    std::cout << "undo : " <<undo_stack.size() << std::endl ;
    std::cout << "redo : " <<redo_stack.size() << std::endl ;
}

void ImageViewer::redo_slot(){
    if(redo_stack.size() == 0)
        return ;
    img->resize(redo_stack.back().size());
    undo_stack.push_back(*(img->pixmap()));
    img->setPixmap(redo_stack.back());\

    redo_stack.pop_back();
    std::cout << redo_stack.size() << std::endl ;
}

void ImageViewer::reset_slot(){
    img->resize(img->resetMap.size());
    img->setPixmap(img->resetMap);
    img->original = img->resetMap ;
    img->scaleFactor = 1 ;
}

void ImageViewer::saveAs_slot(){
    if(!img->pixmap())
        return ;
    QString filename = QFileDialog::getSaveFileName(this, tr("Save As.."),"",tr("Joint Photographic Experts Group(*.jpg);;Bitmap image(*.bmp);;Portable Network Graphics(.png)") );
    if( !filename.isNull() ){
        QPixmap pixmap(*(img->pixmap()));
        std::cout << pixmap.save(filename+".png") << std::endl ;

    }
}
