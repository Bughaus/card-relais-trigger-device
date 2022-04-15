#include "CardList.h"

CardList::CardList() {}
CardList::~CardList() {}

CardList::CardList(int maxCards) { _maxCards = (maxCards < _maxCards) ? maxCards : _maxCards; }

bool CardList::add(String card) {
  // find first leak
  for (int i = 0; i < _maxCards; i++) {
    if (_cards[i].length() == 0) {
      _cards[i] = card;
      return true;
    }
  }
  return false;
}

bool CardList::del(String card) {
  for (int i = 0; i < _maxCards; i++) {
    if (_cards[i] == card) {
      _cards[i] = "";
      return true;
    }
  }
  return false;
}

int CardList::cnt() {
  int iCnt = 0;
  for (int i = 0; i < _maxCards; i++) {
    if (_cards[i].length() > 0) {
      iCnt ++;
    }
  }
  return iCnt;
}

bool CardList::isInList(String card) {
  for (int i = 0; i < _maxCards; i++) {
    if (_cards[i] == card) {
      return true;
    }
  }
  return false;
}

bool CardList::hasSpace() {
  if (cnt() == _maxCards) return false;
  return true;
}
