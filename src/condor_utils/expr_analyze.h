/***************************************************************
 *
 * Copyright (C) 2014-2015, Condor Team, Computer Sciences Department,
 * University of Wisconsin-Madison, WI.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License.  You may
 * obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************/

#ifndef __EXPR_ANALYZE_H__
#define __EXPR_ANALYZE_H__


enum {
	detail_smart_unparse_expr    = 0x01, // show unparsed expressions at analyze step when needed
	detail_always_unparse_expr   = 0x02, // always show unparsed expressions at each analyze step
	detail_show_all_subexprs     = 0x04, // show all sub expressions, with prefixes for pruned subs
	detail_show_ifthenelse       = 0x08, // show sub expressions of ifthenelse functions

	detail_diagnostic            = 0x40, // show steps of expression analysis to stdout
	detail_dump_intermediates    = 0x80, // dump intermediate work buffers to stdout. (verbose diagnostic)

	detail_analyze_each_sub_expr = 0x100, // analyze each sub expression, not just the top level ones.
	detail_always_analyze_req    = 0x200, // always analyze requirements, (better analize does this)
	detail_dont_show_job_attrs   = 0x400, // suppress display of job attributes in verbose analyze
	detail_inline_std_slot_exprs = 0x800, // inline WithinResourceLimits and IsValidCheckpointPlatform in addition to START

	detail_better                = 0x10000, // -better rather than  -analyze was specified.
};

typedef struct {
	int console_width;
	int detail_mask; // one or more of above detail_* flags
	const char * expr_label;
	const char * request_type_name;
	const char * target_type_name;
} anaFormattingOptions;

void AnalyzeRequirementsForEachTarget(
	ClassAd *request,
	const char * attrConstraint, // must be an attribute in the request ad
	classad::References & inline_attrs, // expand these attrs inline
	ClassAdList & targets,
	std::string & return_buf,
	const anaFormattingOptions & fmt);

const char * PrettyPrintExprTree(classad::ExprTree *tree, std::string & temp_buffer, int indent, int width);

class AnalSubExpr;
int AnalyzeThisSubExpr(
	ClassAd *myad,
	classad::ExprTree* expr,
	classad::References & inline_attrs, // expand attrs with these names inline
	std::vector<AnalSubExpr> & clauses,
	bool must_store,
	int depth,
	const anaFormattingOptions & fmt);

void AddReferencedAttribsToBuffer(
	ClassAd * request,
	const char * expr_string, // expression string or attribute name
	classad::References & hidden_refs, // don't print these even if they appear in the trefs list
	StringList & trefs, // out, returns target refs
	bool raw_values, // unparse referenced values if true, print evaluated referenced values if false
	const char * pindent,
	std::string & return_buf);

class QuantizingAccumulator {
public:
	QuantizingAccumulator(int val, int num) : accum(val), quantized(val), allocs(num) {}
	QuantizingAccumulator& operator+=(size_t val) { Add(val); return *this; }
	QuantizingAccumulator& operator+=(int val) { Add(val); return *this; }
	size_t Value(size_t * pquan=NULL, size_t * pcount=NULL) {
		if (pquan) *pquan = quantized;
		if (pcount) *pcount = allocs;
		return accum;
	}
private:
	void Add(size_t val) { 
		accum += val; 
	#ifdef WIN32
		// from expermentation with vs2012 (32 bit), memory allocation does NOT return consecutive allocations
		// but the smallest obserable differnce shows that the allocator adds 8 and then quantizes by 8
		quantized += 8 + ((val + 7) & ~7);
	#else
		// from http://web.eecs.utk.edu/~huangj/cs360/360/notes/Malloc1/lecture.html
		// linux malloc adds 8 then quantizes to 8 bytes
		quantized += 8 + ((val + 7) & ~7);
	#endif
		++allocs;
	}
	size_t accum;
	size_t quantized;
	size_t allocs;
};

void AddTargetAttribsToBuffer(
	StringList & trefs, // in, target refs (probably returned by AddReferencedAttribsToBuffer)
	ClassAd * request,
	ClassAd * target,
	bool raw_values, // unparse referenced values if true, print evaluated referenced values if false
	const char * pindent,
	std::string & return_buf);

// this functions walk the given classad data structure and determines the memory consumed into
// the given QuantizingAccumulator. The effects of classad caching/compression are ignored by these functions.
// the return value is the same as accum.Value().
size_t AddExprTreeMemoryUse (const classad::ExprTree* tree, QuantizingAccumulator & accum, int & num_skipped);
size_t AddClassadMemoryUse (const classad::ClassAd* cad, QuantizingAccumulator & accum, int & num_skipped);
size_t AddClassadMemoryUse (const classad::ExprList* list, QuantizingAccumulator & accum, int & num_skipped);


#endif//__EXPR_ANALYZE_H__
