#ifndef CardList_h
#define CardList_h

#include <Arduino.h>
#include <String.h>

class CardList {
  private:
    String _cards[8] = {"","","","","","","",""};
    int _maxCards = 8;
    
  public:
    CardList();
    CardList(int maxCards);
    ~CardList();
    bool add(String card);
    bool del(String card);
    int cnt();
    bool isInList(String card);
    bool hasSpace();
};

#endif
