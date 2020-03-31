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
      std::string img;
   public:
      CARD(){

      }
      CARD(int v, int s){
         this->value = v;
         this->suite = s;
         img = this->print_value() + this->print_suite() + ".jpg";
      }
      int get_value(){
         return this->value;
      }
      int get_suite(){
         return this->suite;
      }
      std::string get_img(){
         return this->img;
      }
      std::string  print_value(){
         switch(this->value){
            case 0: return "A";
            case 1: return "2";
            case 2: return "3";
            case 3: return "4";
            case 4: return "5";
            case 5: return "6";
            case 6: return "7";
            case 7: return "8";
            case 8: return "9";
            case 9: return "10";
            case 10: return "J";
            case 11: return "Q";
            case 12: return "K";
            default: return "Error : Not a value";        
         }
      }
      std::string print_suite(){
         switch(this->suite){
            case 0: return "H";
            case 1: return "S";
            case 2: return "C";
            case 3: return "D";
            default: return "Error : Not a suite";
         }
      }
};
