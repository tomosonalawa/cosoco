//
// Created by audemard on 25/01/2021.
//

#ifndef COSOCO_GEUNARY_H
#define COSOCO_GEUNARY_H
#include "constraints/Constraint.h"



namespace Cosoco {

class GEUnary : public Constraint, public ObjectiveConstraint  {
   public:
    int k;
    // Constructors
    GEUnary(Problem &p, std::string n, Variable *x,  int k);

    // filtering
    bool filter(Variable *x) override;

    // checking
    bool isSatisfiedBy(vec<int> &tuple) override;
    void updateBound(long bound) override;            // Update the current bound
    long maxUpperBound() override;                    // Bounds are included
    long minLowerBound() override;                    // Bounds are included
    long computeScore(vec<int> &solution) override;   // Compute the current score of the constraint
};
}   // namespace Cosoco




#endif   // COSOCO_GEUNARY_H
