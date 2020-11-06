#ifndef THERMALTHREAD_H
#define THERMALTHREAD_H

#include <qobject.h>
#include <qthread.h>
#include <QString>
#include <qdebug.h>
#include <qimage.h>
#include <QMutex>
#include <QMutexLocker>
#include <QPainter>
#include <QPainterPath>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include <memory>
#include "customtools.h"
#include "streamthread.h"
#include "networkclient.h"

extern QImage *streamFrame;
extern QMutex *thermalMutex;

class ThermalThread : public StreamThread
{
    Q_OBJECT


public:
    ThermalThread(bool* stream, const QString & ssrc, Extreme* max, Extreme* center, Extreme* min, bool* showVals, QTypedArrayData<QString>::const_iterator* zoom, const QString* type)
       : StreamThread (stream, ssrc), _maximum(max), _center(center), _minimum(min), _showVals(showVals), _zoom(zoom), _type(type)
    {}


protected:
    virtual void run() override;

private:
    void zoomImage(cv::Mat& mat);
    void drawExtremes();
    bool* _showVals;;
    int _skipX = 0, _skipY = 0;
    Extreme* _maximum;
    Extreme* _minimum;
    Extreme* _center;
    QTypedArrayData<QString>::const_iterator* _zoom;
    const QString* _type;
    const Values _vals;
};

#endif // THERMALTHREAD_H
