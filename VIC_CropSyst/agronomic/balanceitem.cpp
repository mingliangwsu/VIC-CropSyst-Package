#include "balanceitem.h"
#include <iostream>
template <typename T> inline
bool is_approximately(const T &value,const T &target,
                      const T &tolerance=0.0000007)
{ return (value < (target + tolerance))
         && (value > (target - tolerance));
}

//______________________________________________________________________________
BalanceItem::BalanceItem()
:pool_final(0)
,pool_init(0)
,flux_in(0)
,flux_out(0)
{}
//______________________________________________________________________________
bool BalanceItem::IsMassBalance(const double tol)
{
    //LML 150204 assume mass always has positive value
    bool bbalance(true);
    double massbal = (pool_final - pool_init) - (flux_in - flux_out);
    if (is_approximately<double>(massbal,0,tol) && pool_final >= 0.0)
        bbalance = true;    //LML 150204 added logic for negtive pool
    else bbalance = false;
    return bbalance;
}
//______________________________________________________________________________
void BalanceItem::PrintMassBalanceTerms(std::string processname)
{
    //#pragma omp critical
    //{
        std::clog<<"-----------------------------------------------"
                 <<"\nMass balance in " <<processname
                 <<"\nNetin:"           <<flux_in
                 <<"\nNetout:"          <<flux_out
                 <<"\nOldpool:"         <<pool_init
                 <<"\nNewpool:"         <<pool_final
                 <<"\nPool change:"     <<pool_final-pool_init
                 <<"\nNet flux:"        <<flux_in-flux_out
                 <<"\nDifference between fluxes and net pool change:"
                 <<(pool_final - pool_init) - (flux_in - flux_out)
                 <<"\n-----------------------------------------------"
                 <<std::endl;
    //}
}
//______________________________________________________________________________
void BalanceItem::ClearPoolAndFluxes()
{
    flux_in = 0;
    flux_out = 0;
    pool_final = 0;
    pool_init = 0;
}
//______________________________________________________________________________
