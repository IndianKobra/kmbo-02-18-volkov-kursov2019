#ifndef PRIVATISATION_CPP
#define PRIVATISATION_CPP
#include "privatisation.h"
#include "ctime"
//PrivatisationGame* Game;
string Int2Str(int n)
{
    string Ans = "";
    do{
        Ans = char('0'+(n%10)) + Ans;
        n/=10;
    }while(n>0);
    return Ans;
}
PrivatisationPlayer::PrivatisationPlayer(PrivatisationPlayer *PreviousPlayer)
{
    NextPlayer = PreviousPlayer->NextPlayer;
    PreviousPlayer->NextPlayer = this;
    number = PreviousPlayer->number+1;
}
void PrivatisationPlayer::RemovePrivatisationPlayer()
{
    PrivatisationPlayer *PreviousPlayer = this;
    while(PreviousPlayer->NextPlayer != this) PreviousPlayer = PreviousPlayer->NextPlayer;
    PreviousPlayer->NextPlayer = NextPlayer;
    type = 0;
}
void PrivatisationNew::GenerateNewItem()
{
    srand (time(NULL));
    int NewX = rand() % 6 + 1, NewY = rand() % 6 + 1;
    New.resize(size_t(NewY*NewX));
    for(int i = 0; i < NewX; i++) for(int j = 0; j < NewY; j++) New[size_t(i*NewY+j)] = MyPoint(i, j);
}
PrivatisationMap::PrivatisationMap(size_t n, size_t m)
{
    N = n; M = m;
    Map.resize(size_t(n));
    for(size_t i = 0; i < n; i++) Map[i].resize(size_t(m), 0);
}
PrivatisationGame::PrivatisationGame(int n, int m, int NumberOfPlayers)
{
    Map = PrivatisationMap(size_t(n) , size_t(m));
    srand (time(0));
    ActivePlayer = new PrivatisationPlayer();
    Players.push_back(ActivePlayer);
    for(size_t i = 0; i < NumberOfPlayers-1; i++) Players.push_back(new PrivatisationPlayer(*Players[i]));
}
bool PrivatisationGame::IsTurnPossible(MyPoint r, bool FirsTurn)
{
    for(size_t i = 0; i < New.size(); i++)
    {
        if((New[i]+r).x<0||(New[i]+r).x>=Map.N) return false;
        if((New[i]+r).y<0||(New[i]+r).y>=Map.M) return false;
        if(Map[New[i]+r]!=0) return false;
    }
    if(FirsTurn) return true;
    bool Conected = FirsTurn;
    vector<MyPoint> Conecting({MyPoint(1, 0), MyPoint(-1, 0), MyPoint(0, 1), MyPoint(0, -1)});
    for(size_t i = 0; i < New.size(); i++) for(size_t j = 0; j < Conecting.size(); j++)
    {
        MyPoint P = New[i]+Conecting[j];
        if((P.x >= 0 && P.x < Map.N) && (P.y >= 0 && P.y < Map.M)
            &&Map[P] == ActivePlayer->number) Conected = true;
    }
    return Conected;
}
bool PrivatisationGame::AddItem(MyPoint r, bool FirsTurn)
{
    if(!IsTurnPossible(r, FirsTurn)) return false;
    for(size_t i = 0; i < New.size(); i++) Map[New[i] + r] = ActivePlayer->number;
    ActivePlayer->Score+=New.size();
    ActivePlayer=ActivePlayer->NextPlayer;
    New.GenerateNewItem();
    return true;
}
void PrivatisationGame::SkipTurn()
{
    ActivePlayer->life--;
    if(ActivePlayer->life<=0)ActivePlayer->RemovePrivatisationPlayer();
    if(ActivePlayer==ActivePlayer->NextPlayer)EndGame();
    ActivePlayer=ActivePlayer->NextPlayer;
}
void PrivatisationGame::EndGame()
{}
#endif // PRIVATISATION_CPP
