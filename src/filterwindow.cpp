#include "filterwindow.h"

#include <qwt_plot.h>
#include <qwt_scale_engine.h>

FilterWindow::FilterWindow(double resolution_, unsigned int num_elem_, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::FilterWindow)
{
    ui->setupUi(this);

    resolution = resolution_;
    num_elem = num_elem_;

    //windows on top of the other
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint );
    move(100,100);

    //set up the first plot
    ui->filterPlot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
    ui->filterPlot->setAxisScale(QwtPlot::xBottom, resolution, num_elem*resolution);
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


void FilterWindow::replot(double* filter)
{
    QPolygonF points3;
    for (long int i = 0; i < num_elem; ++i)
        points3 << QPointF(resolution*i, filter[i]);

    curve_filter->setSamples( points3 );

    ui->filterPlot->replot();
}


