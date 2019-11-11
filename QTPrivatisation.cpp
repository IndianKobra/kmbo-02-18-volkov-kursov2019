#include "QTPrivatisation.h"
#ifndef QTPRIVATISATION_CPP
#define QTPRIVATISATION_CPP
QTPrivatisationGame* Game;
void QTPrivatisationPlayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setBrush(Qt::gray);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawRect(0, 0, 40,30);
    QRectF sceneRect = graph->sceneRect();
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,sceneRect.width() - 4, sceneRect.height() - 4);
    string S = Int2Str(Score);
    painter->drawText(QRectF(0, 0, 100, 100),  QString::fromStdString(S));
}
void QTPrivatisationMap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setBrush(Qt::gray);
    painter->setPen(QPen(Qt::black, 0));
    for(size_t i = 0; i < N; i++) for(size_t j = 0; j < M; j++)
    {
        painter->setBrush(Map[i][j]==0?Qt::gray : Qt::yellow);
        painter->drawRect(j*10, i*10, 10,10);
    }
}
void QTPrivatisationMap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    sx = ((event->pos()).x())/10;
    sy = ((event->pos()).y())/10;
    QGraphicsItem::mousePressEvent(event);
}
void QTPrivatisationNew::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    setPos(0, 110);
    f(((event->scenePos()).y()+200)/10 - 10-sy, ((event->scenePos()).x()+200)/10 - 5-sx);
    QGraphicsItem::mousePressEvent(event);
}

void QTPrivatisationNew::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    sx = ((event->pos()).x())/10;
    sy = ((event->pos()).y())/10;
    QGraphicsItem::mousePressEvent(event);
}
void QTPrivatisationNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setBrush(Qt::yellow);
    painter->setPen(QPen(Qt::black, 0));
    for(size_t i = 0; i < size(); i++)painter->drawRect((New)[i].y*10, (New)[i].x*10, 10,10);
}
QTPrivatisationNew::QTPrivatisationNew(GraphWidget *graphWidget):PrivatisationNew()
{
    graph = graphWidget;
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}
QPainterPath QTPrivatisationNew::shape()
{
    QPainterPath path;
    for(size_t i = 0; i < size(); i++)path.addRect((New)[i].y*10, (New)[i].x*10, 10,10);
    return path;
}
QTPrivatisationGame::QTPrivatisationGame(int n, int m, int NumberOfPlayers, GraphWidget *graphWidget, QGraphicsScene * scene)
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
bool QTPrivatisationGame::IsTurnPossible(MyPoint r, bool FirsTurn)
{
    for(size_t i = 0; i < New->size(); i++)
    {
        if(((*New)[i]+r).x<0||((*New)[i]+r).x >=(*Map).N) return false;
        if(((*New)[i]+r).y<0||((*New)[i]+r).y >=(*Map).M) return false;
        if((*Map)[(*New)[i]+r]!=0) return false;
    }
    if(FirsTurn) return true;
    bool Conected = FirsTurn;
    vector<MyPoint> Conecting({MyPoint(1, 0), MyPoint(-1, 0), MyPoint(0, 1), MyPoint(0, -1)});
    for(size_t i = 0; i < (*New).size(); i++) for(size_t j = 0; j < Conecting.size(); j++)
    {
        MyPoint P = (*New)[i]+Conecting[j];
        if((P.x >= 0 && P.x < (*Map).N) && (P.y >= 0 && P.y < (*Map).M)
            &&(*Map)[P] == ActivePlayer->number) Conected = true;
    }
    return Conected;
}
bool QTPrivatisationGame::AddItem(MyPoint r, bool FirsTurn = false)
{
    if(!IsTurnPossible(r, FirsTurn)) return false;
    for(size_t i = 0; i < New->size(); i++) (*Map)[(*New)[i] + r] = ActivePlayer->number;
    ActivePlayer->Score+= New->size();
    Map->update();
    ActivePlayer->update();
    ActivePlayer=(QTPrivatisationPlayer*)ActivePlayer->NextPlayer;
    New->GenerateNewItem();
    return true;
}
void QTPrivatisationGame::SkipTurn()
{
    ActivePlayer->life--;
    if(ActivePlayer->life<=0)ActivePlayer->RemovePrivatisationPlayer();
    if(ActivePlayer==ActivePlayer->NextPlayer)EndGame();
    ActivePlayer=(QTPrivatisationPlayer*)ActivePlayer->NextPlayer;
}
void QTPrivatisationGame::EndGame()
{

}

void f(int x, int y)
{
    Game->AddItem(MyPoint(x, y), 1);
}
void g(GraphWidget* graph, QGraphicsScene *scene )
{
    Game = new QTPrivatisationGame(20, 30, 1, graph, scene);
}
#endif
