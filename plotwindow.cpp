#include "plotwindow.h"
#include "ui_plotwindow.h"

#include <iostream>


PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow)
{
    ui->setupUi(this);

    ui->preFrequencyPlot->setAxisAutoScale(QwtPlot::yLeft, false);
    ui->preFrequencyPlot->setAxisScale(QwtPlot::yLeft, -30000, 30000);

    //mixer startup
    myMixer = new Mixer(std::vector<float>());
    mixerThread = new std::thread(&Mixer::start, myMixer);

    time = 0;

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
    int16_t* data = myMixer->get_rawData();
    QPolygonF points;
    for (long int i = 0; i < NUM_SAMPLES; ++i)
    {
        points << QPointF( time, static_cast<double>(data[i]) );
        time++;
    }
    curve->setSamples( points );
    ui->preFrequencyPlot->setAxisScale(QwtPlot::xBottom, time - NUM_SAMPLES, time);

    ui->preFrequencyPlot->replot();
}
