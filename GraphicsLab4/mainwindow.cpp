#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPainter"
#include "QtMath"
#include "QGridLayout"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    area = new PlotArea();

    ui->method_box->addItem("");
    ui->method_box->addItem("Пошаговый алгоритм");
    ui->method_box->addItem("Алгоритм ЦДА");
    ui->method_box->addItem("Алгоритм Брезенхема");
    ui->method_box->addItem("Алгоритм Брезенхема для окружности");
    ui->method_box->addItem("Алгоритм Ву");
    ui->method_box->addItem("Алгоритм Кастла-Питвея (на доработке)");
    ui->drawButton->setEnabled(false);

    ui->grid->addWidget(area);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintSteps(int x1, int y1, int x2, int y2)
{
    const int dX = abs(x2 - x1);
    const int dY = abs(y2 - y1);
    const int sgnX = x1 < x2 ? 1 : -1;
    const int sgnY = y1 < y2 ? 1 : -1;
    float k = (sgnY * float(dY))/ (sgnX * float(dX));
    int b = y1 - k * x1;
    while(x1 != x2 || y1 != y2)
    {
        area->AddPixel(x1,y1);
        x1++;
        y1 = k * x1 + b;
    }
}

void MainWindow::paintDDA(int x1, int y1, int x2, int y2)
{
    int dX = x2 - x1;
    int dY = y2 - y1;

    int steps = abs(dX) > abs(dY) ? abs(dX) : abs(dY);
    float Xinc = dX / (float)steps;
    float Yinc = dY / (float)steps;

    float x = x1;
    float y = y1;
    for (int i = 0; i <= steps; i++) {
        area->AddPixel(round(x), round(y));
        x += Xinc;
        y += Yinc;
    }
}

void MainWindow::paintBrezenham(int x1, int y1, int x2, int y2)
{
    const int dX = abs(x2 - x1);
    const int dY = abs(y2 - y1);
    const int sgnX = x1 < x2 ? 1 : -1;
    const int sgnY = y1 < y2 ? 1 : -1;
    int error = dX - dY;
    area->AddPixel(x2, y2);
    while(x1 != x2 || y1 != y2)
    {
        area->AddPixel(x1, y1);
        int error2 = error * 2;
        if(error2 > -dY){
            error -= dY;
            x1 += sgnX;
        }
        if(error2 < dX){
            error += dX;
            y1 += sgnY;
        }
    }
}

void MainWindow::paintCircleBrezenham(int x0, int y0, int radius)
{
    int x = 0;
    int y = radius;
    int delta = 1 - 2 * radius;
    int error = 0;
    while(y >= 0) {
        area->AddPixel(x0 + x, y0 + y);
        area->AddPixel(x0 + x, y0 - y);
        area->AddPixel(x0 - x, y0 + y);
        area->AddPixel(x0 - x, y0 - y);
        error = 2 * (delta + y) - 1;
        if(delta < 0 && error <= 0) {
            ++x;
            delta += 2 * x + 1;
            continue;
        }
        error = 2 * (delta - x) - 1;
        if(delta > 0 && error > 0) {
            --y;
            delta += 1 - 2 * y;
            continue;
        }
        ++x;
        delta += 2 * (x - y);
        --y;
    }
}

void MainWindow::paintWu(float x0, float y0, float x1, float y1)
{
    auto ipart = [](float x) -> int {return int(std::floor(x));};
        auto round = [](float x) -> float {return std::round(x);};
        auto fpart = [](float x) -> float {return x - std::floor(x);};
        auto rfpart = [=](float x) -> float {return 1 - fpart(x);};

        const bool steep = abs(y1 - y0) > abs(x1 - x0);
        if (steep) {
            std::swap(x0,y0);
            std::swap(x1,y1);
        }
        if (x0 > x1) {
            std::swap(x0,x1);
            std::swap(y0,y1);
        }

        const float dx = x1 - x0;
        const float dy = y1 - y0;
        const float gradient = (dx == 0) ? 1 : dy/dx;

        int xpx11;
        float intery;
        {
            const float xend = round(x0);
            const float yend = y0 + gradient * (xend - x0);
            const float xgap = rfpart(x0 + 0.5);
            xpx11 = int(xend);
            const int ypx11 = ipart(yend);
            if (steep) {
                area->AddPixel(ypx11,     xpx11, rfpart(yend) * xgap);
                area->AddPixel(ypx11 + 1, xpx11,  fpart(yend) * xgap);
            } else {
                area->AddPixel(xpx11, ypx11,    rfpart(yend) * xgap);
                area->AddPixel(xpx11, ypx11 + 1, fpart(yend) * xgap);
            }
            intery = yend + gradient;
        }

        int xpx12;
        {
            const float xend = round(x1);
            const float yend = y1 + gradient * (xend - x1);
            const float xgap = rfpart(x1 + 0.5);
            xpx12 = int(xend);
            const int ypx12 = ipart(yend);
            if (steep) {
                area->AddPixel(ypx12,     xpx12, rfpart(yend) * xgap);
                area->AddPixel(ypx12 + 1, xpx12,  fpart(yend) * xgap);
            } else {
                area->AddPixel(xpx12, ypx12,    rfpart(yend) * xgap);
                area->AddPixel(xpx12, ypx12 + 1, fpart(yend) * xgap);
            }
        }

        if (steep) {
            for (int x = xpx11 + 1; x < xpx12; x++) {
                area->AddPixel(ipart(intery),     x, rfpart(intery));
                area->AddPixel(ipart(intery) + 1, x,  fpart(intery));
                intery += gradient;
            }
        } else {
            for (int x = xpx11 + 1; x < xpx12; x++) {
                area->AddPixel(x, ipart(intery),     rfpart(intery));
                area->AddPixel(x, ipart(intery) + 1,  fpart(intery));
                intery += gradient;
            }
        }
}

