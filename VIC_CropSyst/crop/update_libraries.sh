
#This script should be run after updating VIC_CropSyst from the repository.
echo 'load specific library source files used by VIC_CropSyst from svn export http repositories'

if [ ! -d common ]; then
   mkdir common
   svn checkout http://nas1.bsyse.wsu.edu/dev/common/trunk/geodesy           common/geodesy
#   svn checkout http://nas1.bsyse.wsu.edu/dev/common/trunk/geolocation           common/geolocation
   svn checkout http://nas1.bsyse.wsu.edu/dev/common/trunk/biometeorology        common/biometeorology
   svn checkout http://nas1.bsyse.wsu.edu/dev/common/trunk/physics               common/physics
   svn checkout http://nas1.bsyse.wsu.edu/dev/common/trunk/soil                  common/soil
   svn checkout http://nas1.bsyse.wsu.edu/dev/common/trunk/simulation            common/simulation
   mkdir common/weather
   mkdir common/weather/parameter
   svn checkout http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/parameter     common/weather/parameter
else
   svn update common/geodesy
#   svn update common/geolocation
   svn update common/biometeorology
   svn update common/physics
   svn update common/soil
   svn update common/simulation
fi
#removing unused units:
#rm common/geolocation/geolocation_item.*
#rm common/geolocation/USA_state_FIPS_code.*
#rm common/geolocation/UTM.*

#rm common/biometeorology/ET_influential*
#rm common/biometeorology/ET_reference_FAO*
#rm -r common/soil/dll
#rm -r common/soil/hydrology
#rm common/soil/3-Pradeep.pdf
#rm common/soil/texttri.*
#rm common/soil/TEXTTRI.*


if [ ! -d CropSyst ]; then
mkdir CropSyst
fi
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/static_phrases.cpp		CropSyst/static_phrases.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/static_phrases.h		CropSyst/static_phrases.h

if [ ! -d CropSyst/crop ]; then
mkdir CropSyst/crop
fi

svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/canopy_biomass_continuum.cpp			CropSyst/crop/canopy_biomass_continuum.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/canopy_biomass_continuum.h			CropSyst/crop/canopy_biomass_continuum.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/canopy_cover_continuum.cpp			CropSyst/crop/canopy_cover_continuum.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/canopy_cover_continuum.h				CropSyst/crop/canopy_cover_continuum.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/CO2_response.cpp						CropSyst/crop/CO2_response.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/CO2_response.h						CropSyst/crop/CO2_response.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_param_class.cpp					CropSyst/crop/crop_param_class.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_param_class.h					CropSyst/crop/crop_param_class.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_param_struct.cpp                 CropSyst/crop/crop_param_struct.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_param_struct.h                   CropSyst/crop/crop_param_struct.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_param_V5.cpp                     CropSyst/crop/crop_param_V5.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_param_V5.h                       CropSyst/crop/crop_param_V5.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_types.c                          CropSyst/crop/crop_types.c
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_types.h                          CropSyst/crop/crop_types.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/crop_types.hpp                        CropSyst/crop/crop_types.hpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/growth_stages.c                       CropSyst/crop/growth_stages.c
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/growth_stages.cpp                     CropSyst/crop/growth_stages.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/growth_stages.h                       CropSyst/crop/growth_stages.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/growth_stages.hpp                     CropSyst/crop/growth_stages.hpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/harvest_grain_asymptotic.c            CropSyst/crop/harvest_grain_asymptotic.c
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/phenology_interface.h                 CropSyst/crop/phenology_interface.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/crop/transpiration_interface.h             CropSyst/crop/transpiration_interface.h


if [ ! -d CropSyst/soil ]; then
mkdir CropSyst/soil
fi

svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/hydrology_interface.h              CropSyst/soil/hydrology_interface.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/layers_interface.h                 CropSyst/soil/layers_interface.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_interface.cpp                 CropSyst/soil/soil_interface.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_interface.h                   CropSyst/soil/soil_interface.h


svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/hydraulic_properties.cpp           CropSyst/soil/hydraulic_properties.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/hydraulic_properties.h             CropSyst/soil/hydraulic_properties.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param.cpp                     CropSyst/soil/soil_param.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param.h                       CropSyst/soil/soil_param.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param_class.cpp               CropSyst/soil/soil_param_class.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param_class.h                 CropSyst/soil/soil_param_class.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param_struct.cpp              CropSyst/soil/soil_param_struct.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param_struct.h                CropSyst/soil/soil_param_struct.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param_V5.cpp                  CropSyst/soil/soil_param_V5.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_param_V5.h                    CropSyst/soil/soil_param_V5.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_properties_sublayers.cpp      CropSyst/soil/soil_properties_sublayers.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/soil_properties_sublayers.h        CropSyst/soil/soil_properties_sublayers.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/sublayers.cpp                      CropSyst/soil/sublayers.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/sublayers.h                        CropSyst/soil/sublayers.h
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/texture_interface.cpp              CropSyst/soil/texture_interface.cpp
svn export http://nas1.bsyse.wsu.edu/dev/CropSyst/trunk/soil/texture_interface.h                CropSyst/soil/texture_interface.h

if [ ! -d USDA ]; then
mkdir USDA
fi

if [ ! -d USDA/NRCS ]; then
mkdir USDA/NRCS
fi

if [ ! -d USDA/NRCS/soil ]; then
mkdir USDA/NRCS/soil
fi

if [ ! -d USDA/NRCS/soil/survey ]; then
mkdir USDA/NRCS/soil/survey
fi

svn export http://nas1.bsyse.wsu.edu/dev/USDA/trunk/NRCS/soil/survey/domains.cpp					USDA/NRCS/soil/survey/domains.cpp
svn export http://nas1.bsyse.wsu.edu/dev/USDA/trunk/NRCS/soil/survey/domains.h                      USDA/NRCS/soil/survey/domains.h

if [ ! -d UED ]; then
mkdir UED
mkdir UED/library				
fi
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/codes.h             UED/library/codes.h
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/std_codes.cpp       UED/library/std_codes.cpp
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/std_codes.h         UED/library/std_codes.h
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/UED_fname.cpp       UED/library/UED_fname.cpp
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/UED_fname.h         UED/library/UED_fname.h
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/UED_types.h         UED/library/UED_types.h
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/varcodes.cpp        UED/library/varcodes.cpp
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/varcodes.h          UED/library/varcodes.h
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/variable.cpp        UED/library/variable.cpp
svn export http://nas1.bsyse.wsu.edu/dev/UED/library/variable.h          UED/library/variable.h


svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/current_elements_interface.h			  common/weather/current_elements_interface.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/ET_and_plant_influential_elements.h     common/weather/ET_and_plant_influential_elements.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/loc_param.cpp                           common/weather/loc_param.cpp
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/loc_param.h                             common/weather/loc_param.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/plant_influential_elements.h            common/weather/plant_influential_elements.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/solar_rad.cpp                           common/weather/solar_rad.cpp
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/solar_rad.h                             common/weather/solar_rad.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/sun.cpp                                 common/weather/sun.cpp
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/sun.h                                   common/weather/sun.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/snow.cpp                                common/weather/snow.cpp
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/snow.h                                  common/weather/snow.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/weather_crop_interface.h                common/weather/weather_crop_interface.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/weather_interface.h                     common/weather/weather_interface.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/weather_provider.cpp                    common/weather/weather_provider.cpp
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/weather_provider.h                      common/weather/weather_provider.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/weather_types.cpp                       common/weather/weather_types.cpp
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/weather/weather_types.h                         common/weather/weather_types.h


if [ ! -d common/residue ]; then
mkdir common/residue
fi
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/residue/residue_decomposition_param.h				common/residue/residue_decomposition_param.h
svn export http://nas1.bsyse.wsu.edu/dev/common/trunk/residue/residue_decomposition_param.cpp			common/residue/residue_decomposition_param.cpp