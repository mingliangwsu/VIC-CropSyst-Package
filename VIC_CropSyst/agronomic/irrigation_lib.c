#include "irrigation_lib.h"
#ifdef VIC_CROPSYST_VERSION
/***********************************************************
this library was created for VIC-CropSyst coupling to consider
the effect of different irrigation systems
it used to be under surface_fluxes.c and  store some information
about irrigation systems and efficiencies

2011-sep library was created                             KJC
2012-Nov-14 seperate irrigation library was created and
            moved from from surface_fluxes.c              KM

***********************************************************/
// IRRIGATION_TYPE_COUNT should be available but was not and we put 25 instead
//Irrigation_library[25] =//				Efficiency		Drop evaporation		Runoff		Deep Percolation
//                    {{/*	Big Gun	*/	                    1	,{	0.55	,	0.4	,	0.05	,	0.0	}}
//                    ,{/*	Big Gun/Center Pivot	*/	    2	,{	0.6	,	0.35	,	0.05	,	0.0	}}
//                    ,{/*	Big Gun/Sprinkler	*/	        6	,{	0.6	,	0.35	,	0.05	,	0.0	}}
//                    ,{/*	Big Gun/Wheel Line	*/	        8	,{	0.6	,	0.35	,	0.05	,	0.0	}}
//                    ,{/*	Center Pivot	*/	            9	,{	0.85	,	0.15	,	0.0	, 0.0	}}
//                    ,{/*	Center Pivot/Drip/Sprinkler	*/	11	,{	0.85	,	0.12	,	0.03	,	0.0	}}
//                    ,{/*	Center Pivot/None	*/	        12	,{	0.85	,	0.12	,	0.03	,	0.0	}}
//                    ,{/*	Center Pivot/Rill	*/	        13	,{	0.85	,	0.12	,	0.03	,	0.0	}}
//                    ,{/*	Center Pivot/Rill/Sprinkler	*/	14	,{	0.85	,	0.12	,	0.03	,	0.0	}}
//                    ,{/*	Center Pivot/Sprinkler	*/	    16	,{	0.85	,	0.12	,	0.03	,	0.0	}}
//                    ,{/*	Center Pivot/Sprinkler/WheelLine*/	17	,{	0.85	,	0.12	,	0.03	,	0.0	}}
//                    ,{/*	Center Pivot/Wheel Line	*/	    19	,{	0.85	,	0.12	,	0.03	,	0.0	}}
//                    ,{/*	Drip	*/	                    20	,{	0.97	,	0.03	,	0.0	,	0.0	}}
//                    ,{/*	Drip/Big Gun	*/	            21	,{	0.98	,	0.02	,	0.0	,	0.0	}}
//                   ,{/*	Drip/Rill	*/	                23	,{	0.98	,	0.02	,	0.0	,	0.0	}}
//                    ,{/*	Drip/Sprinkler	*/	            25	,{	0.98	,	0.02	,	0.0	,	0.0	}}
//                    ,{/*	Flood	*/	                    28	,{	0.35	,	0	,	0.30	,	0.35	}}
//                    ,{/*	Rill	*/	                    37 28	,{	0.5	,	0.0	,	0.15	,	0.35	}}
//                    ,{/*	Rill/Sprinkler	*/	            38	,{	0.4	,	0.0	,	0.2	,	0.4	}}
//                    ,{/*	Rill/Wheel Line	*/	            40	,{	0.4	,	0.0	,	0.2	,	0.4	}}
//                   ,{/*	Sprinkler	*/	                41	,{	0.8	,	0.20	,	0.00	,	0.0	}}
//                    ,{/*	Sprinkler/Wheel Line	*/	    42	,{	0.7	,	0.25	,	0.05	,	0.0	}}
//                    ,{/*	Unknown	*/	                    43	,{	0.4	,	0.0	,	0.2	,	0.4	}}
//                    ,{/*	Wheel Line	*/	                44	,{	0.7	,	0.30	,	0.00	,	0.0	}}
//                    ,{/*	GENERIC	*/	                    45	,{	0.66625	,	0.155	,	0.07875	,	0.1	}}};
//

