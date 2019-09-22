#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //control the width of pen
    void on_actionWidth_triggered();

    //control the colour of pen
    void on_actionColour_triggered();

    //select tool rectangle
    void on_actionRectangle_triggered();

    //select tool pen
    void on_actionPen_triggered();

    //select tool bucket
    void on_actionBucket_triggered();

    //Save image
    void on_actionSave_triggered();

    //Open image
    void on_actionOpen_triggered();

    //Remove last change
    void on_actionUndo_triggered();

    //Quit program
    void on_actionQuit_triggered();

private:
    Ui::MainWindow *ui;

    //Extra image
    QImage new_image;

    //Paint tools
    enum Tools{ToolPen, ToolBucket, ToolRectangle};

    //Main image
    QImage* image;

    //Images saved for removing last changes
    QVector<QImage> previous_images;

    //Main painter
    QPainter* painter;


    QPoint start_position;
    QPoint end_position;

    QColor main_colour;

    QPen* main_pen;

    int pen_size = 3;

    //Current tool
    int main_tool = ToolPen;

    //Use of paint tool bucket
    void fillBucket(QPoint point, QColor current_color);

    //Use of paint tool rectangle
    void drawRectangle(QPoint first_point, QPoint last_point);

    //Check toggled of the tools
    void check_toggled();

    bool saveFile();

    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *event);

    void quit();

    //Remove last changes
    void undo();
};

#endif // MAINWINDOW_H
