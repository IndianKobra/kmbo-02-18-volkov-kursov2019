/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <math.h>

#include <QKeyEvent>
#include <QRandomGenerator>
#include <vector>
using namespace std;

class MyPoint
{
public:
int x, y;
MyPoint(int arg1, int arg2){x=arg1;y=arg2;}
MyPoint operator+(MyPoint P){return MyPoint(P.x+x, P.y+y);}
};

class PrivatisationGame;
class PrivatisationPlayer
{
    private:
        PrivatisationPlayer *NextPlayer;
        int Score = 0, number = 1, type = 1, life = 3;//1 - real player, 2 - bot, 0 - dead
        PrivatisationPlayer(){NextPlayer = this;}
        PrivatisationPlayer(PrivatisationPlayer *PreviousPlayer)
        {
            NextPlayer = PreviousPlayer->NextPlayer;
            PreviousPlayer->NextPlayer = this;
            number = PreviousPlayer->number+1;
        }
        void RemovePrivatisationPlayer()
        {
            PrivatisationPlayer *PreviousPlayer = this;
            while(PreviousPlayer->NextPlayer != this) PreviousPlayer = PreviousPlayer->NextPlayer;
            PreviousPlayer->NextPlayer = NextPlayer;
            type = 0;
        }
    public:
        friend PrivatisationGame;
};
template<typename Type>
class Lvalue
{
private:
    Type* value;
public:
    Lvalue(Type *arg){value = arg;}
    operator Type() {return *value;}
    Type operator=(Type NewValue) {return *value = NewValue;}
};
class PrivatisationMap
{
    private:
        vector<vector<int>> Map;
        size_t N, M;
        PrivatisationMap(){}
        PrivatisationMap(size_t n, size_t m)
        {
            N = n; M = m;
            Map.resize(size_t(n));
            for(size_t i = 0; i < n; i++) Map[i].resize(size_t(m), 0);
        }
        Lvalue<int> operator[](QPoint r){return Lvalue<int>(&(Map[size_t(r.x())][size_t(r.y())]));}
    public:
        friend PrivatisationGame;
};
class PrivatisationNew
{
    //private:
    protected:
        vector<QPoint> New;
        PrivatisationNew() {GenerateNewItem();}
        void GenerateNewItem()
        {
            int NewX = rand() % 6 + 1, NewY = rand() % 6 + 1;
            New.resize(size_t(NewY*NewX));
            for(int i = 0; i < NewX; i++) for(int j = 0; j < NewY; j++) New[size_t(i*NewY+j)] = QPoint(i, j);
        }
        QPoint operator[](size_t i){return New[i];}
        virtual size_t size()const{return New.size();}
    public:
        friend PrivatisationGame;
};
class QTPrivatisationNew: public QGraphicsItem, public PrivatisationNew
{
    public:
    GraphWidget *graph;
   //size_t size(){return New.size();}
    QTPrivatisationNew(GraphWidget *graphWidget):PrivatisationNew()
    {
        graph = graphWidget;
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
        setCacheMode(DeviceCoordinateCache);
        setZValue(-1);
    }
    QPainterPath shape() const
    {
        QPainterPath path;
        for(size_t i = 0; i < size(); i++)path.addRect(((this)->[i]).x()*10, ((this)->[i]).y()*10, ((this)->[i]).x()*10+30, ((this)->[i].y())*10+30);
        return path;
    }
    //! [9]

