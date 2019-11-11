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

#include <QGraphicsItem>

#include <math.h>
#include <QString>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsSceneMouseEvent>
#include <vector>
using namespace std;
class PrivatisationMap;
class QTPrivatisationGame;
QTPrivatisationGame* Game;
class QTPrivatisationNew;
class MyPoint
{
    public:
    int x, y;
    MyPoint(int arg1, int arg2){x=arg1;y=arg2;}
    MyPoint operator+(MyPoint P){return MyPoint(P.x+x, P.y+y);}
};
template<typename Type = int>
class Lvalue
{
private:
    Type* value;
public:
    Lvalue(Type *arg){value = arg;}
    operator Type() {return *value;}
    Type operator=(Type NewValue) {return *value = NewValue;}
};
string Int2Str(int n)
{
    string Ans = "";
    do{
        Ans = char('0'+(n%10)) + Ans;
        n/=10;
    }while(n>0);
    return Ans;
}

class PrivatisationGame;
class PrivatisationPlayer
{
    protected:
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
class QTPrivatisationPlayer : public PrivatisationPlayer, public QGraphicsItem
{
public:
    friend QTPrivatisationGame;
QRectF boundingRect() const override
{
    qreal adjust = 2;
    return QRectF( -1000 - adjust, -1000 - adjust, 2300 + adjust, 2300 + adjust);
}
GraphWidget *graph;
QTPrivatisationPlayer(GraphWidget * widget, PrivatisationPlayer *PreviousPlayer):PrivatisationPlayer(PreviousPlayer)
{
    graph = widget;
    /*setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);*/
}
QTPrivatisationPlayer(GraphWidget * widget):PrivatisationPlayer()
{
    graph = widget;
}
QPainterPath shape() const override
{
    return QPainterPath();
}
void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) override
{
    painter->setBrush(Qt::gray);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawRect(0, 0, 40,30);
    QRectF sceneRect = graph->sceneRect();
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,sceneRect.width() - 4, sceneRect.height() - 4);
    string S = Int2Str(Score);
    painter->drawText(QRectF(0, 0, 100, 100),  QString::fromStdString(S));
}

};
class PrivatisationMap
{
    protected:
        vector<vector<int>> Map;
        size_t N, M;
        PrivatisationMap(){}
    public:

        Lvalue<int> operator[](QPoint r){return Lvalue<int>(&(Map[size_t(r.x())][size_t(r.y())]));}
        PrivatisationMap(size_t n, size_t m)
        {
            N = n; M = m;
            Map.resize(size_t(n));
            for(size_t i = 0; i < n; i++) Map[i].resize(size_t(m), 0);
        }
        friend PrivatisationGame;
};

class QTPrivatisationMap : public QGraphicsItem, private PrivatisationMap
{
public:
    int sx = 0, sy= 0;
    QRectF boundingRect() const override
    {
        qreal adjust = 2;
        return QRectF( -1000 - adjust, -1000 - adjust, 2300 + adjust, 2300 + adjust);
    }
    friend QTPrivatisationGame;
    GraphWidget *graph;
    //size_t size(){return New.size();}
    QTPrivatisationMap(GraphWidget *graphWidget, size_t n, size_t m):PrivatisationMap(n, m)
    {
        graph = graphWidget;
        setFlag(ItemIsSelectable);
        setFlag(ItemSendsGeometryChanges);
        setCacheMode(DeviceCoordinateCache);
        setZValue(-1);
    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        for(size_t i = 0; i < N; i++) for(size_t j = 0; j < M; j++)path.addRect(j*10, i*10, 10,10);
        return path;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) override
    {
        painter->setBrush(Qt::gray);
        painter->setPen(QPen(Qt::black, 0));
        for(size_t i = 0; i < N; i++) for(size_t j = 0; j < M; j++)
        {
            painter->setBrush(/*(j==sx && i == sy)*/Map[i][j]==0?Qt::gray : Qt::yellow);
            painter->drawRect(j*10, i*10, 10,10);
        }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override
    {
        update();
        sx = ((event->pos()).x())/10;
        sy = ((event->pos()).y())/10;
        QGraphicsItem::mousePressEvent(event);
    }
};

class PrivatisationNew
{
    //private:
    protected:
        vector<QPoint> New;
        string Name;
        int sx = 0, sy = 0;//selected
    public:
        PrivatisationNew() {GenerateNewItem();}
        void GenerateNewItem()
        {
            srand (time(NULL));
            int NewX = rand() % 6 + 1, NewY = rand() % 6 + 1;
            New.resize(size_t(NewY*NewX));
            for(int i = 0; i < NewX; i++) for(int j = 0; j < NewY; j++) New[size_t(i*NewY+j)] = QPoint(i, j);
        }
        QPoint operator[](size_t i)const{return New[i];}
        size_t size()const{return New.size();}
      //  friend PrivatisationGame;
};
class QTPrivatisationNew: public QGraphicsItem, public PrivatisationNew
{
    public:
    QRectF boundingRect() const override
    {
        qreal adjust = 2;
        return QRectF( -100 - adjust, -100 - adjust, 230 + adjust, 230 + adjust);
    }
    GraphWidget *graph;
    QTPrivatisationNew(GraphWidget *graphWidget):PrivatisationNew()
    {
        graph = graphWidget;
        setFlag(ItemIsMovable);
        setFlag(ItemSendsGeometryChanges);
        setCacheMode(DeviceCoordinateCache);
        setZValue(-1);
    }
    QPainterPath shape() const override
    {
        QPainterPath path;
        for(size_t i = 0; i < size(); i++)path.addRect((New)[i].y()*10, (New)[i].x()*10, 10,10);
        return path;
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) override
    {
        painter->setBrush(Qt::yellow);
        painter->setPen(QPen(Qt::black, 0));
        for(size_t i = 0; i < size(); i++)
        {
            painter->setBrush(((New)[i].y()==sx && (New)[i].x() == sy)?Qt::red : Qt::yellow);
            painter->drawRect((New)[i].y()*10, (New)[i].x()*10, 10,10);
        }
    }
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};
void f(int x, int y);
void QTPrivatisationNew::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    setPos(0, 110);
    f(((event->scenePos()).y()+200)/10 - 10-sy, ((event->scenePos()).x()+200)/10 - 5-sx);
    QGraphicsItem::mousePressEvent(event);
}

