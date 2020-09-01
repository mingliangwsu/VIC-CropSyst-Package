#error this is obsolete use use grid/grid_T.h

//---------------------------------------------------------------------------
#ifndef image_grid_TH
#define image_grid_TH
//---------------------------------------------------------------------------

#include <limits>
#include <corn/no_ns.h>
#include <corn/primitive.h>

// This is an abstract class for other derived classes
// The first row of data is at the top of the data set, moving from left to right

#define UNDIMENSIONED_GRID_SIZE 1
// I think we need at least 1 cell to avoid a problem
// check this, otherwise 0 would make more sense

namespace CORN {
//______________________________________________________________________________
   typedef nat32  Grid_dimension;
   typedef sint32 Grid_row;
   typedef sint32 Grid_col;
   // We allow the coordinate to be signed because
   // we allow negative index.
   // The value of a coordinate to the left or above the top row (0) will have a negative value.
   // Generally accessing native coordinates is not considered erroneous
   // because negative coordinates are simply considered of-site and methods accessing negative
   // coordinates either do nothing and/or return the no-data-value.
//______________________________________________________________________________
interface_ Image_grid_interface  // rename to Image_grid
{
 public:
   virtual Grid_dimension get_rows() const =0;
   virtual Grid_dimension get_cols() const =0;
   virtual float32 get_cell_size()   const =0;
      // Although cell_size is intented for the GIS_Grid derived classes,
      // it may also be applicable for other uses, (I.e. image pixel units).
};
//_Image_grid_interface________________________________________________________/
template <typename T>
class Image_grid
: public Image_grid_interface  // rename to Image_grid_clad
{
 protected:
   Grid_dimension rows; // zero based
   Grid_dimension cols;
   float32 cell_size; // the width and height of the square cell real world units to be defined by derived grid (meters recommended)
                      // This can be 1 images
 protected:
   T     no_data_value;
   T     lowest_value;
   T     highest_value;
 protected :
   T    **grid;   // Currently it is assumed that the grid data is stored
                  // in row-major order row 0 is the top of the grid.
                  // For GIS_grid derived classes the top row typically
                  // corresponds to the northern most cells.
 public:
   //___________________________________________________________________________
   Image_grid()
   : rows(UNDIMENSIONED_GRID_SIZE)
   , cols(UNDIMENSIONED_GRID_SIZE)
   , cell_size(UNDIMENSIONED_GRID_SIZE)
   , grid(NULL)
   , no_data_value   (0)
   , lowest_value    (std::numeric_limits<T>::min())
   , highest_value   (std::numeric_limits<T>::max())
   { };
   //_Image_grid:constructor___________________________________________________/
   Image_grid(Grid_dimension rows_, Grid_dimension cols_,float32 cell_size_)
   : rows(rows_)
   , cols(cols_)
   , cell_size(cell_size_)
   , grid(NULL)
   , no_data_value   (0)
   , lowest_value    (std::numeric_limits<T>::min())
   , highest_value   (std::numeric_limits<T>::max())
   { }
   //_Image_grid:constructor___________________________________________________/
   bool redimension
   (Grid_dimension rows_, Grid_dimension cols_,float32 cell_size_
   )
   {  bool result = true;
      rows = rows_;
      cols = cols_;
      cell_size = cell_size_;
      dispose_grid(); // Make sure grid is clear (if grid exists then this is reinitialize()
      typedef T *typeptr;
      grid = new typeptr[rows];
      if (grid)
         for (Grid_row r = 0; r < rows; r++)
         {
            grid[r] = new T[cols];
            if (grid[r] == 0)
                return false;
            for (Grid_col c = 0; c < cols; c++)
               grid[r][c] = no_data_value;
         };
      return result;
   };
   //_redimension______________________________________________________________/
   bool redimension_congruent_to(const Image_grid_interface &exemplar_grid)
   {  return redimension(exemplar_grid.get_rows(),exemplar_grid.get_cols()
         ,exemplar_grid.get_cell_size());
   }
   //_redimension_congruent_to_________________________________________________/
   inline virtual Grid_dimension get_rows()                const { return rows;}
   inline virtual Grid_dimension get_cols()                const { return cols;}
   //___________________________________________________________________________
   inline bool valid_coordinates( Grid_row row,  Grid_col col)             const
   {  // Note row and col must be signed
      return   (row >= 0) && (row < rows) && (col >=0) && (col < cols);
   }
   //_valid_coordinates________________________________________________________/
   virtual void update_extremes()
   {
      lowest_value  = std::numeric_limits<T>::max(); // start with max then minimize
      highest_value = std::numeric_limits<T>::min(); // start with min then maximize
      for (Grid_row r = 0; r < rows; r++)
      {
         for (Grid_col c = 0; c < cols; c++)
         {
            T value = get_at(r,c);
            if (value != no_data_value)
            {  if (value > highest_value)
                  highest_value = value;
               if (value < lowest_value)
                  lowest_value   = value;
            }
         }
      }
   }
   //_update_extremes__________________________________________________________/
   inline float32 get_cell_size() const
      { return cell_size; };
   //___________________________________________________________________________
   void clear_to(T value)                                           assignation_
   {
      for (Grid_row r = 0; r < rows; r++)
         for (Grid_col c = 0; c < cols; c++)
            set_at(r,c,value);
   }
   //_clear_to_________________________________________________________________/
   virtual void clear()                 assignation_ { clear_to(no_data_value);}
   // Sets all cells to no_data_value
   // Must be defined in base classes which have the no_data_value
   //_clear____________________________________________________________________/
   virtual void dispose_grid()
   {
       if (grid)
       {
          for (Grid_row r = 0; r < rows; r++)
             if (grid[r])
                delete [] grid[r];
          delete [] grid;
       };
       grid = 0;
   }
   //_dispose_grid_____________________________________________________________/
   bool set_range
   (T i_no_data_value
   ,T i_lowest_value
   ,T i_highest_value)                                              assignation_
   {
      no_data_value = i_no_data_value;
      lowest_value  = i_lowest_value;
      highest_value = i_highest_value;
      return true;
   }
   //_set_range__________________________________________________________________________
   inline bool dimensioned()                                        affirmation_
      // rename is_dimensioned
   { return (rows>UNDIMENSIONED_GRID_SIZE) && cols>UNDIMENSIONED_GRID_SIZE; }
   //_dimensioned______________________________________________________________
   inline virtual T set_at(Grid_row row, Grid_col col,T value)      assignation_
   {  return  (grid && valid_coordinates(row,col))
          ? (grid[row][col] = value)  // assignment not comparison
          : no_data_value;
   }
   //_set_at___________________________________________________________________/
   inline virtual T get_at(Grid_row row, Grid_col col)                     const
   {
      return (grid && valid_coordinates(row,col))
      ? grid[row][col]
      : no_data_value;
   }
   //_get_at___________________________________________________________________/
public: // properties accessors
   inline virtual T get_lowest_value()           const { return lowest_value; }
   inline virtual T get_highest_value()          const { return highest_value; }
   inline T get_no_data_value()                  const { return no_data_value; }
   inline  set_no_data_value(T new_no_data_value)
      { return no_data_value = new_no_data_value; }
};
//_Image_grid__________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

