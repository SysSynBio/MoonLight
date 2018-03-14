#include "graphe2.h"
#include "ui_graphe2.h"

Graphe2::Graphe2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Graphe2)
{
    ui->setupUi(this);

    BigPlot = new QwtPlot(ui->widgetGraphe);
    BigPlot->resize(ui->widgetGraphe->width(), ui->widgetGraphe->height());
    curveData = new QwtPlotCurve("Essai Titre");
    BigPlot->axisWidget(QwtPlot::yLeft)->setFont(QFont(QString("Arial"),7));
    BigPlot->axisWidget(QwtPlot::xBottom)->setFont(QFont(QString("Arial"),7));
}

Graphe2::~Graphe2()
{
    delete ui;
}

void Graphe2::Plot(vector<double> y_to_plot, vector<double> x_to_plot, QString _titre){
    int size = y_to_plot.size();
    if((int) x_to_plot.size() < size) {cerr << "Graphe1::Plot , error, the vector of x points is smaller than the vector of y points\n "; return;}
    x_to_plot.resize(size);

    cerr << "Size of data plotted : " << size << endl;
    /*for(int i = 0; i < size; ++i){
        cerr << i << "\t" << x_to_plot[i] << "\t" << y_to_plot[i] << endl;
    }*/

    Data.clear();
    Data = vector<double>(y_to_plot);
    xpoints.clear();
    xpoints = vector<double>(x_to_plot);
    if(curveData){
        curveData->detach();
        delete curveData;
    }
        curveData = new QwtPlotCurve(_titre.toStdString().c_str());
        curveData->setPen(QColor(Qt::darkBlue));
        curveData->setStyle(QwtPlotCurve::Lines);
        curveData->setRenderHint(QwtPlotItem::RenderAntialiased);

        curveData->setSamples(&xpoints[0], &Data[0], size);    // I hope
        curveData->attach(BigPlot);

        //BigPlot->setAxisScale(2, xpoints[0], xpoints[xpoints.size()-1]);
        //BigPlot->setAxisScale(3, xpoints[0], xpoints[xpoints.size()-1]);
        BigPlot->replot();

}

void Graphe2::Example(){
    double p1[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5};
    double p2[] = {0, 0.5, 0.8, 0.3, 0.2, 0.1};
    vector<double> xs = vector<double>(p1, &p1[5]);
    vector<double> ys = vector<double>(p2, &p2[5]);
    Plot(ys, xs, QString("Example of plot"));
}
