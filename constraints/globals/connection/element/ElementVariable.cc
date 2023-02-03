#include "ElementVariable.h"

#include "solver/Solver.h"

using namespace Cosoco;


//----------------------------------------------
// Check validity and correct definition
//----------------------------------------------


bool ElementVariable::isSatisfiedBy(vec<int> &tuple) {
    int idx = posIndex == -1 ? tuple[tuple.size() - 2] : posIndex;
    int res = tuple[tuple.size() - 1];
    return true;
    return tuple[idx - (startAtOne ? 1 : 0)] == res;
}


bool ElementVariable::isCorrectlyDefined() {
    if(index == value)
        throw std::logic_error("Constraint " + std::to_string(idc) + ": ElementVariable has index=result");
    return Element::isCorrectlyDefined();
}

//----------------------------------------------
// Filtering
//----------------------------------------------


bool ElementVariable::filter(Variable *dummy) {
    return true;
    if(index->size() > 1) {
        // updating vdom (and valueSentinels)
        if(filterValue() == false)
            return false;
        while(true) {
            // updating idom (and indexSentinels)
            int sizeBefore = index->size();
            if(filterIndex() == false)
                return false;
            if(sizeBefore == index->size())
                break;
            // updating vdom (and valueSentinels)
            sizeBefore = value->size();
            if(filterValue() == false)
                return false;
            if(sizeBefore == value->size())
                break;
        }
    }
    // If index is singleton, we update dom(list[index]) and vdom so that they are both equal to the
    // intersection of the two domains
    if(index->size() == 1) {
        if(solver->delValuesNotInDomain(list[index->value()], value->domain) == false)
            return false;
        if(value->size() == 1)
            solver->entail(this);
    }
    return true;
}

bool ElementVariable::validIndex(int idv) {
    int v = indexSentinels[idv];
    if(v != -1 && list[idv]->containsValue(v) && value->containsValue(v))
        return true;
    for(int idv2 : list[idv]->domain) {   // int a = dom.first(); a != -1; a = dom.next(a)) {
        v = list[idv]->domain.toVal(idv2);
        if(value->containsValue(v)) {
            indexSentinels[idv] = v;
            return true;
        }
    }
    return false;
}

bool ElementVariable::filterIndex() {
    for(int idv : index->domain)
        if(validIndex(idv) == false && solver->delIdv(index, idv) == false)
            return false;
    return true;
    // return idom.removeIndexesChecking(i -> !validIndex(i));
}

bool ElementVariable::validValue(int idv) {
    int v        = value->domain.toVal(idv);
    int sentinel = valueSentinels[idv];
    if(sentinel != -1 && index->containsIdv(sentinel) && list[sentinel]->containsValue(v))
        return true;
    for(int idv2 : index->domain) {
        if(list[idv2]->containsValue(v)) {
            valueSentinels[idv] = idv2;
            return true;
        }
    }
    return false;
}

bool ElementVariable::filterValue() {
    for(int idv : value->domain)
        if(validValue(idv) == false && solver->delIdv(value, idv) == false)
            return false;
    return true;
}


//----------------------------------------------
// Constructor and initialisation methods
//----------------------------------------------

ElementVariable::ElementVariable(Problem &p, std::string n, vec<Variable *> &vars, Variable *i, Variable *r, bool one)
    : Element(p, n, "Element Variable", Constraint::createScopeVec(&vars, i, r), i, one), value(r) {
    szVector = vars.size();
    posIndex = vars.firstOccurrenceOf(i);
    vars.copyTo(list);
    valueSentinels.growTo(value->domain.maxSize(), -1);
    indexSentinels.growTo(vars.size(), -1);
}
