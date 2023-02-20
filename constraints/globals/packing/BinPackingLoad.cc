//
// Created by audemard on 02/01/23.
//

#include "BinPackingLoad.h"

#include "solver/Solver.h"

using namespace Cosoco;

//----------------------------------------------
// Check validity and correct definition
//----------------------------------------------

bool BinPackingLoad::isSatisfiedBy(vec<int> &tuple) {
    sums.fill(0);
    for(int i = 0; i < nItems; i++) sums[tuple[i]] += sizes[i];
    for(int i = 0; i < sums.size(); i++)
        if(sums[i] != tuple[nItems + i])
            return false;

    return true;
}


//----------------------------------------------
// Filrtering
//----------------------------------------------

bool BinPackingLoad::filter(Variable *x) {
    /*    if (unassignedVariablesIdx.size() == 0) {
            int[] t = Stream.of(scp).mapToInt(y -> y.dom.singleValue()).toArray();
            if (!isSatisfiedBy(t)) {
                cnt++;
                System.out.println(this + " bef " + cnt);
                System.out.println("limits : " + Kit.join(limits));
                System.out.println("sizes : " + Kit.join(sizes));
                for (Variable y : scp)
                    y.dom.displayCurrentBranch(true);
                System.exit(1);
            }
        }
       */

    // we call the super propagator after setting the highest possible limits
    for(int i = 0; i < nBins; i++) limits[i] = loads[i]->maximum();
    if(BinPacking::filter(x) == false)
        return false;

    sums.fill(0);
    freeItems.clear();
    for(int posx = 0; posx < nItems; posx++)
        if(scope[posx]->size() == 1)
            sums[scope[posx]->value()] += sizes[posx];
        else
            freeItems.add(posx);


    for(int i = 0; i < nBins; i++) {
        long currentFill = sums[i];
        if(loads[i]->size() == 1) {
            int load           = loads[i]->value();
            int possibleExtent = 0;
            int minSize        = INT_MAX;
            for(int k = freeItems.size() - 1; k >= 0; k--) {
                int j = freeItems[k];
                if(scope[j]->containsValue(i)) {
                    if(currentFill + sizes[j] > load) {
                        if(solver->delVal(scope[j], i) == false)
                            return false;
                    } else {
                        possibleExtent += sizes[j];
                        minSize = std::min(minSize, sizes[j]);
                    }
                }
            }

            if(currentFill + possibleExtent < load)
                return false;

            if(currentFill + possibleExtent == load) {
                for(int k = freeItems.size() - 1; k >= 0; k--) {
                    int j = freeItems[k];
                    if(scope[j]->containsValue(i))
                        solver->assignToVal(scope[j], i);
                }
            } else if(currentFill + possibleExtent - minSize < load)
                return false;
        } else {
            if(solver->delValuesLowerOrEqualThan(loads[i], currentFill - 1) == false)
                return false;
            int loadMin        = loads[i]->minimum();
            int loadMax        = loads[i]->maximum();
            int possibleExtent = 0;
            for(int k = freeItems.size() - 1; k >= 0; k--) {
                int j = freeItems[k];
                if(scope[j]->containsValue(i)) {
                    if(currentFill + sizes[j] > loadMax) {
                        if(solver->delVal(scope[j], i) == false)
                            return false;
                    } else
                        possibleExtent += sizes[j];
                }
            }
            if(currentFill + possibleExtent < loadMin)
                return false;
            if(currentFill + possibleExtent == loadMin) {
                solver->assignToVal(loads[i], loadMin);
                for(int k = freeItems.size() - 1; k >= 0; k--) {
                    int j = freeItems[k];
                    if(scope[j]->containsValue(i))
                        solver->assignToVal(scope[j], i);
                }
            }
        }
    }
    return true;
}

template <typename T>
T &as_lvalue(T &&val) {
    return val;
}

BinPackingLoad::BinPackingLoad(Problem &p, std::string n, vec<Variable *> &vars, vec<int> &_sizes, vec<Variable *> &_loads)
    : BinPacking(p, n, vars.extend(_loads), _sizes, as_lvalue(vec<int>(_loads.size()))) {
    _loads.copyTo(loads);
    vars.shrink(_loads.size());
    assert(vars.size() + _loads.size() == scope.size());   // To be sure
    freeItems.setCapacity(nItems, true);
}
