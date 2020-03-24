#pragma once

#include <iostream>
#include <string>

#define DECK_SIZE 52

enum SUITE{
   HEARTS,
   SPADES,
   CLUBS,
   DIAMONDS,
   SUITE_MAX
};

enum VALUE{
   ACE,
   TWO,
   THREE,
   FOUR,
   FIVE,
   SIX,
   SEVEN,
   EIGHT,
   NINE,
   TEN,
   JACK,
   QUEEN,
   KING,
   VALUE_MAX
};

class CARD{

   private:
      int value;
      int suite;
   public:
      CARD(){

      }
      CARD(int v, int s){
         this->value = v;
         this->suite = s;
      }
      int get_value(){
         return this->value;
      }
      int get_suite(){
         return this->suite;
      }
      std::string  print_value(){
         switch(this->value){
            case 0: return "Ace";
            case 1: return "Two";
            case 2: return "Three";
            case 3: return "Four";
            case 4: return "Five";
            case 5: return "Six";
            case 6: return "Seven";
            case 7: return "Eight";
            case 8: return "Nine";
            case 9: return "Ten";
            case 10: return "Jack";
            case 11: return "Queen";
            case 12: return "King";
            default: return "Error : Not a value";        
         }
      }
      std::string print_suite(){
         switch(this->suite){
            case 0: return "Hearts";
            case 1: return "Spades";
            case 2: return "Clubs";
            case 3: return "Diamonds";
            default: return "Error : Not a suite";
         }
      }
};