struct VIC_Irrigation_library Irrigation_library[IRRIGATION_TYPE_ACCOUNTS] =
    #if (VIC_CROPSYST_VERSION>=3)
    {///**type_model_compony**/irrig index,{Runoff,D (mm),h (kg/cm2),H (m),cd,y0 (m),x (m),Q,q,v (m/sec),A/Ap,tirrig, actual_droplet_size}}
        /*170830LML
         #ifdef USE_SIMPLIFIED_IRRIGATION_TYPES
         {CENTER_PIVOT, {0.75,  0.15,   0.10,   0.00,   20.0}}
        ,{BIG_GUN,      {0.75,  0.15,   0.10,   0.00,   20.0}}
        ,{SPRINKLER,    {0.75,  0.15,   0.10,   0.00,   20.0}}
        ,{WHEEL_LINE,   {0.75,  0.15,   0.10,   0.00,   20.0}}
        ,{DRIP,         {0.95,  0.00,   0.05,   0.00,   20.0}}
        ,{FLOOD,        {0.60,  0.30,   0.10,   0.00,   20.0}}
        ,{RILL,         {0.70,  0.15,   0.15,   0.00,   20.0}}
        ,{FURROW,       {0.60,  0.30,   0.10,   0.00,   20.0}}
        //151211LML need Kevan's real data to fill in!!!
        #else
        */
        {IrrigTP_CP_impact_14VH_RainBird,{0.03,2.78,4.1,41,0.9,1.8,11.7,0.61,23,25.5,0.1,24,1.16}}
        ,{IrrigTP_CP_impact_M20VH_PM_RainBird,{0.03,1.59,1.7,17,0.9,1.1,7.8,0.13,15,16.4,0.1,24,1.42}}
        ,{IrrigTP_CP_impact_65PJ_RainBird,{0.03,6.35,5.5,55,0.9,3,19.8,3.75,27,29.6,0.1,24,1.68}}
        ,{IrrigTP_CP_impact_30FH_30FWH_RainBird,{0.03,3.18,5.5,55,0.9,2.7,13.3,0.92,27,29.6,0.1,24,0.99}}
        ,{IrrigTP_CP_impact_L36H_L36AH_RainBird_1,{0.03,7.14,3.5,35,0.9,4,14,3.7,10,23.6,0.1,24,2.74}}
        ,{IrrigTP_CP_impact_L36H_L36AH_RainBird_2,{0.03,3.97,1.4,14,0.9,4,13.1,0.95,10,14.9,0.1,24,2.6845}}
        ,{IrrigTP_CP_impact_85EHD_RainBird_1,{0.03,8.7,1.7,17,0.9,4.3,18.6,3.88,27,16.4,0.1,24,3.4}}
        ,{IrrigTP_CP_impact_85EHD_RainBird_2,{0.03,12.7,3.5,35,0.9,4.3,26.6,11,27,23.6,0.1,24,2.17}}
        ,{IrrigTP_CP_impact_85EHD_RainBird_3,{0.03,17.5,6.9,69,0.9,4.3,35.4,29,27,33.1,0.1,24,1.91}}
        ,{IrrigTP_CP_impact_85EHD_LA_RainBird_1,{0.03,8.7,1.7,17,0.9,3,15.8,3.88,17,16.4,0.1,24,2.52}}
        ,{IrrigTP_CP_impact_85EHD_LA_RainBird_2,{0.03,12.7,3.5,35,0.9,3,24.2,11,17,23.6,0.1,24,2.17}}
        ,{IrrigTP_CP_impact_85EHD_LA_RainBird_3,{0.03,17.5,6.9,69,0.9,3,32.9,29,17,33.1,0.1,24,1.91}}
        ,{IrrigTP_CP_spray_S3000_Nelson,{0.03,2.44064516129032,1,10,0.9,0.9,10.4,3.63,5,12.6,0.1,24,1.89}}
        ,{IrrigTP_CP_spray_O3000_Nelson,{0.03,7.32193548387097,1,10,0.9,1.8,14.3,32.74,5,12.6,0.1,24,3.05726451612903}}
        ,{IrrigTP_CP_spray_R3000_Nelson_1,{0.03,6.50838709677419,2.04,20.4,0.9,3.7,22.6,36.9,5,18,0.1,24,2.06569259962049}}
        ,{IrrigTP_CP_spray_R3000_Nelson_2,{0.03,7.32193548387097,1.7,17,0.9,1.8,17.7,42.24,5,16.4,0.1,24,2.33368500948767}}
        ,{IrrigTP_CP_spray_A3000_Nelson_1,{0.03,2.03387096774194,0.69,6.9,0.9,1.8,9.1,2.04,5,10.5,0.1,24,2.00743338008415}}
        ,{IrrigTP_CP_spray_A3000_Nelson_2,{0.03,8.53548387096774,0.69,6.9,0.9,2.7,16.5,37.13,5,10.5,0.1,24,2.27}}
        ,{DEFAULT_CENTER_PIVOT,{0.03,17.5,6.9,69,0.9,4.3,35.4,29,27,33.1,0.1,24,1.91}}             //180126 LML_MK set as IrrigTP_CP_impact_85EHD_RainBird_3

        ,{IrrigTP_Big_Gun_75TR_Nelson_1,{0.05,10.2,3,30,0.9,2,47,6.6,24,21.8,0.2,24,2.116}}
        ,{IrrigTP_Big_Gun_75TR_Nelson_2,{0.05,14,1.75,17.5,0.9,2,44,9.5,24,16.7,0.2,24,3.22}}
        ,{IrrigTP_Big_Gun_75TR_Nelson_3,{0.05,20.3,6,60,0.9,2,87,37.2,24,30.9,0.2,24,2.112}}
        ,{IrrigTP_Big_Gun_150TB_Nelson_1,{0.05,17.8,3.5,35,0.9,2,76,23,24,23.6,0.2,24,2.52}}
        ,{IrrigTP_Big_Gun_150TB_Nelson_2,{0.05,30.5,7,70,0.9,2,128,96.6,24,33.4,0.2,24,2.35}}
        ,{IrrigTP_Big_Gun_150TB_Nelson_3,{0.05,35.6,9,90,0.9,2,149,148,24,37.8,0.2,24,2.25}}
        ,{IrrigTP_Big_Gun_200TB_Nelson_1,{0.05,26.7,4,40,0.9,2,104,55.7,27,25.2,0.2,24,2.902}}
        ,{IrrigTP_Big_Gun_200TB_Nelson_2,{0.05,40.6,9,90,0.9,2,170,195,27,37.8,0.2,24,2.38}}
        ,{IrrigTP_Big_Gun_200TB_Nelson_3,{0.05,48.3,9,90,0.9,2,188,274,27,37.8,0.2,24,2.588}}
        ,{DEFAULT_BIG_GUN,{0.05,30.5,7,70,0.9,2,128,96.6,24,33.4,0.2,24,2.35}}                      //180126 LML_MK set as IrrigTP_Big_Gun_150TB_Nelson_2

        ,{IrrigTP_Solid_set_R5_POP_UP_Nelson,{0.05,10.1693548387097,2.75,27.5,0.9,0.501515,5.5,110,9,20.9,0.1,24,2.19}}
        ,{IrrigTP_Solid_set_R2000WF_6_Nelson_1,{0.05,2.03387096774194,2.75,27.5,0.9,1.001515,10.7,254,25,20.9,0.1,24,1.47750146627566}}
        ,{IrrigTP_Solid_set_R2000WF_6_Nelson_2,{0.05,3.0508064516129,2.75,27.5,0.9,1.001515,11.9,574,25,20.9,0.1,24,1.57}}
        ,{IrrigTP_Solid_set_R2000WF_6_Nelson_3,{0.05,4.06774193548387,4.5,45,0.9,1.001515,13.7,1295,25,26.7,0.1,24,1.52}}
        ,{IrrigTP_Solid_set_R33_Nelson,{0.05,4.8,4.5,45,0.9,1.001515,15.2,1862,25,26.7,0.1,24,1.556}}
        ,{DEFAULT_SOLID_SET,{0.05,4.06774193548387,4.5,45,0.9,1.001515,13.7,1295,25,26.7,0.1,24,1.52}} //180126 LML_MK set as IrrigTP_Solid_set_R2000WF_6_Nelson_3

        ,{IrrigTP_Moving_wheel_R2000WF_6_Nelson_1,{0.05,2.03387096774194,2.75,27.5,0.9,1.001515,10.7,254,25,20.9,0.1,24,1.48}}
        ,{IrrigTP_Moving_wheel_R2000WF_6_Nelson_2,{0.05,3.0508064516129,2.75,27.5,0.9,1.001515,11.9,574,25,20.9,0.1,24,1.57}}
        ,{IrrigTP_Moving_wheel_R2000WF_6_Nelson_3,{0.05,4.06774193548387,4.5,45,0.9,1.001515,13.7,1295,25,26.7,0.1,24,1.52}}
        ,{DEFAULT_MOVING_WHEEL,{0.05,3.0508064516129,2.75,27.5,0.9,1.001515,11.9,574,25,20.9,0.1,24,1.57}}   //180126 LML_MK set as IrrigTP_Moving_wheel_R2000WF_6_Nelson_2

        ,{IrrigTP_drip_0_0,{0,0,0,0,0,0,0,0,0,0,0.1,5,0}}
        ,{IrrigTP_Sub_surf_drip_0_0,{0,0,0,0,0,0,0,0,0,0,0.1,5,0}}
        ,{IrrigTP_Sub_surf_drip_perfect,{0,0,0,0,0,0,0,0,0,0,0.1,5,0}}                                      //190724LML
        ,{IrrigTP_Sub_surf_drip_perfect_eliminate_top,{0,0,0,0,0,0,0,0,0,0,0.1,5,0}}                        //190805
        ,{DEFAULT_DRIP,{0,0,0,0,0,0,0,0,0,0,0.1,5,0}}                                                       //180126 LML_MK set as IrrigTP_drip_0_0

        ,{IrrigTP_flood_0_0,{0.3,0,0,0,0,0,0,0,0,0,0.9,5,0}}
        ,{IrrigTP_rill_0_0,{0.2,0,0,0,0,0,0,0,0,0,0.9,5,0}}
        ,{IrrigTP_furrow_0_0,{0.3,0,0,0,0,0,0,0,0,0,0.9,5,0}}
        //170830LML #endif
    };
