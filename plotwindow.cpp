#include "plotwindow.h"
#include "ui_plotwindow.h"

#include "qwt_plot.h"
#include "qwt_plot_curve.h"

PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
{
    ui->setupUi(this);

    QwtPlotCurve *curve = new QwtPlotCurve();
    curve->setTitle( "Some Points" );
    curve->setPen( Qt::blue, 2 );
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    QPolygonF points;
    for (int i = 0; i < 1000; i++)
        points << QPointF( i, i );
    curve->setSamples( points );

    curve->attach( ui->preFrequencyPlot );



}

PlotWindow::~PlotWindow()
{
    delete ui;
}
