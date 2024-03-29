#ifndef unilistH
#define unilistH
#ifndef containerH
#  include "corn/container/container.h"
#endif
namespace CORN
{
 class Data_record;                                                              //010103
//______________________________________________________________________________
class Unidirectional_list
: public Container                                                               //991105
{public:  // public only for BC5 which cannot see the protected node!
   class Node
   : public Container::Node                                                      //040109
   {
    public:
      Node *next;
      inline Node(Item *i_elmt_data)                                             //980206
      : Container::Node(i_elmt_data)                                             //040107
      , next(0)                                                                  //980206
      {}
      inline virtual ~Node()                                       { next = 0; }
   };
   //_Node_____________________________________________________________________/
 public: // I would like Iterator to be protected, but Bidirectional_list::Iterator needs to access and I don't know how to forward declare for friendship
   class Iterator
   : public Container::Iterator
   {
    protected:
      Unidirectional_list      &container;
    protected:
      Unidirectional_list::Node *curr_node;
    protected:
      contribute_ bool detached_head;                                            //161212
    public: // structors
      Iterator( Unidirectional_list &container_);
      virtual ~Iterator();
    public: //
      virtual Item *  first();
      virtual Item *  last();
      virtual Item *  next();
      virtual Item *  prev();
      virtual Item *  current();
      virtual Item *  Ith(nat32 i);                                              //030707
      virtual Item *  detach_current() ;                                         //030707
      inline virtual void end() {curr_node = 0;};                                //060927
      virtual bool has_successor()                                 affirmation_; //180515
      virtual bool has_predecessor()                               affirmation_; //180515
   };
   //_Iterator______________________________________________________2003-06-03_/
   friend class Unidirectional_list::Iterator;                                   //030603
 public:
   Node *head;
   Node *tail;
 protected:
   void append_node(Node *node);
   void prepend_node(Node *node);                                                //980206
   Node *find_node_with_element(Item *element)                            const;
 public:
   virtual Node *find_prev_of(Item *element)                              const; //970930
   virtual Node *find_prev_of(Node *element)                              const; //970930
 public:
   Unidirectional_list(bool _unique_keys=false);                                 //150818
   virtual ~Unidirectional_list();                                               //980508
   virtual Container::Iterator *iterator()                                const; //030603
      // Derived classes will create an iterator capable of
      // moving in the various directions.
   virtual nat32  count()                                                 const; //990104
   inline virtual Item * take(Item *new_item)                     appropriation_ //150818
      { return append(new_item); }
   virtual Item * append(Item *new_item);
   virtual Item * prepend(Item *new_item);
   virtual Item * insert_before(Item *new_item, Item *before_element);
      // not currently implemented in unidir list
      // virtual Item * add_sorted(Item *new_element);
   virtual nat32 delete_all(Item::Filter filter = Item::filter_all);
   virtual Item *pop_first();                                                    //991122
   virtual Item *pop_last ();                                                    //171204
   virtual Item *pop_tail();
   virtual Item *pop_head();
   virtual Item * get_at(nat32 index)                                     const; //991216
   virtual bool is_empty()                                                const; //040107
 public: // public only for BC5 which cannot see the protected node!
   inline Node *get_head()                                       { return head;}
};
//_Unidirectional_list_________________________________________________________/
}//_namespace CORN_____________________________________________________________/
#endif

