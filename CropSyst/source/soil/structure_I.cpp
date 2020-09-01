#include "soil/structure_I.h"
#include "corn/measure/measures.h"
namespace Soil
{
//______________________________________________________________________________
float64 Structure ::get_bulk_density_kg_m3(uint8 layer)                    const
{   // 1000 converts g/cm3 to kg/m3
   return g_cm3_to_kg_m3(get_bulk_density_g_cm3(layer));
}
//_get_bulk_density_kg_m3___________________________________________2005-07-27_/
float64 Structure::set_bulk_density_kg_m3(nat8 layer,float64 _bulk_density_kg_cm3)
{  float64 bulk_density_g_cm3 = kg_m3_to_g_cm3(_bulk_density_kg_cm3);
   return set_bulk_density_g_cm3(layer,bulk_density_g_cm3);
}
//_set_bulk_density_kg_m3___________________________________________2015-04-11_/
float64 Structure::set_bulk_density_Mg_m3
(nat8 layer,float64 _bulk_density_Mg_m3)
{ return set_bulk_density_g_cm3(layer,_bulk_density_Mg_m3) ;} // Equivelent
//_set_bulk_density_Mg_m3___________________________________________2005-07-26_/
float64 Structure::get_bulk_density_Mg_m3(nat8 layer)                      const
{ return get_bulk_density_g_cm3(layer); } // equivelent
//_get_bulk_density_Mg_m3___________________________________________2005-12-04_/
}//_namespace_Soil______________________________________________________________
