#ifndef binary_treeH
#define binary_treeH
#ifndef   containerH
#  include "corn/container/container.h"
#endif
namespace CORN {
//______________________________________________________________________________
class Binary_tree
: public Container
{  // This is a balanced binary tree
   // The balancing algorithm was devised by Roger Nelson
   // It may be similar to weight balanced binary tree,
   // but I have not been able to find a description of that data structure
public:   // In UNIX this was not accessable to Iterator when protected
   class Node
   : public Container::Node
   {
#ifdef __BCPLUSPLUS__
      friend class Binary_tree::Iterator;                                        //040208
#endif
   private:
      Node  *left;
      Node  *right;
      // This is the number of nodes in this branch of the tree (inclusive)
   public: // structors
      Node(Item *elmt_data_);
      virtual ~Node();
   public:
      nat32  get_weight() const;
      Node *get_at(nat32 index,nat32 &curr_index) ;
      Node *insert(Node *node,bool keep_balance);
      Node *detach(Node *node,bool keep_balance);
      // protected: ?
      Node *detach_left_most_node();
      // returns 0 if there was no left node.
      Node *detach_right_most_node();
      // returns 0 if there was no left node.
      inline Node  *get_left()                             const { return left;}
      inline Node  *relinquish_left()
         { Node  *left_node = left; left = 0; return left_node;}
      inline Node  *get_right()                            const {return right;}
      inline Node  *relinquish_right()
         { Node  *right_node = right; right = 0; return right_node;}
      virtual void write_structure
         (std::ostream &strm,nat32 indentation)                      stream_IO_;
   public: // rebalance is currently public because we may want to all the tree
      // to be rebalanced at any time even though it is not autobalanced.
      bool rebalance();
      inline void clear_right()                                   { right = 0; }
      inline void clear_left()                                    { left  = 0; }
   };
 protected:
   class Iterator
   : public Container::Iterator
   {
    protected:
      Binary_tree      &container;
    protected:
      Node *curr;
      Node *parent_stack[1000];
      int parent_stack_index;
   public:
      Iterator(Binary_tree &i_container);
      virtual ~Iterator();                                                       //030610
      // Each of the following advances the iterator to the respective node
      // and returns the item at that position.
      // If the container is empty
      // or there is no item at the position it returns 0;
      virtual Item *  first();
      virtual Item *  last();
      virtual Item *  next();
      virtual Item *  prev();
      virtual Item *  current();
      virtual Item *  Ith(nat32 i);                                              //030707
      virtual Item * find_comparable                                             //990105
         (const Item &comparable_item_to_match,bool from_beginning = true);
         //990105 (was find, but conflict with association_list::find())
      inline virtual void end()                                    { curr = 0; } //060927
      virtual bool has_predecessor()                               affirmation_; //180515
      virtual bool has_successor()                                 affirmation_; //150505

      //                Advances the iterator to the Ith node (0 based).
   public: // The following are editing.
      virtual Item *  detach_current() ;                                         //030707
      //  Removes the current node from the list
      //  The node data is not deleted.
      //  Returns the item or 0 if not successful
      inline Node    *get_root()                      { return container.root; }
   private:
      virtual Item *  furthest(bool forward);
      virtual Item *  adjacent(bool forward);
   };
   //2003-06-03_____________________________________________________
   friend class Binary_tree::Iterator;
 protected:
   Node    *root;
   bool     keep_balance; // true if the user want's to keep the tree balanced as items are inserted or deleted
 public:
   inline Binary_tree(bool _keep_balance = true)
      : Container(true)
      , root(0)
      , keep_balance(_keep_balance)
      {}
   virtual ~Binary_tree();                                                       //051010
   virtual Container::Iterator *iterator()                                const; //030603
   virtual nat32       count()                                            const; //991215
   virtual Item       *take(Item *new_element)                   appropriation_; //150818
   //150818 virtual Item        *append(Item *new_element);
   virtual nat32       delete_all(Item::Filter filter = Item::filter_all);       //040318
   virtual Item        *pop_first();
   virtual Item        *pop_last();                                              //171204
   virtual Item        *get_at(nat32 index)                               const;
   virtual inline bool  is_empty()                 const { return (root == 0); }
   virtual void         write_structure(std::ostream &strm)        performs_IO_;
 protected:
   virtual bool         rebalance();                                             //051009
      // returns true if rebalance was necessary;
      // Conceptual return the number of reattachments.
};
//_Binary_tree_________________________________________________________________/
}//_namespace CORN_____________________________________________________________/
#endif
// binary_tree.h

