#ifndef constH
#define constH

// Note the following defines are to essentially create pseudo keywords
// they should be appended with an underscore

// The following can be used to indicate that the method doesn't modify
// the data or the data structure, current pointers may be adjusted
// The following can be used to indicate that a method must not be const
#define modifiable_
// Indicates the return value or parameter of the function is modifiable.
#define return_
// When used with formal parameter indicates the parameter will be modified
// and will return a value (for functions that return multiple values).
#define unmodifiable_ const
// Indicates the return value or parameter of the function is unmodifiable.
#define streamable_
// Indicates that the parameter of the function is a stream
// (currently stream IO cannot be const because stream methods are not const.

#define performs_IO_
// Indicates the function performs input or output,
// this may adjust stream buffers and pointers (I/O functions should never be const).
#define performs_IO_const_ const
// Indicates the function performs input or output,
// this may adjust stream buffers and pointers .
// In the future make the method a contribution and the modified member a contribute_.
#define stream_IO_
// Indicates the method performs stream input or output.
// Normally such functions would be consider const
// (as far as the class is concerned) but the stream class
// methods may adjust stream buffers and pointers
// (so I/O functions that have stream members cannot be const).
// Reading values (attributes) from a file could be considered initialization.
#define modification_
// Indicates the function is intended to modify a member value (attribute).
// (not_const is deprecated)
#define not_virtual_
// Opposite of virtual
#define mutates_ const
#define mutation_ const
// Mutation (Mutates) is a const method that changes mutable values
//use provision #define provides_ const
#define provision_ const

#define provided_ mutable
// provided is deprecated, use providable_

#define providable_ mutable
// In my design pattern provision method that allocates a member
// or container item as needed such members are providable_
// Typically member item pointers are declared as const mutable
// (the item would otherwise be considered to have always existed
// ,the caller would generally not know of previous existance,
//  the object simply dynamically allocates the member/item as needed)
// if not already allocated.
// Provide is similar to a get but tries to always return the item,
// hence provision methods typically will return a const reference to the
// dynamically allocated item.
// Provision may return pointer 0 if there is a potential rare unexpected
// condition that would otherwise prevent such an item from being provided.
// (I.e. cannot load data from a missing file)
// Such a condition should be considered error state so if a 0 pointer is
// returned, the caller should initiate an error, exception or otherwise
// not continue the program.
// Otherwise use
// These member or containers storing the provided item must be declared mutable
// in order to allow the addition any new item (use providable_/provided_ indicator).

#define updatable_ mutable
// The method is intended to update (usually one) mutable datamember

#define rendition_ const
// The method (function) is intended to create a new item which is relinquished
// to the caller.
// The method does not retain a pointer or reference to the object.
// The method might return 0 if the item could not be rendered.
// Compare with provision_ when the object retains ownership of the item.
// Methods names are often prefixed  render_

#define appropriation_  modification_
// The object takes exclusive ownership of the item passed (as a pointer).
// The taken item could be immediately deleted.
// The caller must not further use or reference the object and must not delete it.
// Method names are often named take or prefixed take_
// Methods such as append prepend would be considered appropriations.

#define submission_ modification_
// The caller wishs to relinquish ownership of a passed item to the object.
// The method will return true if the object takes ownership of the item.
// or false if it refuses, or some other indicator on the status of the
// or appropriateness of the submission
// If the object takes the item, it guarentees not to immediately delete the item
// and will not delete the item until the object is destroyed
// (or the item is otherwise relinquished to other object).
// Returns true if the submitted item is indeed taken, otherwise returns false
// if ownership is refused (caller must otherwise dispose of the item)
// See appropriation where the object is free to immediately delete the item.
// Method names are often prefixed submit_

#define remission_ modification_
// The caller wants the object to take ownership of the passed item
// and potentially relinquish a corresponding item currently owned by the object
// (an exchange).
// The caller can pass 0 for item (essentially a renunciation).
// Object returns pointer to the relinquished item , 0 if nothing remitted.
// As with a submission, the remitted item will not be deleted by the object.
// method names are often prefixed remit_

#define omission_ modification_
// The caller requests the object to delete the specified item which is owned by the object.
// Essentially caller is saying it nolongers want or needs the item and it can be delete.
// The object is not oblicated to actually delete the item, it may still be using the item
// or it may know that other objects are using the item.
// Such ownership mechanism might be implemented with smart pointer.
// methods are usually prefixed with omit_

#define reception_ modification_
// The caller requests the object take all or some of the offered item.
// The object would typically remove elements or a portion of the item.
// The object retains ownership and may at any point delete removed elements.
// The caller retains ownership of the remainder of offered item
// (which may be potentially exhausted by the object).

