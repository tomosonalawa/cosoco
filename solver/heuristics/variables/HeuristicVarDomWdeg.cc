#include "HeuristicVarDomWdeg.h"

using namespace Cosoco;


HeuristicVarDomWdeg::HeuristicVarDomWdeg(Solver &s) : HeuristicVar(s) {
    s.addObserverConflict(this);
    s.addObserverDeleteDecision(this);
    s.addObserverNewDecision(this);
    mode = V2021;
}


Variable *HeuristicVarDomWdeg::select() {
    if(solver.warmStart && solver.conflicts == 0 && solver.nbSolutions == 0) {
        for(int i = 0; i < solver.decisionVariables.size(); i++)
            if(solver.decisionVariables[i]->_name.rfind("__av", 0) != 0)
                return solver.decisionVariables[i];
    }

    if(solver.warmStart == false && solver.statistics[restarts] < 1 && solver.nbSolutions == 0) {
        Constraint *c = solver.problem.constraints.last();
        for(Variable *x : c->scope)
            if(x->size() > 1)
                return x;
    }


    Variable *x = solver.decisionVariables[0];
    if(mode == V2004 || mode == ABSCON) {
        double bestV = ((double)x->size()) / x->wdeg;

        for(int i = 1; i < solver.decisionVariables.size(); i++) {
            Variable *y = solver.decisionVariables[i];
            if(((double)y->size()) / y->wdeg < bestV) {
                bestV = ((double)y->size()) / y->wdeg;
                x     = y;
            }
        }
        assert(x != nullptr);
        return x;
    }
    // MODE NEWWDEG

    if(mode == NEWWDEG) {
        double bestV = ((double)x->wdeg);

        for(int i = 1; i < solver.decisionVariables.size(); i++) {
            Variable *y = solver.decisionVariables[i];
            if(y->wdeg > bestV) {
                bestV = y->wdeg;
                x     = y;
            }
        }
        return x;
    }


    double bestV = ((double)x->wdeg);

    for(int i = 1; i < solver.decisionVariables.size(); i++) {
        Variable *y = solver.decisionVariables[i];
        if(y->wdeg > bestV || (y->wdeg == bestV && y->size() < x->size())) {
            bestV = y->wdeg;
            x     = y;
        }
    }

    assert(x != nullptr);
    return x;
}


void HeuristicVarDomWdeg::notifyConflict(Constraint *c, int level) {
    int notThisposition = NOTINSCOPE;
    if(c->unassignedVariablesIdx.size() == 1)
        notThisposition = c->unassignedVariablesIdx[0];   // c->toScopePosition(c->unassignedVariablesIdx[0]);


    if(mode == V2004) {
        for(int i = 0; i < c->scope.size(); i++) {
            if(i == notThisposition)
                continue;
            c->wdeg[i]++;
            c->scope[i]->wdeg++;
        }
    } else {
        for(int i = 0; i < c->unassignedVariablesIdx.size(); i++) {
            int posx = c->unassignedVariablesIdx[i];
            if(mode == ABSCON) {
                c->wdeg[posx]++;
                c->scope[posx]->wdeg++;
            } else {   // NEWWDEG
                c->wdeg[posx] +=
                    1 / (c->unassignedVariablesIdx.size() * (c->scope[posx]->size() == 0 ? 0.5 : c->scope[posx]->size()));
                c->scope[posx]->wdeg +=
                    1 / (c->unassignedVariablesIdx.size() * (c->scope[posx]->size() == 0 ? 0.5 : c->scope[posx]->size()));
            }
        }
    }
}


void HeuristicVarDomWdeg::notifyNewDecision(Variable *x, Solver &s) {
    for(Constraint *c : x->constraints) {
        if(c->unassignedVariablesIdx.size() == 1)
            c->scope[c->unassignedVariablesIdx[0]]->wdeg -= c->wdeg[c->unassignedVariablesIdx[0]];
    }
}


void HeuristicVarDomWdeg::notifyDeleteDecision(Variable *x, int v, Solver &s) {
    for(Constraint *c : x->constraints) {
        if(c->unassignedVariablesIdx.size() == 2)
            c->scope[c->unassignedVariablesIdx[0]]->wdeg += c->wdeg[c->unassignedVariablesIdx[0]];
    }
}


void HeuristicVarDomWdeg::notifyFullBacktrack() {
    /*    printf("erer\n");
        Variable *y;
        double    wd = 0;
        for(Variable *x : solver.problem.variables) {
            if(x->wdeg > wd) {
                wd = x->wdeg;
                y  = x;
            }
        }
        std::cout << y->_name << std::endl;
        for(Constraint *c : y->constraints) {
            double v = 0;
            for(double tmp : c->wdeg) v += tmp;
            std::cout << c->type << " " << v << std::endl;
        }
    */
    if(solver.statistics[GlobalStats::restarts] > 0 &&
       ((solver.statistics[GlobalStats::restarts] + 1) - solver.lastSolutionRun) % 30 == 0) {
        printf("erer\n");
        for(Constraint *c : solver.problem.constraints) c->wdeg.fill(0);
        for(Variable *x : solver.problem.variables) x->wdeg = 0;
    }
}
