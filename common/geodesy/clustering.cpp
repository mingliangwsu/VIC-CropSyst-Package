#include "common/geodesy/clustering.h"
#include "common/geodesy/geometry/terrestrial_geometry.h"
#include "corn/math/compare.hpp"
namespace CS
{
Terrestrial_geometry clustering_geometry;
//______________________________________________________________________________
Clustering::Cluster::Cluster
(float32 tolerance_km_)
:geodesy::Geocoordinates()
,tolerance_km(tolerance_km_)
{}
//_Cluster:constructor_________________________________________________________/
nat32 Clustering::Cluster::take_potential_members(geodesy::Geocoordinates &nonmembers)
{  nat32 taken = 0;
   FOR_EACH(member,geodesy::Geocoordinate_item,each_member)
   {  FOR_EACH_IN(nonmember,geodesy::Geocoordinate_item,nonmembers,each_nonmember)
      {  float32 distance = clustering_geometry.distance_between_km(*member,*nonmember);
         if (distance <= tolerance_km)
         {  nonmembers.detach(nonmember);
            take(nonmember);
            taken++;
         }
      } FOR_EACH_END(each_nonmember)
   } FOR_EACH_END(each_member)
   // Recurse until
   if (taken)
   {  nat32 more_taken = 0;
      while ((more_taken = take_potential_members(nonmembers)) != 0)
         taken += more_taken;
   }
   return taken;
}
//_take_potential_members______________________________________________________/
Clustering::Clustering
(const geodesy::Geocoordinates &all_points_
,float32 tolerance_km_)
: all_points(all_points_)
, tolerance_km(tolerance_km_)
, unclustered()
{}
//_Clustering:constructor______________________________________________________/
/* 200216  actually this method is not needed, no special case needed to
instanciate a single point.

nat32 Clustering::cluster_single()
{
   //200216 Cluster *only_cluster = new Cluster(0);

   Cluster *only_cluster = render_cluster();                                     //200216
   only_cluster->transfer_all_from(unclustered);
   clusters.take(only_cluster);
   return only_cluster->count();
}
*/
//_cluster_single______________________________________________________________/
nat32 Clustering::initialize()                                   initialization_
{  nat32 taken = 0;
   unclustered.clone_from(all_points);

/* 200216 not sure why I needed single
   if (CORN::is_approximately<float32>(tolerance_km,0.0))
      taken = cluster_single();
   else
*/
   {  const CORN::OS::Directory_name &netCDF_source_directory =
         get_netCDF_source_superdir();
      while (unclustered.count())
      {

         // Conceptual,  needed
         // Load list of existing clusters (if any)
         // (that may have been previously identified
         // render new clusters that
         // would have been contiguous with existing clusters
         // but would have to be now a new cluster

         geodesy::Geocoordinate_item *geocoord_to_cluster
            = dynamic_cast<geodesy::Geocoordinate_item *>
               (unclustered.pop_tail());
         Cluster *cluster = render_cluster();
         cluster->take(geocoord_to_cluster);
         taken++;
         clusters.take(cluster);
         taken += cluster->take_potential_members(unclustered);
         std::string extent_name;
         cluster->provide_extent().compose_name(extent_name);
         CORN::OS::Directory_name_concrete *source_directory = new
            CORN::OS::Directory_name_concrete
               (get_netCDF_source_superdir(),extent_name);
         cluster->take_netCDF_extent_source_directory(source_directory);
      }
   }
   return taken;
}
//_initialize__________________________________________________________________/
}//_namespace CS_______________________________________________________________/
