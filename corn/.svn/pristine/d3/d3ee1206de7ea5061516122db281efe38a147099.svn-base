#ifndef units_measureH
#define units_measureH

// UM_      Units of measure   base code for units composition

// UM codes are general "Units of Measure" and are used to compose units codes
// UM codes cannot be used by themselves, they must be or'ed with bit 30
// and or'ed with the units system (I.e. metric kilo, hecto, milli ...)

#define UM_temperature                 (nat8) 0x01
#define UM_thermaldynamic_temperature  (nat8) 0x02
// UM_XXXXX not currently used         (nat8) 0x03   (was thermal time)
#define UM_chemical_weight             (nat8) 0x04
#define UM_amount_of_substance         (nat8) 0x05
#define UM_mass                        (nat8) 0x06

#define UM_weight_liquid               (nat8) 0x07
// reserved.
// There may be English units of liquid weight but I haven't see this
// Otherwise UM_weight_liquid is probably obsolete

//#define UM_weight_dry                 (nat8) 0x08
// obsolete?

#define UM_luminous_intensity          (nat8) 0x0C

#define UM_luminous_flux               (nat8) 0x0D
// not currently used  (nat8) 0x09
// not currently used  (nat8) 0x0A
// not currently used  (nat8) 0x0B

#define UM_illuminance                 (nat8) 0x0E

// not currently used  (nat8) 0x0F

// 1C to 1F are currently free,

// 1 dimensional units:
// Linear measure (I.e. meters)
//    I had originally intended to have linear measures with the following qualifications
//      but decided it would be overkill and greatly complicate units conversions.
//       Length      parallel to the plane of reference
//       Distance    from point to point or center to center
//       Width       perpendicular to the (horizontal plane of reference
//       Breadth     through the center (widest) point or width at a given interval along the length
//       Diameter
//       Height
//       Depth
//       Altitude    Areal measure from reference surface up

#define UM_linear                      (nat8) 0x20   // 00100000 00000000
// ! deprecated UM_distance_D                 (nat8) 0x21
// ! deprecated UM_width                      (nat8) 0x22
// ! deprecated UM_breadth_D                  (nat8) 0x23
// ! deprecated UM_height_D                   (nat8) 0x24   // 00100100 00000000
// ! deprecated UM_depth_D                    (nat8) 0x25
// ! deprecated UM_altitude_D                 (nat8) 0x26   // 00100110 00000000

// 2 dimensional:
// Area measure in square (This could probably be a composite of meter length / breadth /depth etc.).
//    Land area (are) has a separate unit of measure
//    Area may have the following qualifications:
//       flat area
//       surface area (curved or convoluted surface
#define UM_rectangular_area            (nat8) 0x28   // 00101000 00000000
#define UM_area                        (nat8) 0x28   // 00101000 00000000
#define UM_land_area                   (nat8) 0x2A   // 00101010 00000000

// ! NYI                                           (nat8) 0x2B   // 00101011 00000000

// ! 3 dimensional
// !    I had originally intended for volume to have the following qualifications:
// !       solid volume
// !       liquid volume
// !       gas volume
#define UM_volume                      (nat8) 0x30   // 00101100 00000000
// ! 040715 depricated UM_cubic_solid_volume    0x31
// ! 040715 depricated UM_cubic_liquid_volume   0x32
// ! 040715 depricated UM_cubic_gas_volume      0x33
#define UM_liquid                      (nat8) 0x34   // 00101110 00000000
#define UM_dry                         (nat8) 0x35
// NYI  Reserved codes                    (nat8) 0x36  - 0x3F
#define UM_plane_angle                 (nat8) 0x40
#define UM_solid_angle                 (nat8) 0x41
#define UM_frequency                   (nat8) 0x42
#define UM_energy                      (nat8) 0x4C
// special measures reservered for energy rates 0x4d- 0x4E


#define UM_J_per_kg_K_specific_heat    (nat16)0x4800
// Specific heat is actually Joules per kg times kelvin
// since this is a complex unit, I create a new unit of measure


#define UM_heat                        (nat8) 0x4F
// Currently reserved for for possible use as thermal units (will apply to to BTU).

#define UM_thermal_conductivity        (nat8) 0x49

#define UM_force                       (nat8) 0x50

// NYI  forces                                  (nat8) 0x51
// NYI  forces                                  (nat8) 0x52
// NYI  forces                                  (nat8) 0x53
// NYI   was power but power (watt) but watt is J/sec  (nat8) 0x54
#define UM_power                       (nat8) 0x54

#define UM_radiant_flux                (nat8) 0x55

#define UM_pressure                    (nat8) 0x58
#define UM_pressure_hydrostatic        (nat8) 0x59
// Atmospheric pressure is a specialization of hydrostatic
#define UM_pressure_atmospheric        (nat8) 0x5A
// atmspheric pressure may have different basis
// so need to qualify for reference basis
// see https://en.wikipedia.org/wiki/Standard_conditions_for_temperature_and_pressure
// https://en.wikipedia.org/wiki/Atmosphere_(unit)
#define UM_pressure_atmospheric_NIST   (nat8) 0x5A
#define UM_pressure_barometric         (nat8) 0x5A
// barometric and atmospheric are synonyms for pre 1982 standards
#define UM_pressure_atmospheric_IUPAC  (nat8) 0x5B
#define UM_pressure_atmospheric_FAA    (nat8) 0x5C

// Note that 00 value units I.e. 0x5A00 are 'atmosperes'
#define UC_pressure_barometric         (nat16) 0x5A00


// NYI reserved code                                          (nat8) 0x5A

#define UM_pressure_osmotic            (nat8) 0x5B
// NYI   reserved code                 (nat8) 0x5C
// NYI   reserved code                 (nat8) 0x5D
#define UM_viscosity_dynamic           (nat8) 0x5E
#define UM_viscosity_kinematic         (nat8) 0x5F

// Transdimensional?
// NYI                                           (nat8) 0x60 - 0x6F

#define UM_electrical_current          (nat8) 0x70
#define UM_magnetomotive_force UM_electrical_current
// synonym

#define UM_electric_charge             (nat8) 0x71
//NYI   UM_electricity                (nat8) 0x72
#define UM_electric_potential          (nat8) 0x73
#define UM_voltage                        UM_electric_potential
#define UM_electric_potential_difference  UM_electric_potential
// now free  0x74
#define UM_electric_capacitance        (nat8) 0x75
#define UM_electric_resistance         (nat8) 0x76
#define UM_electric_conductance        (nat8) 0x77
#define UM_electric_inductance         (nat8) 0x78
#define UM_magnetic_flux               (nat8) 0x79
#define UM_magnetic_flux_density       (nat8) 0x7A
// Perhaps add  magnetic dipole moment emu in the CGS system
// NYI                                                    7B-7C
#define UM_radionuclide_activity       (nat8) 0x7D
#define UM_absorbed_dose               (nat8) 0x7E
#define UM_dose_equivalent             (nat8) 0x7F

// We can't have units codes greater than 7F since MSB 7 is reserved

#endif
