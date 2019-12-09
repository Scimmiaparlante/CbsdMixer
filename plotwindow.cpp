#include "plotwindow.h"
#include "ui_plotwindow.h"

#include <iostream>


PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
{
    ui->setupUi(this);

    ui->preFrequencyPlot->setAxisAutoScale(QwtPlot::yLeft, false);
    ui->preFrequencyPlot->setAxisScale(QwtPlot::yLeft, -1000000, 1000000);

    //mixer startup
    myMixer = new Mixer(std::vector<float>());
    mixerThread = new std::thread(&Mixer::start, myMixer);

    curve = new QwtPlotCurve();
    curve->setTitle( "Some Points" );
    curve->setPen( Qt::blue, 1 );
    curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    curve->attach( ui->preFrequencyPlot );

    plotTimer = new QTimer(nullptr);
    connect(plotTimer, SIGNAL(timeout()), this, SLOT(replot()));
    plotTimer->start(50);
}

PlotWindow::~PlotWindow()
{
    delete ui;
    delete myMixer;
    delete plotTimer;
    delete mixerThread;
}


void PlotWindow::replot()
{
    double* data = myMixer->get_rawFrequencies();
    QPolygonF points;
    for (long int i = 0; i < COMP_SAMPLES; ++i)
    {
        points << QPointF( static_cast<double>(SAMPLE_RATE)/NUM_SAMPLES*i, data[i] );
    }
    curve->setSamples( points );
    ui->preFrequencyPlot->setAxisScale(QwtPlot::xBottom, 0, SAMPLE_RATE/2);

    ui->preFrequencyPlot->replot();
}
