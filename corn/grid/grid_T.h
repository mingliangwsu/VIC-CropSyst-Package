//---------------------------------------------------------------------------
#ifndef grid_TH
#define grid_TH
//---------------------------------------------------------------------------
#include <limits>
#include "corn/no_ns.h"
#include "corn/primitive.h"

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
interface_ Grid_interface  // rename to Grid
{
public:
   virtual Grid_dimension get_rows() const =0;
   virtual Grid_dimension get_cols() const =0;
   virtual float32 get_cell_size()   const =0;
      // Although cell_size is intented for the GIS_Grid derived classes,
      // it may also be applicable for other uses, (I.e. image pixel units).
};
//_Grid_interface______________________________________________________________/
template <typename T>
class Grid   // rename to Grid_clad
: public Grid_interface
{
public:

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
   Grid()
      : rows(UNDIMENSIONED_GRID_SIZE)
      , cols(UNDIMENSIONED_GRID_SIZE)
      , cell_size(UNDIMENSIONED_GRID_SIZE)
      , grid(NULL)
      , no_data_value   (0)
      , lowest_value    (std::numeric_limits<T>::min())
      , highest_value   (std::numeric_limits<T>::max())
      { }
   //________________________________________________________empty_constructor_/
   Grid(Grid_dimension i_rows, Grid_dimension i_cols,float32 i_cell_size)
      : rows(i_rows)
      , cols(i_cols)
      , cell_size(i_cell_size)
      , grid(NULL)
      , no_data_value   (0)
      , lowest_value    (std::numeric_limits<T>::min())
      , highest_value   (std::numeric_limits<T>::max())
      { }
   //________________________________________________________sized_constructor_/
   Grid(Grid &copy_from)
      : rows(0) //(copy_from.rows)
      , cols(0) //(copy_from.cols)
      , cell_size(copy_from.cell_size)
      , grid(NULL)
      , no_data_value   (copy_from.no_data_value)
      , lowest_value    (copy_from.lowest_value)
      , highest_value   (copy_from.highest_value)
      {
         redimension_congruent_to(copy_from);
         for (Grid_dimension r = 0; r < rows; r++)
            for (Grid_dimension c = 0; c < cols; c++)
               grid[r][c] = copy_from.get_at(r,c) ;
      }
   //_________________________________________________________copy_constructor_/
   bool redimension
   (Grid_dimension i_rows, Grid_dimension i_cols
   ,float32 i_cell_size)
   {  bool result = true;
      rows = i_rows;
      cols = i_cols;
      cell_size = i_cell_size;
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
         }
      return result;
   }
   //___________________________________________________________________________
   bool redimension_congruent_to(const Grid_interface &exemplar_grid)
   {  return redimension(exemplar_grid.get_rows(),exemplar_grid.get_cols(),exemplar_grid.get_cell_size());
   }
   //___________________________________________________________________________
   inline virtual Grid_dimension get_rows() const { return rows;};
   inline virtual Grid_dimension get_cols() const { return cols;};
   //___________________________________________________________________________
   inline bool valid_coordinates( Grid_row row,  Grid_col col) const
   {  // Note row and col must be signed
      return   (row >= 0) && (row < rows) && (col >=0) && (col < cols);
   }
   //___________________________________________________________________________
   virtual void update_extremes()
   {
      lowest_value  = std::numeric_limits<T>::max(); // We set to the max then minimize
      highest_value = std::numeric_limits<T>::min(); // We set to the min then maximize
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
   //___________________________________________________________________________
   inline float32 get_cell_size() const
      { return cell_size; }
   //___________________________________________________________________________
   void clear_to(T value)
   {
      for (Grid_row r = 0; r < rows; r++)
         for (Grid_col c = 0; c < cols; c++)
            set_at(r,c,value);
   }
   //___________________________________________________________________________
   virtual void clear() { clear_to(no_data_value); }
   // Sets all cells to no_data_value
   // Must be defined in base classes which have the no_data_value
   //___________________________________________________________________________
   virtual void dispose_grid()
   {
      if (grid)
      {  for (Grid_row r = 0; r < rows; r++)
            if (grid[r])
               delete [] grid[r];
         delete [] grid;
      }
      grid = 0;
   }
   //___________________________________________________________________________
   bool set_range
   (T no_data_value_
   ,T lowest_value_
   ,T highest_value_)
   {  no_data_value = no_data_value_;
      lowest_value  = lowest_value_;
      highest_value = highest_value_;
      return true;
   }
   //___________________________________________________________________________
   inline bool dimensioned() const
   { return (rows>UNDIMENSIONED_GRID_SIZE) && cols>UNDIMENSIONED_GRID_SIZE; }
   //___________________________________________________________________________
   inline virtual T set_at(Grid_row row, Grid_col col,T value)
   {  return  (grid && valid_coordinates(row,col))
         ? (grid[row][col] = value)  // assignment not comparison
         : no_data_value;
   }
   //___________________________________________________________________________
   inline virtual T get_at(Grid_row row, Grid_col col) const
   {
      return (grid && valid_coordinates(row,col))
      ? grid[row][col]
      : no_data_value;
   }
   //_get_at___________________________________________________________________/
public: // properties accessors
   inline virtual T get_lowest_value()            const { return lowest_value;}
   inline virtual T get_highest_value()           const { return highest_value;}
   inline  set_no_data_value(T nodata_)        { return no_data_value = nodata;}
   inline T get_no_data_value()                   const { return no_data_value;}
};
//_Grid________________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