#else
{///**type_model_compony**/irrig index,{Runoff,D (mm),h (kg/cm2),H (m),cd,y0 (m),x (m),Q,q,v (m/sec),A/Ap,tirrig, actual_droplet_size}}
 { 1,{0.03,2.78,4.1,41,0.9,1.8,11.7,0.61,23,25.5,0.1,24,1.16}}                   //CP-impact_14VH_RainBird
,{ 2,{0.03,1.59,1.7,17,0.9,1.1,7.8,0.13,15,16.4,0.1,24,1.42}}                    //CP-impact_M20VH-PM_RainBird
,{ 3,{0.03,6.35,5.5,55,0.9,3,19.8,3.75,27,29.6,0.1,24,1.68}}                     //CP-impact_65PJ_RainBird
,{ 4,{0.03,3.18,5.5,55,0.9,2.7,13.3,0.92,27,29.6,0.1,24,0.99}}                   //CP-impact_30FH/30FWH_RainBird
,{ 5,{0.03,7.14,3.5,35,0.9,4,14,3.7,10,23.6,0.1,24,2.74}}                        //CP-impact_L36H/L36AH_RainBird
,{ 6,{0.03,3.97,1.4,14,0.9,4,13.1,0.95,10,14.9,0.1,24,2.6845}}                   //CP-impact_L36H/L36AH_RainBird
,{ 7,{0.03,8.7 ,1.7,17,0.9,4.3,18.6,3.88,27,16.4,0.1,24,3.4}}                    //CP-impact_85EHD_RainBird
,{ 8,{0.03,12.7,3.5,35,0.9,4.3,26.6,11,27,23.6,0.1,24,2.17}}                     //CP-impact_85EHD_RainBird
,{ 9,{0.03,17.5,6.9,69,0.9,4.3,35.4,29,27,33.1,0.1,24,1.91}}                     //CP-impact_85EHD_RainBird
,{10,{0.03,8.7 ,1.7,17,0.9,3,15.8,3.88,17,16.4,0.1,24,2.52}}                     //CP-impact_85EHD-LA_RainBird
,{11,{0.03,12.7,3.5,35,0.9,3,24.2,11,17,23.6,0.1,24,2.17}}                       //CP-impact_85EHD-LA_RainBird
,{12,{0.03,17.5,6.9,69,0.9,3,32.9,29,17,33.1,0.1,24,1.91}}                       //CP-impact_85EHD-LA_RainBird
,{21,{0.03,2.44064516129032,1,10,0.9,0.9,10.4,3.63,5,12.6,0.1,24,1.89}}          //CP-spray_S3000_Nelson
,{22,{0.03,7.32193548387097,1,10,0.9,1.8,14.3,32.74,5,12.6,0.1,24,3.05726451612903}}   //CP-spray_O3000_Nelson
,{23,{0.03,6.50838709677419,2.04,20.4,0.9,3.7,22.6,36.9,5,18,0.1,24,2.06569259962049}} //CP-spray_R3000_Nelson
,{24,{0.03,7.32193548387097,1.7,17,0.9,1.8,17.7,42.24,5,16.4,0.1,24,2.33368500948767}} //CP-spray_R3000_Nelson
,{25,{0.03,2.03387096774194,0.69,6.9,0.9,1.8,9.1,2.04,5,10.5,0.1,24,2.00743338008415}} //CP-spray_A3000_Nelson
,{26,{0.03,8.53548387096774,0.69,6.9,0.9,2.7,16.5,37.13,5,10.5,0.1,24,2.27}}           //CP-spray_A3000_Nelson
,{31,{0.05,10.2,3,30,0.9,2,47,6.6,24,21.8,0.2,24,2.116}}                         //Big-Gun_75TR_Nelson
,{32,{0.05,14  ,1.75,17.5,0.9,2,44,9.5,24,16.7,0.2,24,3.22}}                     //Big-Gun_75TR_Nelson
,{33,{0.05,20.3,6,60,0.9,2,87,37.2,24,30.9,0.2,24,2.112}}                        //Big-Gun_75TR_Nelson
,{34,{0.05,17.8,3.5,35,0.9,2,76,23,24,23.6,0.2,24,2.52}}                         //Big-Gun_150TB_Nelson
,{35,{0.05,30.5,7,70,0.9,2,128,96.6,24,33.4,0.2,24,2.35}}                        //Big-Gun_150TB_Nelson
,{36,{0.05,35.6,9,90,0.9,2,149,148,24,37.8,0.2,24,2.25}}                         //Big-Gun_150TB_Nelson
,{37,{0.05,26.7,4,40,0.9,2,104,55.7,27,25.2,0.2,24,2.902}}                       //Big-Gun_200TB_Nelson
,{38,{0.05,40.6,9,90,0.9,2,170,195,27,37.8,0.2,24,2.38}}                         //Big-Gun_200TB_Nelson
,{39,{0.05,48.3,9,90,0.9,2,188,274,27,37.8,0.2,24,2.588}}                        //Big-Gun_200TB_Nelson
,{41,{0.05,10.1693548387097,2.75,27.5,0.9,0.501515,5.5,110,9,20.9,0.1,24,2.19}}  //Solid-set_R5-POP-UP_Nelson
,{42,{0.05, 2.03387096774194,2.75,27.5,0.9,1.001515,10.7,254,25,20.9,0.1,24,1.47750146627566}}   //Solid-set_R2000WF-6_Nelson
,{43,{0.05,3.0508064516129,2.75,27.5,0.9,1.001515,11.9,574,25,20.9,0.1,24,1.57}} //Solid-set_R2000WF-6_Nelson
,{44,{0.05,4.06774193548387,4.5,45,0.9,1.001515,13.7,1295,25,26.7,0.1,24,1.52}}  //Solid-set_R2000WF-6_Nelson
,{45,{0.05,4.8,4.5,45,0.9,1.001515,15.2,1862,25,26.7,0.1,24,1.556}}              //Solid-set_R33_Nelson
,{51,{0.05,2.03387096774194,2.75,27.5,0.9,1.001515,10.7,254,25,20.9,0.1,24,1.48}}//Moving-wheel_R2000WF-6_Nelson
,{52,{0.05,3.0508064516129,2.75,27.5,0.9,1.001515,11.9,574,25,20.9,0.1,24,1.57}} //Moving-wheel_R2000WF-6_Nelson
,{53,{0.05,4.06774193548387,4.5,45,0.9,1.001515,13.7,1295,25,26.7,0.1,24,1.52}}  //Moving-wheel_R2000WF-6_Nelson
,{61,{0,0,0,0,0,0,0,0,0,0,0.1,5,0}}                                              //drip_0_0
,{62,{0,0,0,0,0,0,0,0,0,0,0.1,5,0}}                                              //Sub-surf-drip_0_0
,{71,{0.3,0,0,0,0,0,0,0,0,0,0.9,5,0}}                                            //flood_0_0
,{72,{0.2,0,0,0,0,0,0,0,0,0,0.9,5,0}}                                            //rill_0_0
,{73,{0.3,0,0,0,0,0,0,0,0,0,0.9,5,0}}                                            //furrow _0_0
};
#endif
//_170830LML____________________________________________________________________
General_Irrigation_Type identify_general_irrigation_type(
    const Irrigation_Type irrigation)
{
    General_Irrigation_Type gtype = GENERAL_NO_IRRIGATION;
    if (irrigation >= IrrigTP_CP_impact_14VH_RainBird
        && irrigation <= DEFAULT_CENTER_PIVOT)  gtype = CENTER_PIVOT;
    else if (irrigation >= IrrigTP_Big_Gun_75TR_Nelson_1
        && irrigation <= DEFAULT_BIG_GUN)       gtype = BIG_GUN;
    else if (irrigation >= IrrigTP_Solid_set_R5_POP_UP_Nelson
        && irrigation <= DEFAULT_SOLID_SET)     gtype = SOLID_SET;
    else if (irrigation >= IrrigTP_Moving_wheel_R2000WF_6_Nelson_1
        && irrigation <= DEFAULT_MOVING_WHEEL)  gtype = WHEEL_LINE;
    else if (irrigation >= IrrigTP_drip_0_0
        && irrigation <= DEFAULT_DRIP)          gtype = DRIP;
    else if (irrigation == IrrigTP_flood_0_0)   gtype = FLOOD;
    else if (irrigation == IrrigTP_rill_0_0)    gtype = RILL;
    else if (irrigation == IrrigTP_furrow_0_0)  gtype = FURROW;
    return gtype;
}
//______________________________________________________________________________
void set_irrigation_efficiency_of_irrigation_library(const int management_f)
{
    //management_factor:
    //1: fair management; 2: poor management (15% more water loss);
    //3: good management (15% less water loss)

    extern VIC_Irrigation_library Irrigation_library[IRRIGATION_TYPE_ACCOUNTS];
    for (int i = 0; i < IRRIGATION_TYPE_ACCOUNTS; i++) {
        General_Irrigation_Type gtype = identify_general_irrigation_type(
            Irrigation_library[i].IrrigationType_code);
        Irrigation_Efficiency &irrig_eff =
            Irrigation_library[i].irrigation_efficiency;
        if (gtype == FLOOD || gtype == RILL || gtype == FURROW) {                //surface
            irrig_eff.uniformity = 0.5;
            irrig_eff.maximum_capacity = 150.0;                                   //06272022LML 30
            if (management_f == 1)      irrig_eff.presumed_efficiency = 0.70;
            else if (management_f == 2) irrig_eff.presumed_efficiency = 0.50;
            else                        irrig_eff.presumed_efficiency = 0.80;
            irrig_eff.table_irrigation_efficiency               = 0.5;
            irrig_eff.table_irrigation_deep_percolation_loss    = 0.35;
            irrig_eff.table_irrigation_evap_loss                = 0.0;
            irrig_eff.table_irrigation_runoff_loss              = 0.15;
            irrig_eff.max_allowable_depletion                   = 0.4;          //201111LML 0.5 06232022LML changed to 0.4
        } else if ((gtype == DRIP)
                   && (Irrigation_library[i].IrrigationType_code != IrrigTP_Sub_surf_drip_perfect)
                   && (Irrigation_library[i].IrrigationType_code != IrrigTP_Sub_surf_drip_perfect_eliminate_top)) {
            irrig_eff.time_irrig = 20;
            irrig_eff.uniformity = 0.95;
            irrig_eff.maximum_capacity = 150.0;                                   //06272022LML 20
            if (management_f == 1)      irrig_eff.presumed_efficiency = 0.90;
            else if (management_f == 2) irrig_eff.presumed_efficiency = 0.70;
            else                        irrig_eff.presumed_efficiency = 0.95;
            irrig_eff.table_irrigation_efficiency               = 0.97;
            irrig_eff.table_irrigation_deep_percolation_loss    = 0.0;
            irrig_eff.table_irrigation_evap_loss                = 0.03;
            irrig_eff.table_irrigation_runoff_loss              = 0.0;
            irrig_eff.max_allowable_depletion                   = 0.2;           //201111LML
        } else if ((Irrigation_library[i].IrrigationType_code == IrrigTP_Sub_surf_drip_perfect)
                   || Irrigation_library[i].IrrigationType_code == IrrigTP_Sub_surf_drip_perfect_eliminate_top){   //190724LML
            irrig_eff.time_irrig = 20;
            irrig_eff.uniformity = 1.0;
            irrig_eff.maximum_capacity = 8.0;                 //190725LML COS 50 mm    190918 COS set 8 mm
            irrig_eff.presumed_efficiency = 1.0;
            irrig_eff.table_irrigation_efficiency               = 1.0;
            irrig_eff.table_irrigation_deep_percolation_loss    = 0.0;
            irrig_eff.table_irrigation_evap_loss                = 0.0;
            irrig_eff.table_irrigation_runoff_loss              = 0.0;
            irrig_eff.max_allowable_depletion                   = 0.2;           //201111LML
        } else if (gtype == WHEEL_LINE) {
            irrig_eff.uniformity = 0.95;
            irrig_eff.maximum_capacity = 150.0;                                   //06272022LML 30
            if (management_f == 1)      irrig_eff.presumed_efficiency = 0.8;
            else if (management_f == 2) irrig_eff.presumed_efficiency = 0.65;
            else                        irrig_eff.presumed_efficiency = 0.85;
            irrig_eff.table_irrigation_efficiency               = 0.7;
            irrig_eff.table_irrigation_deep_percolation_loss    = 0.0;
            irrig_eff.table_irrigation_evap_loss                = 0.3;
            irrig_eff.table_irrigation_runoff_loss              = 0.0;
            irrig_eff.max_allowable_depletion                   = 0.4;          //201111LML 0.5 06232022LML changed to 0.4
        } else if (gtype == SOLID_SET) {
            irrig_eff.uniformity = 0.95;
            irrig_eff.maximum_capacity = 150.0;                                   //06272022LML 30
            if (management_f == 1)      irrig_eff.presumed_efficiency = 0.8;
            else if (management_f == 2) irrig_eff.presumed_efficiency = 0.65;
            else                        irrig_eff.presumed_efficiency = 0.85;
            irrig_eff.table_irrigation_efficiency               = 0.8;
            irrig_eff.table_irrigation_deep_percolation_loss    = 0.0;
            irrig_eff.table_irrigation_evap_loss                = 0.2;
            irrig_eff.table_irrigation_runoff_loss              = 0.0;
            irrig_eff.max_allowable_depletion                   = 0.4;          //201111LML 0.5 06232022LML changed to 0.4
        } else if (gtype == BIG_GUN) {
            irrig_eff.uniformity = 0.95;
            irrig_eff.maximum_capacity = 150.0;                                   //06272022LML 30
            if (management_f == 1)      irrig_eff.presumed_efficiency = 0.75;
            else if (management_f == 2) irrig_eff.presumed_efficiency = 0.6;
            else                        irrig_eff.presumed_efficiency = 0.8;
            irrig_eff.table_irrigation_efficiency               = 0.55;
            irrig_eff.table_irrigation_deep_percolation_loss    = 0.0;
            irrig_eff.table_irrigation_evap_loss                = 0.4;
            irrig_eff.table_irrigation_runoff_loss              = 0.05;
            irrig_eff.max_allowable_depletion                   = 0.4;          //201111LML 0.5 06232022LML changed to 0.4
        } else if (gtype == CENTER_PIVOT) {
            irrig_eff.uniformity = 0.92;
            irrig_eff.maximum_capacity = 150.0;                                   //06272022LML 20
            if (management_f == 1)      irrig_eff.presumed_efficiency = 0.85;
            else if (management_f == 2) irrig_eff.presumed_efficiency = 0.75;
            else                        irrig_eff.presumed_efficiency = 0.90;
            irrig_eff.table_irrigation_efficiency               = 0.85;
            irrig_eff.table_irrigation_deep_percolation_loss    = 0.0;
            irrig_eff.table_irrigation_evap_loss                = 0.12;
            irrig_eff.table_irrigation_runoff_loss              = 0.03;
            irrig_eff.max_allowable_depletion                   = 0.2;           //201111LML
        }
    } //i
}
#endif //VIC_CROPSYST_VERSION
