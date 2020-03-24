#pragma once

#include <iostream>
#include <vector>
#include "Card.hpp"

class DECK{
   
   private:
      
   public:
      std::vector<CARD> deck;
      DECK(){
         for(int i = 0; i < SUITE_MAX; i++){
            for(int j = 0; j < VALUE_MAX; j++){
               deck.push_back(CARD(j, i));
            }
         }   
      }
      void print_deck(){
         for(unsigned int i = 0; i < this->deck.size(); i++){
            std::cout << i+1 << " : " << this->deck.at(i).print_value()
                      << " of " << this->deck.at(i).print_suite() << std::endl;
         }
      }
};
