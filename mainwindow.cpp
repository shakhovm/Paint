#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QInputDialog>
#include <QColorDialog>
#include <QDebug>
#include <vector>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    image = new QImage(QApplication::desktop()->size(),  QImage::Format_RGBA8888_Premultiplied);

    painter = new QPainter(image);
    painter->fillRect(image->rect(), Qt::white);

    //Add image to last changes
    previous_images.push_back(*image);
    new_image = *image;
    main_colour = QColor(Qt::black);
    main_pen = new QPen(main_colour);
    main_pen->setCapStyle(Qt::RoundCap);
    main_pen->setWidth(pen_size);
    painter->setPen(*main_pen);
    ui->actionPen->toggle();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete image;
    delete main_pen;
}

bool MainWindow::saveFile()
{

    QString path = QDir::currentPath() + "/image" + ".png";
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), path,
                                                    tr("PNG Files (*.png)"));

    if(image->save(fileName, "png"))
    {
        return true;
    }
}

void MainWindow::undo()
{
    //If previous changes exist remove last ones
    if(previous_images.size() > 1)
    {
        previous_images.pop_back();
        new_image = previous_images.back();
        painter->drawImage(0, 0, new_image);
        this->update();
    }

}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter* new_painter = new QPainter(this);
    new_painter->drawImage(0, ui->menuBar->height(), *image);
    new_painter->end();
}


void MainWindow::fillBucket(QPoint point, QColor current_color)
{
    int height = image->height();
    int width = image->width();
    std::vector<QPoint> points;
    points.push_back(point);
    while(!points.empty())
    {
        QPoint current_point = points.back();
        painter->drawPoint(current_point);
        points.pop_back();
        int current_x = current_point.x();
        int current_y = current_point.y();
        for(int i = current_x - 1; i < current_x + 2; ++i)
        {
            for(int j = current_y - 1; j < current_y + 2; ++j)
            {
                if(i >= 0 && i < width && j >= 0 && j < height)
                {
                    QPoint new_point(i, j);
                    if(QColor(image->pixelColor(new_point)) == current_color)
                    {
                        points.push_back(new_point);
                    }
                }

            }
        }
    }
}

void MainWindow::drawRectangle(QPoint first_point, QPoint last_point)
{
    painter->drawImage(0, 0, new_image);
    new_image = *image;
    QRect rect(first_point.x(), first_point.y(), last_point.x() - first_point.x(), last_point.y() - first_point.y());
    painter->drawRect(rect);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    start_position = event->pos() - QPoint(0, ui->menuBar->height());

    //Make change according to current tool
    switch(main_tool) {
    case (ToolPen):
    {
        painter->drawPoint(start_position);
        break;
    }

    case (ToolBucket):
    {
        QColor current_color = image->pixelColor(start_position);
        if(current_color == main_colour)
            return;
        main_pen->setWidth(1);
        painter->setPen(*main_pen);

        fillBucket(start_position, current_color);
        main_pen->setWidth(pen_size);
        painter->setPen(*main_pen);
        break;
    }

    case (ToolRectangle):
    {
        painter->drawPoint(start_position);
        break;
    }

    default:
    {
        painter->drawPoint(start_position);
        break;
    }


    }


    this->update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //Make change according to current tool
    QPoint last_position = event->pos() - QPoint(0, ui->menuBar->height());
    if(main_tool == ToolBucket)
        return;

    if(main_tool == ToolRectangle)
    {
        drawRectangle(start_position, last_position);
    }

    else if(main_tool == ToolPen)
    {

        painter->drawLine(start_position, last_position);
        start_position = last_position;
    }

    this->update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    new_image = *image;

    //add last changes to All changes
    previous_images.push_back(new_image);


    //If there are too many changes remove the first one
    if(previous_images.size() >= 10)
    {
        previous_images.pop_front();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    quit();
}

void MainWindow::on_actionWidth_triggered()
{
    pen_size = QInputDialog::getInt(this, "Width of pen", "pen", pen_size, 2);
    pen_size = pen_size >= 2 ? pen_size : 2;
    main_pen->setWidth(pen_size);
    painter->setPen(*main_pen);

}

void MainWindow::on_actionColour_triggered()
{
    main_colour = QColorDialog::getColor(Qt::black, this, "colour");
    main_pen->setColor(main_colour);
    painter->setPen(*main_pen);
}

void MainWindow::check_toggled()
{

    switch(main_tool)
    {
    case(ToolPen):
        ui->actionPen->toggle();
        break;

    case(ToolBucket):
        ui->actionBucket->toggle();
        break;

    case(ToolRectangle):
        ui->actionRectangle->toggle();
        break;

    default:
        ui->actionPen->toggle();
        break;
    }

}
void MainWindow::on_actionPen_triggered()
{
    check_toggled();
    main_tool = ToolPen;
}

void MainWindow::on_actionRectangle_triggered()
{
    check_toggled();
    main_tool = ToolRectangle;
}


void MainWindow::on_actionBucket_triggered()
{
    check_toggled();
    main_tool = ToolBucket;
}

void MainWindow::on_actionSave_triggered()
{
    saveFile();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr ("Open image file"), "",
                                                        tr ("Jpeg files (*.jpg *.jpeg );; "
                                                            "PNG files (*.png )"));
    if ( !fileName.isEmpty() )
    {
        QPixmap* loaded_image = new QPixmap;

        loaded_image->load(fileName);

        if (!loaded_image->isNull () )
        {
            painter->drawImage(0, 0, loaded_image->toImage());
            this->update();
        }
        else {
            QMessageBox::information(this, tr ("Paint"),
            tr ("Failed to load %1.").arg(fileName ));
        }

        return;
    }
}

void MainWindow::quit()
{
    QMessageBox quitMsg;
    quitMsg.setWindowTitle("Paint");
    quitMsg.setText("Do you really want to quit?");
    quitMsg.setStandardButtons(QMessageBox::Yes |
                               QMessageBox::Cancel);
    quitMsg.setDefaultButton(QMessageBox::Cancel);
    if (quitMsg.exec() == QMessageBox::Yes)
        QApplication::instance()->quit();
}


void MainWindow::on_actionUndo_triggered()
{
    undo();
}

void MainWindow::on_actionQuit_triggered()
{
    quit();
}
