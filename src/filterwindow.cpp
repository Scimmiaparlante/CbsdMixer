#include "filterwindow.h"

#include <qwt_plot.h>
#include <qwt_scale_engine.h>

FilterWindow::FilterWindow(int x_min, int x_max, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::FilterWindow)
{
    ui->setupUi(this);

    //windows on top of the other
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint );
    move(100,100);

    //set up the first plot
    ui->filterPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
    ui->filterPlot->setAxisScale(QwtPlot::xBottom, x_min, x_max);
    ui->filterPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLogScaleEngine);

    curve_filter = new QwtPlotCurve();
    curve_filter->setPen(Qt::blue, 1);
    curve_filter->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve_filter->attach(ui->filterPlot);
}

FilterWindow::~FilterWindow()
{
    delete ui;
    delete curve_filter;
}