void QTPrivatisationNew::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //update();
    //event->pos();
    sx = ((event->pos()).x())/10;
    sy = ((event->pos()).y())/10;
    update();
    QGraphicsItem::mousePressEvent(event);
}
class PrivatisationGame
{
    //protected:
    public:
        vector<PrivatisationPlayer*> Players;
        PrivatisationPlayer* ActivePlayer;
        PrivatisationNew New;
        PrivatisationMap Map;
    public:
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

class QTPrivatisationGame
{
    //protected:
    public:
        vector<QTPrivatisationPlayer*> Players;
        QTPrivatisationPlayer* ActivePlayer;
        QTPrivatisationNew *New;
        QTPrivatisationMap *Map;
        GraphWidget *graph;
    public:
        QTPrivatisationGame(int n, int m, int NumberOfPlayers, GraphWidget *graphWidget, QGraphicsScene * scene)
        {
            graph = graphWidget;
            Map = new QTPrivatisationMap(graph, size_t(n) , size_t(m));
            srand (time(0));
            ActivePlayer = new QTPrivatisationPlayer(graph);
            New = new QTPrivatisationNew(graph);
            Players.push_back(ActivePlayer);
            for(size_t i = 0; i < size_t(NumberOfPlayers-1); i++) Players.push_back(new QTPrivatisationPlayer(graph, Players[i]));
            scene->addItem(Map);
            scene->addItem(New);
            Map->setPos(-150, -100);
            New->setPos(0, 110);
            ActivePlayer->setPos(-180, 120);
            for(size_t i = 0; i < size_t(NumberOfPlayers); i++) scene->addItem(Players[i]);
        }
        bool IsTurnPossible(QPoint r, bool FirsTurn = false)
        {
            for(size_t i = 0; i < New->size(); i++)
            {
                if(((*New)[i]+r).x()<0||((*New)[i]+r).x()>=(*Map).N) return false;
                if(((*New)[i]+r).y()<0||((*New)[i]+r).y()>=(*Map).M) return false;
                if((*Map)[(*New)[i]+r]!=0) return false;
            }
            if(FirsTurn) return true;
            bool Conected = FirsTurn;
            vector<QPoint> Conecting({QPoint(1, 0), QPoint(-1, 0), QPoint(0, 1), QPoint(0, -1)});
            for(size_t i = 0; i < (*New).size(); i++) for(size_t j = 0; j < Conecting.size(); j++)
            {
                QPoint P = (*New)[i]+Conecting[j];
                if((P.x() >= 0 && P.x() < (*Map).N) && (P.y() >= 0 && P.y() < (*Map).M)
                    &&(*Map)[P] == ActivePlayer->number) Conected = true;
            }
            return Conected;
        }
        bool AddItem(QPoint r, bool FirsTurn = false)
        {
            if(!IsTurnPossible(r, FirsTurn)) return false;
            for(size_t i = 0; i < (*New).size(); i++) (*Map)[(*New)[i] + r] = ActivePlayer->number;
            ActivePlayer->Score+=(*New).size();
            ActivePlayer=(QTPrivatisationPlayer*)ActivePlayer->NextPlayer;
            (*New).GenerateNewItem();
            Map->update();

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
            ActivePlayer=(QTPrivatisationPlayer*)ActivePlayer->NextPlayer;
        }
        ~QTPrivatisationGame(){for(size_t i = 0; i < Players.size(); i++) delete Players[i];}
    private:
};

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
    setWindowTitle(tr("Privatisation Game"));
    Game = new QTPrivatisationGame(20, 30, 1, this, scene);
    //scene->addItem(Map);
    //scene->addItem(New);

}

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
        break;
    case Qt::Key_Down:
        break;
    case Qt::Key_Left:
        break;
    case Qt::Key_Right:
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
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


}

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



void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
void f(int x, int y)
{
    Game->AddItem(QPoint(x, y), 1);
}
