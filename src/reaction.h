/* reaction.h
 */

#ifndef REACTION_H
#define REACTION_H 

class Reaction
{
   public:
      static void initList();
      static void printList();

   private:
      static int list[5][2];
      //static int *list;
};

#endif /* REACTION_H */
