#pragma once

#include <stdlib.h>
#include <time.h>
#include "../include/Card.hpp"
#include "../include/Deck.hpp"

srand(time(NULL));

class DEALER{

   private:
      DECK deck;
   public:
      DEALER(){
         deck = DECK();
      }
      void shuffle_deck(){
         
      }
      CARD deal_card(){
         return deck.erase( rand() % deck.size() );
      }
      void return_card(CARD card){
         this->deck.push_back(card);
         shuffle_deck();
      } 
};