void MainWindow::hideLabels()
{
    ui->label_1->hide();
    ui->lineEdit_1->hide();
    ui->label_2->hide();
    ui->lineEdit_2->hide();
    ui->label_3->hide();
    ui->lineEdit_3->hide();
    ui->label_4->hide();
    ui->lineEdit_4->hide();
}

void MainWindow::showLabels()
{
    ui->label_1->show();
    ui->label_1->setText("x1");
    ui->lineEdit_1->show();
    ui->label_2->show();
    ui->label_2->setText("y1");
    ui->lineEdit_2->show();
    ui->label_3->show();
    ui->label_3->setText("x2");
    ui->lineEdit_3->show();
    ui->label_4->show();
    ui->label_4->setText("y2");
    ui->lineEdit_4->show();
}

void MainWindow::showLabelsCircle()
{
    ui->label_1->show();
    ui->label_1->setText("x1");
    ui->lineEdit_1->show();
    ui->label_2->show();
    ui->label_2->setText("y1");
    ui->lineEdit_2->show();
    ui->label_3->show();
    ui->label_3->setText("r");
    ui->lineEdit_3->show();
    ui->label_4->hide();
    ui->lineEdit_4->hide();
}

void MainWindow::paintCastlePitvey(int x1, int y1, int x2, int y2)
{
    double x = x2 - y2;
    double y = y2;
    QString m1 = "s";
    QString m2 = "d";
    QString temp;
    while (x != y){
        if (x > y){
            x -= y;
            temp = m2;
            std::reverse(temp.begin(), temp.end());
            m2 = m1 + temp;
        }
        else {
            y -= x;
            temp = m1;
            std::reverse(temp.begin(), temp.end());
            m1 = m2 + temp;
        }
    }
    std::reverse(m1.begin(), m1.end());
    temp = m2 + m1;

    x = x1;
    y = y1;
    for (int i = 0; i < temp.size(); i++){
        area->AddPixel(x, y);
        x++;
        if(temp[i] == "d"){
            y++;
        }
    }
}


void MainWindow::on_drawButton_clicked()
{
    int k = ui->method_box->currentIndex();
    int x1 = ui->lineEdit_1->text().toInt();
    int y1 = ui->lineEdit_2->text().toInt();
    int x2 = ui->lineEdit_3->text().toInt();
    int y2 = ui->lineEdit_4->text().toInt();

    area->Clear();

    switch (k) {
    case 1:
        paintSteps(x1, y1, x2, y2);
        break;
    case 2:
        paintDDA(x1, y1, x2, y2);
        break;
    case 3:
        paintBrezenham(x1, y1, x2, y2);
        break;
    case 4:
        paintCircleBrezenham(x1, y1, x2);
        break;
    case 5:
        paintWu(float(x1), float(y1), float(x2), float(y2));
        break;
    case 6:
        paintCastlePitvey(x1, y1, x2, y2);
        break;
    default:
        break;
    }
}

void MainWindow::on_method_box_currentIndexChanged(int index)
{
    switch (index) {
    case 1:
        ui->drawButton->setEnabled(true);
        showLabels();
        break;
    case 2:
        ui->drawButton->setEnabled(true);
        showLabels();
        break;
    case 3:
        ui->drawButton->setEnabled(true);
        showLabels();
        break;
    case 4:
        ui->drawButton->setEnabled(true);
        showLabelsCircle();
        break;
    case 5:
        ui->drawButton->setEnabled(true);
        showLabels();
        break;
    case 6:
        ui->drawButton->setEnabled(false);
        showLabels();
        break;
    default:
        ui->drawButton->setEnabled(false);
        hideLabels();
        break;
    }
}
