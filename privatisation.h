#ifndef PRIVATISATION_H
#define PRIVATISATION_H
#include <vector>
#include <string>
#include <ctime>
using namespace std;
class MyPoint
{
    public:
    int x, y;
    MyPoint(){}
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
string Int2Str(int n);
class PrivatisationGame;
class PrivatisationPlayer
{
    protected:
        PrivatisationPlayer *NextPlayer;
        int Score = 0, number = 1, type = 1, life = 3;//1 - real player, 2 - bot, 0 - dead
        PrivatisationPlayer(){NextPlayer = this;}
        PrivatisationPlayer(PrivatisationPlayer *PreviousPlayer);
        void RemovePrivatisationPlayer();
    public:
        friend PrivatisationGame;
};
class PrivatisationMap
{
    protected:
        vector<vector<int>> Map;
        size_t N, M;
        PrivatisationMap(){}
        Lvalue<int> operator[](MyPoint r){return Lvalue<int>(&(Map[size_t(r.x)][size_t(r.y)]));}
        PrivatisationMap(size_t n, size_t m);
    public:
        friend PrivatisationGame;
};
class PrivatisationNew
{
    //private:
    protected:
        vector<MyPoint> New;
        string Name;

    public:
        PrivatisationNew() {GenerateNewItem();}
        void GenerateNewItem();
        MyPoint operator[](size_t i)const{return New[i];}
        size_t size()const{return New.size();}
      //  friend PrivatisationGame;
};
class PrivatisationGame
{
    //protected:
    public:
        vector<PrivatisationPlayer*> Players;
        PrivatisationPlayer* ActivePlayer;
        PrivatisationNew New;
        PrivatisationMap Map;
    public:
        PrivatisationGame(int n, int m, int NumberOfPlayers);
        bool IsTurnPossible(MyPoint r, bool FirsTurn = false);
        bool AddItem(MyPoint r, bool FirsTurn = false);
        void EndGame();
        void SkipTurn();
        ~PrivatisationGame(){for(size_t i = 0; i < Players.size(); i++) delete Players[i];}
    private:
};

#endif // PRIVATISATION_H
