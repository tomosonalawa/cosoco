//
// Created by audemard on 23/01/23.
//

#include "CumulativeVariablesHW.h"

#include "Cumulative.h"
using namespace Cosoco;


//----------------------------------------------
// Check validity and correct definition
//----------------------------------------------


bool CumulativeVariablesHW::isSatisfiedBy(vec<int> &tuple) {
    vec<int> st;
    int      i, j = 0;
    for(i = 0; i < starts.size(); i++) st.push(tuple[i]);

    for(; i < starts.size() * 2; i++) wwidths[j++] = tuple[i];
    j = 0;
    for(; i < tuple.size(); i++) wheights[j++] = tuple[i];

    return Cumulative::isSatisfiedBy(st);
}


//----------------------------------------------------------
// Filtering
//----------------------------------------------------------

bool CumulativeVariablesHW::filter(Variable *dummy) {
    for(int i = 0; i < starts.size(); i++) {
        wwidths[i]  = widthVariables[i]->minimum();
        wheights[i] = heightVariables[i]->minimum();
    }

    if(Cumulative::filter(dummy) == false)
        return false;

    filterWidthVariables(widthVariables);
    filterHeightVariables(heightVariables);

    return true;
}

int CumulativeVariablesHW::maxWidth(int posx) { return widthVariables[posx]->maximum(); }

//----------------------------------------------
// Construction and initialisation
//----------------------------------------------


CumulativeVariablesHW::CumulativeVariablesHW(Problem &p, std::string n, vec<Variable *> &vars, vec<Variable *> &w,
                                             vec<Variable *> &h, int limit)
    : Cumulative(p, n, vars, Constraint::createScopeVec(&vars, &w, &h), wwidths, wheights, limit) {
    h.copyTo(heightVariables);
    w.copyTo(widthVariables);
    wwidths.growTo(w.size(), 0);
    wheights.growTo(h.size(), 0);
}