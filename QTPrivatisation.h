#include "privatisation.h"
#include "graphwidget.h"
#include <QGraphicsItem>
#include <math.h>
#include <QString>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QGraphicsItem>
#include <QPainter>

#ifndef QTPRIVATISATION_H
#define QTPRIVATISATION_H
class QTPrivatisationGame;

class QTPrivatisationPlayer : public PrivatisationPlayer, public QGraphicsItem
{
public:
    friend QTPrivatisationGame;
    QRectF boundingRect() const override{return QRectF(0, 0, 100, 100);}
    GraphWidget *graph;
    QTPrivatisationPlayer(GraphWidget*widget, PrivatisationPlayer *PrevPlayer):PrivatisationPlayer(PrevPlayer),graph(widget){}
    QTPrivatisationPlayer(GraphWidget * widget):PrivatisationPlayer(),graph(widget){}
    QPainterPath shape() const override{return QPainterPath();}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) override;
};

class QTPrivatisationMap : public QGraphicsItem, private PrivatisationMap
{
public:
    int sx = 0, sy= 0;
    QRectF boundingRect() const override{return QRectF(0, 0, 10*M, 10*N);}
    friend QTPrivatisationGame;
    GraphWidget *graph;
    QTPrivatisationMap(GraphWidget *graphWidget, size_t n, size_t m):PrivatisationMap(n, m)
    {
        graph = graphWidget;
        setFlag(ItemIsSelectable);
        setFlag(ItemSendsGeometryChanges);
        setCacheMode(DeviceCoordinateCache);
        setZValue(-1);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

class QTPrivatisationNew: public QGraphicsItem, public PrivatisationNew
{
    public:
    int sx = 0, sy = 0;//selected
    GraphWidget *graph;
    QRectF boundingRect() const override{return QRectF(0, 0, 10*6, 10*6);}
    QTPrivatisationNew(GraphWidget *graphWidget);
    QPainterPath shape() ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
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
        QTPrivatisationGame(int n, int m, int NumberOfPlayers, GraphWidget *graphWidget, QGraphicsScene * scene);
        bool IsTurnPossible(MyPoint r, bool FirsTurn = false);
        bool AddItem(MyPoint r, bool FirsTurn);
        void EndGame();
        void SkipTurn();
        ~QTPrivatisationGame(){for(size_t i = 0; i < Players.size(); i++) delete Players[i];}
    private:
};
void f(int x, int y);
void g(GraphWidget* graph, QGraphicsScene *scene );
#endif // QTPRIVATISATION_H
