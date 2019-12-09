double* data = myMixer->get_rawData();
QPolygonF points;
for (long int i = 0; i < NUM_SAMPLES; ++i)
{
	points << QPointF( time, data[i] );
	time++;
}
curve->setSamples( points );
ui->preFrequencyPlot->setAxisScale(QwtPlot::xBottom, time - NUM_SAMPLES, time);

ui->preFrequencyPlot->replot();
