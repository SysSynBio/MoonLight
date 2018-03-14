#ifndef GRAPHE2_H
#define GRAPHE2_H

#include <QWidget>

#include "qwt6.1.0/qwt_plot.h"
#include "qwt6.1.0/qwt_scale_map.h"
#include "qwt6.1.0/qwt_color_map.h"
#include "qwt6.1.0/qwt_symbol.h"
#include "qwt6.1.0/qwt.h"
#include "qwt6.1.0/qwt_plot_curve.h"
#include "qwt6.1.0/qwt_color_map.h"
#include "qwt6.1.0/qwt_scale_widget.h"
#include "qwt6.1.0/qwt_scale_draw.h"
#include "qwt6.1.0/qwt_plot_zoomer.h"
#include "qwt6.1.0/qwt_plot_panner.h"
#include "qwt6.1.0/qwt_plot_layout.h"

#include <iostream>
#include <vector>
using namespace std;

namespace Ui {
class Graphe2;
}

class Graphe2 : public QWidget
{
    Q_OBJECT
    
public:
    explicit Graphe2(QWidget *parent = 0);
    ~Graphe2();

    vector<double> Data;
    vector<double> xpoints;
    QwtPlot* BigPlot;
    QwtPlotCurve* curveData;


    void Plot(vector<double> y_to_plot, vector<double> x_to_plot, QString _titre);
    void Example();

private:
    Ui::Graphe2 *ui;
};

#endif // GRAPHE2_H
