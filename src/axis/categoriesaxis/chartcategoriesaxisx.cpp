/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc
 ** All rights reserved.
 ** For any questions to Digia, please use contact form at http://qt.digia.com
 **
 ** This file is part of the Qt Commercial Charts Add-on.
 **
 ** $QT_BEGIN_LICENSE$
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.
 **
 ** If you have questions regarding the use of this file, please use
 ** contact form at http://qt.digia.com
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "chartcategoriesaxisx_p.h"
#include "chartpresenter_p.h"
#include "qbarcategoriesaxis_p.h"
#include <cmath>

static int label_padding = 5;

QTCOMMERCIALCHART_BEGIN_NAMESPACE

ChartCategoriesAxisX::ChartCategoriesAxisX(QBarCategoriesAxis *axis,ChartPresenter *presenter) : ChartAxis(axis,presenter),
m_categoriesAxis(axis)
{

}

ChartCategoriesAxisX::~ChartCategoriesAxisX()
{
}

QVector<qreal> ChartCategoriesAxisX::calculateLayout() const
{
    Q_ASSERT(m_categoriesAxis->categories().count()>=1);

    QVector<qreal> points;
    points.resize(m_categoriesAxis->categories().count()+1);

    const qreal delta = m_rect.width()/(m_categoriesAxis->categories().count());
    const qreal min = m_categoriesAxis->d_ptr->min();
    const qreal max = m_categoriesAxis->d_ptr->max();
    qreal start =-min-0.5;
    if(start<=0) {
        start = fmod(start * m_rect.width()/(max - min),delta) + delta;
    }
    else {
        start = fmod(start * m_rect.width()/(max - min),delta);
    }

    points[m_categoriesAxis->categories().count()] = m_rect.left() + m_rect.width();

    for (int i = 0; i < m_categoriesAxis->categories().count(); ++i) {
        qreal x = start + i * delta + m_rect.left();
        points[i] = x;
    }

    return points;
}

void ChartCategoriesAxisX::updateGeometry()
{
    const QVector<qreal>& layout = ChartAxis::layout();

    m_minWidth = 0;
    m_minHeight = 0;

    if(layout.isEmpty()) return;

    QStringList ticksList;

    createCategoryLabels(ticksList,m_min,m_max,m_categoriesAxis->categories());

    QList<QGraphicsItem *> lines = m_grid->childItems();
    QList<QGraphicsItem *> labels = m_labels->childItems();
    QList<QGraphicsItem *> shades = m_shades->childItems();
    QList<QGraphicsItem *> axis = m_axis->childItems();

    Q_ASSERT(labels.size() == ticksList.size());
    Q_ASSERT(layout.size() == ticksList.size());

    const qreal delta = m_rect.width()/(m_categoriesAxis->categories().count());

    QGraphicsLineItem *lineItem = static_cast<QGraphicsLineItem*>(axis.at(0));
    lineItem->setLine(m_rect.left(), m_rect.bottom(), m_rect.right(), m_rect.bottom());

    qreal width = m_rect.left();
    for (int i = 0; i < layout.size(); ++i) {
        QGraphicsLineItem *lineItem = static_cast<QGraphicsLineItem*>(lines.at(i));
        lineItem->setLine(layout[i], m_rect.top(), layout[i], m_rect.bottom());
        QGraphicsSimpleTextItem *labelItem = static_cast<QGraphicsSimpleTextItem*>(labels.at(i));
        labelItem->setText(ticksList.at(i));
        const QRectF& rect = labelItem->boundingRect();
        QPointF center = rect.center();
        labelItem->setTransformOriginPoint(center.x(), center.y());

        if(i==layout.size()-1){
            labelItem->setPos(layout[i-1] + (delta)/2 - center.x(), m_rect.bottom() + label_padding);
        }else{
            labelItem->setPos(layout[i] - (delta)/2 - center.x(), m_rect.bottom() + label_padding);
        }

        if(labelItem->pos().x()<=width || labelItem->pos().x()+ rect.width()>m_rect.right()) {
            labelItem->setVisible(false);
        }
        else {
            labelItem->setVisible(true);
            width=rect.width()+labelItem->pos().x();
        }

        m_minWidth+=rect.width();
        m_minHeight=qMax(rect.height()+label_padding,m_minHeight);

        if ((i+1)%2 && i>1) {
            QGraphicsRectItem *rectItem = static_cast<QGraphicsRectItem*>(shades.at(i/2-1));
            rectItem->setRect(layout[i-1],m_rect.top(),layout[i]-layout[i-1],m_rect.height());
        }
        lineItem = static_cast<QGraphicsLineItem*>(axis.at(i+1));
        lineItem->setLine(layout[i],m_rect.bottom(),layout[i],m_rect.bottom()+5);
    }
}

void ChartCategoriesAxisX::handleAxisUpdated()
{
    if(m_categoriesAxis->categories()!=m_categories)
    {
        m_categories=m_categoriesAxis->categories();
        if(ChartAxis::layout().count()==m_categories.size()+1) updateGeometry();
    }
    ChartAxis::handleAxisUpdated();
}

QTCOMMERCIALCHART_END_NAMESPACE