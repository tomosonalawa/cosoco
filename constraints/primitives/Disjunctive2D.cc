#include <algorithm>

#include "Disjunctive.h"
#include "solver/Solver.h"
using namespace Cosoco;

//----------------------------------------------
// Check validity and correct definition
//----------------------------------------------

bool Disjunctive2D::isSatisfiedBy(vec<int> &t) {
    return t[0] + w1 <= t[1] || t[1] + w2 <= t[0] || t[2] + h1 <= t[3] || t[3] + h2 <= t[2];
}

//----------------------------------------------
// Filtering
//----------------------------------------------


bool Disjunctive2D::filter(Variable *xx) {
    int minx1 = x1->minimum() + w1, minx2 = x2->minimum() + w2;
    int miny1 = y1->minimum() + h1, miny2 = y2->minimum() + h2;
    bool bx1 = minx1 <= x2->maximum(), bx2 = minx2 <= x1->maximum();
    bool by1 = miny1 <= y2->maximum(), by2 = miny2 <= y1->maximum();
    bool bx = bx1 || bx2, by = by1 || by2;
    if (bx && by)
        return true;
    if (!bx && !by)
        return false;
    if (bx) {
        if(solver->delValuesInRange(x1, x2->maximum() -w1 + 1, minx2) == false)
            return false;
        return solver->delValuesInRange(x2, x1->maximum() - w2 + 1, minx1);
    }
    if(solver->delValuesInRange(y1, y2->maximum() - h1 + 1, miny2) == false)
        return false;
    return solver->delValuesInRange(y2, y1->maximum() - h2 + 1, miny1);
}

//----------------------------------------------
// Construction and initialisation
//----------------------------------------------

Disjunctive2D::Disjunctive2D(Problem &p, std::string n, Variable *xx1, Variable *xx2, Variable *yy1, Variable *yy2, int ww1,
                             int ww2, int hh1, int hh2)
    : GlobalConstraint(p, n, "Disjunctive2D", 4) {
    vec<Variable *> vars;
    vars.push(xx1);
    vars.push(xx2);
    vars.push(yy1);
    vars.push(yy2);
    scopeInitialisation(vars);
    x1 = xx1;
    x2 = xx2;
    y1 = yy1;
    y2 = yy2;
    w1 = ww1;
    w2 = ww2;
    h1 = hh1;
    h2 = hh2;
}
