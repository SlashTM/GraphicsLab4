#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <plotarea.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    void paintSteps(int x1, int y1, int x2, int y2);
    void paintDDA(int x1, int y1, int x2, int y2);
    void paintBrezenham(int x1, int y1, int x2, int y2);
    void paintCircleBrezenham(int x0, int y0, int radius);
    void paintWu(float x1, float y1, float x2, float y2);
    void paintCastlePitvey(int x1, int y1, int x2, int y2);

    void hideLabels();
    void showLabels();
    void showLabelsCircle();

private slots:
    void on_drawButton_clicked();

    void on_method_box_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    PlotArea *area = nullptr;
};
#endif // MAINWINDOW_H