#define given_

#define renunciation_ modification_
// The method relinquishes ownership of the returned item.
// Method names are often prefixed relinquish_, renounce_ or give_.
// For containers, the method will either receive as a pointer, reference
// or some other mechanism identifying the item to be given.
// May return 0 if there is no matching item, or the object refuses to
// relinquish ownership, otherwise the caller must take ownership of the item.
// Note that this differs from rendition_ in that
// the item was never owned by the object.

#define association_ modification_
// The method makes the object know the passed item (inclusive reference).
// The object does not take ownership.
// association_ implies that the passed object will be one of many
// such objects to be known (I.e. pointer will be stored in a data structure).
// The associated item is modifiable by the object.
// Method names are often named  associate_  or simply know_
// The caller must promise that the associated object will not be deleted
// for the duration of this object's existance without using a dissassociation method.

#define cognition_ contribution_
// The item that is to be known is referenced as a nonmutable cognate.
// Cognition implies that the 'known' object is not tobe modified.
// The cognated item is picked up as a contribute,
// since would be contributing to the function of the object, not an attribute.
// Just because you know something, that doesn't mean your constitution changes.
// cognition_ implies that the passed 'cognate' item will be the only
// such member item to be known and any previously known member is forgotton.
// (This doesn't mean it is the only known item, only that there
// will only be one known occurance for such an item
// Compare association_ where where there may be list of 'associate' items.
// The caller must promise that the cognate item will not be deleted for the
// duration of the object's existance without using a dissassociation_ method.
// Method names are often named recognize_ (if the cognate changes more than once)
// cognize_ if the cognate is set zero or one time (or simply know_).


#define cognate_ mutable
// Mutable only in that the reference (pointer) can be changed, not the value(s)

#define recognition_ modification_
// Similar to cognition_ but the object mutates itself

/*
#define recognition_ const
// Method that returns true if the object has or owns or knows an object
// Typically the method may return the a pointer to the object to test existance
// known.
*/

#define disassociation_ modification_
// The method clears knowledge of the specified or currently known item.
// Typically the method returns boolean true if the object was known.
// If forgetting a list of items one might return the count of items actually known and forgotton
// (the object might not know all listed objects to forget).
// Method names are often named  forget_

#define contribution_ const
// Indicate the function may also calculate other values (contributes) in the object not just the return value.
#define contribute_  mutable

// abdication_
// method names are often prefixed abdicate_

#define affirmation_ const
// Method that returns true .
// Method names are often prefixed is_.

#define interrogation_ const
// Method returns some sort of indicator of state.
// Unlike affirmation confirmation which return true/false a specific code is usually returned.

#define assignation_ modification_
// Method assigns a value to a member
// Method names are often prefixed assign_ or set_

#define confirmation_ const
// The method tests if the object in a requisite state.
// returns false if it is not possible to enter the state.

#define supposition_ modification_
// The method puts the object in a requisite state

#define calculation_ const
// The method is a true function
// Method names are often prefixed calc_
// returns the calculated value

#define computation_ modification_
// The method performs a number of calculations
// (the results the calculations may be stored in one or more member variables)
// ultimately producing a single computed value
// (which is also typically stored in a member variable)
// returns the ultimate computed value

#define interface_ struct
// All methods of interfaces should be public (struct ensures this)

// Note yet defined:
// alteration
// amelioration

#define rectification_ mutation_
// The method updates, adjusts or corrects a value.
// Method names are often prefixed with update_.
// Member attributes to be rectified are usually mutable.

#define initialization_ modification_
// The method is used to initialize or reinitialize the object.

#define finalization_ modification_
// The method is used to initialize or reinitialize the object.

#define incorporation_ modification_
// The method (of a container) takes new elements/items
// generally used with classes specializing containers.
// Very similar in concept to rectification, except is is not
// possible to define a method a const when is needs to use non
// const members

#define construction_
// The method is called by the constructor to perform initialization during construction.
// Used when multiple constructors have the same initialization.
// Unlike initialization_ methods, construction methods must not be virtual
// because constructors do not call reimplemented methods of derived classes.

#define expectation_
// The method prepares attributes for I/O in document structures.
// Methods are often prefixed with expect_

#define invariant_ virtual
// The method must be invariant.

#define covariant_ virtual
// The method has covariant return
// https://en.wikipedia.org/wiki/Covariance_and_contravariance_(computer_science)
// http://www.lwithers.me.uk/articles/covariant.html

#define implements_ virtual
#define extends_
#define extends_interface_  virtual

#define implmt
// implmt is the opposite of inline

#endif
//const

