#ifndef _RULE_H
#define _RULE_H

#include "ast.h"


class Rule 
{
public:
	Rule() = default;
	Rule(const Rule&) = delete;
	virtual ~Rule();

	virtual Ast *rewrite(Ast *node);
	virtual bool match(const Ast *node) const;
};

class ScalarVecMultRule: public Rule
{
	ScalarVecMultRule() = default;
	ScalarVecMultRule(const ScalarVecMultRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class MultZeroRule : public Rule
{

	MultZeroRule() = default;
	MultZeroRule(const MultZeroRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class ZeroMultRule : public Rule
{
	ZeroMultRule() = default;
	ZeroMultRule(const ZeroMultRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class XPlusXRule : public Rule
{
	XPlusXRule() = default;
	XPlusXRule(const XPlusXRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class MultByTwoRule : public Rule
{
	MultByTwoRule() = default;
	MultByTwoRule(const MultByTwoRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class CombineLeftShiftRule : public Rule
{
	CombineLeftShiftRule() = default;
	CombineLeftShiftRule(const CombineLeftShiftRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};


class AstRewriter
{
public:
	enum VISIT_ORDER { TOPDOWN, BOTTOMUP };

	AstRewriter() = default;
	~AstRewriter();

	void add_rule(Rule *rule, VISIT_ORDER order);
	Ast* rewrite_ex(Ast *node);
	Ast* rewrite(Ast *node);
	//Ast* rewrite(AssignNode *node);
	//Ast* rewrite(PrintNode *node);
	//Ast* rewrite(StatListNode *node);
	//Ast* rewrite(VarNode *node);
	//Ast* rewrite(AddNode *node);
	//Ast* rewrite(DotProductNode *node);
	//Ast* rewrite(IntNode *node);
	//Ast* rewrite(MultNode *node);
	//Ast* rewrite(VecNode *node);
	//Ast* rewrite(LeftShiftNode *node);

private:
	std::vector<Rule*> topdown_rules;
	std::vector<Rule*> bottomup_rules;
};

#endif // !_RULE_H
