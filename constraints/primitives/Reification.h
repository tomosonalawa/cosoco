//
// Created by audemard on 09/04/24.
//

#ifndef COSOCO_REIFICATION_H
#define COSOCO_REIFICATION_H


#include "Binary.h"
#include "constraints/Ternary.h"

namespace Cosoco {
class ReifLE : public Ternary {
   public:
    ReifLE(Problem &p, std::string n, Variable *xx, Variable *yy, Variable *zz);
    bool filter(Variable *x) override;
    bool isSatisfiedBy(vec<int> &tuple) override;
};


class ReifLT : public Ternary {
   public:
    ReifLT(Problem &p, std::string n, Variable *xx, Variable *yy, Variable *zz);
    bool filter(Variable *x) override;
    bool isSatisfiedBy(vec<int> &tuple) override;
};

class ReifEQ : public Ternary {
   public:
    int residue;
    ReifEQ(Problem &p, std::string n, Variable *xx, Variable *yy, Variable *zz);
    bool filter(Variable *x) override;
    bool isSatisfiedBy(vec<int> &tuple) override;
};

class ReifNE : public Ternary {
   public:
    int residue;
    ReifNE(Problem &p, std::string n, Variable *xx, Variable *yy, Variable *zz);
    bool filter(Variable *x) override;
    bool isSatisfiedBy(vec<int> &tuple) override;
};

class XeqYeqK : public Binary {
   public:
    int k;
    XeqYeqK(Problem &p, std::string n, Variable *xx, Variable *yy, int k);
    bool filter(Variable *x) override;
    bool isSatisfiedBy(vec<int> &tuple) override;
};

class XeqKleY : public Binary {
   public:
    int k;
    XeqKleY(Problem &p, std::string n, Variable *xx, Variable *yy, int k);
    bool filter(Variable *x) override;
    bool isSatisfiedBy(vec<int> &tuple) override;
};

class XeqYleK : public Binary {
   public:
    int k;
    XeqYleK(Problem &p, std::string n, Variable *xx, Variable *yy, int k);
    bool filter(Variable *x) override;
    bool isSatisfiedBy(vec<int> &tuple) override;
};
}   // namespace Cosoco


#endif   // COSOCO_REIFICATION_H