    //! [10]
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) const
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::darkGray);
        static const QPointF points[4] = {
              QPointF(-10.0, -10.0),
              QPointF(10.0, -10.0),
              QPointF(10.0, 10.0),
              QPointF(-10.0, 10.0)
          };
        //painter->drawConvexPolygon(points, 4);
        painter->drawRect(-7, -7, 20, 20);

        QRadialGradient gradient(-3, -3, 10);
        if (option->state & QStyle::State_Sunken) {
            gradient.setCenter(3, 3);
            gradient.setFocalPoint(3, 3);
            gradient.setColorAt(1, QColor(Qt::yellow).lighter(120));
            gradient.setColorAt(0, QColor(Qt::darkYellow).lighter(120));
        } else {
            gradient.setColorAt(0, Qt::yellow);
            gradient.setColorAt(1, Qt::darkYellow);
        }
        painter->setBrush(gradient);

        painter->setPen(QPen(Qt::black, 0));
        for(size_t i = 0; i < size(); i++)painter->drawRect((this)->[i].x()*10, (this)->[i].y()*10, (this)->[i].x()*10+30, (this)->[i].y()*10)+30;
        //painter->drawRect(-10, -10, 20, 20);
        //painter->drawConvexPolygon(points, 4);
    }
};
class PrivatisationGame
{
    public:
        vector<PrivatisationPlayer*> Players;
        PrivatisationPlayer* ActivePlayer;
        PrivatisationNew New;
        PrivatisationMap Map;
        PrivatisationGame(int n, int m, int NumberOfPlayers)
        {
            Map = PrivatisationMap(size_t(n) , size_t(m));
            srand (time(0));
            ActivePlayer = new PrivatisationPlayer();
            Players.push_back(ActivePlayer);
            for(size_t i = 0; i < NumberOfPlayers-1; i++) Players.push_back(new PrivatisationPlayer(*Players[i]));
        }
        bool IsTurnPossible(QPoint r, bool FirsTurn = false)
        {
            for(size_t i = 0; i < New.size(); i++)
            {
                if((New[i]+r).x()<0||(New[i]+r).x()>=Map.N) return false;
                if((New[i]+r).y()<0||(New[i]+r).y()>=Map.M) return false;
                if(Map[New[i]+r]!=0) return false;
            }
            if(FirsTurn) return true;
            bool Conected = FirsTurn;
            vector<QPoint> Conecting({QPoint(1, 0), QPoint(-1, 0), QPoint(0, 1), QPoint(0, -1)});
            for(size_t i = 0; i < New.size(); i++) for(size_t j = 0; j < Conecting.size(); j++)
            {
                QPoint P = New[i]+Conecting[j];
                if((P.x() >= 0 && P.x() < Map.N) && (P.y() >= 0 && P.y() < Map.M)
                    &&Map[P] == ActivePlayer->number) Conected = true;
            }
            return Conected;
        }
        bool AddItem(QPoint r, bool FirsTurn = false)
        {
            if(!IsTurnPossible(r, FirsTurn)) return false;
            for(size_t i = 0; i < New.size(); i++) Map[New[i] + r] = ActivePlayer->number;
            ActivePlayer->Score+=New.size();
            ActivePlayer=ActivePlayer->NextPlayer;
            New.GenerateNewItem();
            return true;
        }
        void EndGame()
        {

        }
        void SkipTurn()
        {
            ActivePlayer->life--;
            if(ActivePlayer->life<=0)ActivePlayer->RemovePrivatisationPlayer();
            if(ActivePlayer==ActivePlayer->NextPlayer)EndGame();
            ActivePlayer=ActivePlayer->NextPlayer;
        }
        ~PrivatisationGame(){for(size_t i = 0; i < Players.size(); i++) delete Players[i];}
    private:
};

//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Elastic Nodes"));
//! [0]
    //PrivatisationGame Game(20, 30, 1);
    QTPrivatisationNew New(this);
//! [1]
    Node *node1 = new Node(this);
    Node *node2 = new Node(this);
    Node *node3 = new Node(this);
    Node *node4 = new Node(this);
    centerNode = new Node(this);
    Node *node6 = new Node(this);
    Node *node7 = new Node(this);
    Node *node8 = new Node(this);
    Node *node9 = new Node(this);
    /*scene->addItem(node1);
    scene->addItem(node2);
    scene->addItem(node3);
    scene->addItem(node4);
    scene->addItem(centerNode);
    scene->addItem(node6);
    scene->addItem(node7);
    scene->addItem(node8);
    scene->addItem(node9);
    //scene->addItem(new Edge(node1, node2));
    scene->addItem(new Edge(node2, node3));
    scene->addItem(new Edge(node2, centerNode));
    scene->addItem(new Edge(node3, node6));
    //scene->addItem(new Edge(node4, node1));
    scene->addItem(new Edge(node4, centerNode));
    scene->addItem(new Edge(centerNode, node6));
    scene->addItem(new Edge(centerNode, node8));
    scene->addItem(new Edge(node6, node9));
    scene->addItem(new Edge(node7, node4));
    scene->addItem(new Edge(node8, node7));
    scene->addItem(new Edge(node9, node8));*/

    node1->setPos(-50, -50);
    node2->setPos(0, -50);
    node3->setPos(50, -50);
    node4->setPos(-50, 0);
    centerNode->setPos(0, 0);
    node6->setPos(50, 0);
    node7->setPos(-50, 50);
    node8->setPos(0, 50);
    node9->setPos(50, 50);
}
//! [1]

//! [2]
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
//! [2]

//! [3]
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    const QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : items) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    for (Node *node : qAsConst(nodes))
        node->calculateForces();

    bool itemsMoved = false;
    for (Node *node : qAsConst(nodes)) {
        if (node->advancePosition())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}
//! [4]

#if QT_CONFIG(wheelevent)
//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}
//! [5]
#endif

//! [6]
void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Privatiosation game"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}
//! [6]

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]

void GraphWidget::shuffle()
{
    const QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : items) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
