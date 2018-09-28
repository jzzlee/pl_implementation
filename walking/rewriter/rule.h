#ifndef _RULE_H
#define _RULE_H

#include "ast.h"


class Rule 
{
public:
	enum VISIT_ORDER { TOPDOWN, BOTTOMUP };

	Rule() = delete;
	Rule(VISIT_ORDER order);
	Rule(const Rule&) = delete;
	virtual ~Rule();

	virtual Ast *rewrite(Ast *node);
	virtual bool match(const Ast *node) const;
	const VISIT_ORDER get_order() const;

private:
	VISIT_ORDER order;
};

class ScalarVecMultRule: public Rule
{
public:

	ScalarVecMultRule() = delete;
	ScalarVecMultRule(VISIT_ORDER order);
	ScalarVecMultRule(const ScalarVecMultRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class MultZeroRule : public Rule
{
public:
	MultZeroRule() = delete;
	MultZeroRule(VISIT_ORDER order);
	MultZeroRule(const MultZeroRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class ZeroMultRule : public Rule
{
public:
	ZeroMultRule() = delete;
	ZeroMultRule(VISIT_ORDER order);
	ZeroMultRule(const ZeroMultRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class XPlusXRule : public Rule
{
public:
	XPlusXRule() = delete;
	XPlusXRule(VISIT_ORDER order);
	XPlusXRule(const XPlusXRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class MultByTwoRule : public Rule
{
public:
	MultByTwoRule() = delete;
	MultByTwoRule(VISIT_ORDER order);
	MultByTwoRule(const MultByTwoRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};

class CombineLeftShiftRule : public Rule
{
public:
	CombineLeftShiftRule() = delete;
	CombineLeftShiftRule(VISIT_ORDER order);
	CombineLeftShiftRule(const CombineLeftShiftRule&) = delete;
	Ast *rewrite(Ast *node) override;
	bool match(const Ast *node) const override;
};


class AstRewriter
{
public:

	AstRewriter() = default;
	~AstRewriter();

	void add_rule(Rule *rule);
	void rewrite_ex(Ast *&node);
	void rewrite(Ast *&node);
	void rewrite_elements(Ast *&node);
	void rewrite_elements(AssignNode *&node);
	void rewrite_elements(PrintNode *&node);
	void rewrite_elements(StatListNode *&node);
	void rewrite_elements(VarNode *&node);
	void rewrite_elements(AddNode *&node);
	void rewrite_elements(DotProductNode *&node);
	void rewrite_elements(IntNode *&node);
	void rewrite_elements(MultNode *&node);
	void rewrite_elements(VecNode *&node);
	void rewrite_elements(LeftShiftNode *&node);
	void rewrite_onback(Ast *&node);

private:
	std::vector<Rule*> topdown_rules;
	std::vector<Rule*> bottomup_rules;
};

#endif // !_RULE_H
