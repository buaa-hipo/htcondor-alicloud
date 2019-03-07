/***************************************************************
 *
 * Copyright (C) 1990-2007, Condor Team, Computer Sciences Department,
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

#ifndef __AD_PRINT_MASK__
#define __AD_PRINT_MASK__

#include "condor_common.h"
#include "list.h"
#include "condor_classad.h"
#include "condor_attributes.h"
#include "pool_allocator.h"
#include "tokener.h"

// currently no-one uses the MyRowOfData version of the print mask
//#define ALLOW_ROD_PRINTMASK

enum {
	FormatOptionNoPrefix = 0x01,
	FormatOptionNoSuffix = 0x02,
	FormatOptionNoTruncate = 0x04,
	FormatOptionAutoWidth = 0x08,
	FormatOptionLeftAlign = 0x10,
	FormatOptionAlwaysCall = 0x80,
	FormatOptionHideMe    = 0x100,
	FormatOptionFitToData = 0x200,	  // for use by the adjust_formats callback

	FormatOptionSpecialBase = 0x1000, // for use by the adjust_formats callback
	FormatOptionSpecialMask = 0xF000, // for use by the adjust_formats callback

	AltQuestion = 0x10000,     // alt text is ?
	AltStar     = 0x20000,     // alt text is *
	AltDot      = 0x30000,     // alt text is .
	AltDash     = 0x40000,     // alt text is -
	AltUnder    = 0x50000,     // alt text is _
	AltPound    = 0x60000,     // alt text is #
	AltZero     = 0x70000,     // alt text is 0
	AltMask     = 0x70000,     // mask to extract alt text type
	AltWide     = 0x80000,     // alt text is the width of the field with [] around it
};

typedef const char *(*IntCustomFormat)(long long, struct Formatter &);
typedef const char *(*FloatCustomFormat)(double, struct Formatter &);
typedef const char *(*StringCustomFormat)(const char*, struct Formatter &);
//typedef const char *(*AlwaysCustomFormat)(AttrList*,struct Formatter &);
typedef const char *(*ValueCustomFormat)(const classad::Value & value, struct Formatter &);
typedef bool (*StringCustomRender)(std::string & str, AttrList*, struct Formatter &);
typedef bool (*IntCustomRender)(long long & lval, AttrList*, struct Formatter &);
typedef bool (*FloatCustomRender)(double & dval, AttrList*, struct Formatter &);
typedef bool (*ValueCustomRender)(classad::Value & value, AttrList*, struct Formatter &);

class CustomFormatFn {
public:
	enum FormatKind { PRINTF_FMT=0, INT_CUSTOM_FMT, FLT_CUSTOM_FMT, STR_CUSTOM_FMT, VALUE_CUSTOM_FMT, INT_CUSTOM_RENDER, FLT_CUSTOM_RENDER, STR_CUSTOM_RENDER, VALUE_CUSTOM_RENDER };
	operator StringCustomFormat() const { return reinterpret_cast<StringCustomFormat>(pfn); }
	char Kind() const { return (char)fn_type; }
	bool IsString() const { return fn_type == STR_CUSTOM_FMT; }
	bool IsNumber() const { return fn_type >= INT_CUSTOM_FMT && fn_type <= FLT_CUSTOM_FMT; }
	bool Is(IntCustomFormat pf) const { return (void*)pf == pfn; } // This hack is for the condor_status code...
	bool Is(IntCustomRender pf) const { return (void*)pf == pfn; } // This hack is for the condor_status code...
	CustomFormatFn() : pfn(NULL), fn_type(PRINTF_FMT) {};
	CustomFormatFn(StringCustomFormat pf) : pfn((void*)pf), fn_type(STR_CUSTOM_FMT) {};
	CustomFormatFn(IntCustomFormat pf) : pfn((void*)pf), fn_type(INT_CUSTOM_FMT) {};
	CustomFormatFn(FloatCustomFormat pf) : pfn((void*)pf), fn_type(FLT_CUSTOM_FMT) {};
	CustomFormatFn(ValueCustomFormat pf) : pfn((void*)pf), fn_type(VALUE_CUSTOM_FMT) {};
	CustomFormatFn(StringCustomRender pf) : pfn((void*)pf), fn_type(STR_CUSTOM_RENDER) {};
	CustomFormatFn(IntCustomRender pf) : pfn((void*)pf), fn_type(INT_CUSTOM_RENDER) {};
	CustomFormatFn(FloatCustomRender pf) : pfn((void*)pf), fn_type(FLT_CUSTOM_RENDER) {};
	CustomFormatFn(ValueCustomRender pf) : pfn((void*)pf), fn_type(VALUE_CUSTOM_RENDER) {};
private:
	void * pfn;
	FormatKind fn_type;
};

// This struct holds information on how to format (and possibly generate)
// a given column of text.  FmtKind determines how it will be interpreted.
//
// PRINTF_FMT - then the printfFmt string is used to format the attribute.
// *_CUSTOM_FMT - the appropriate function is called which returns a string.
// then if printfFmt is non NULL, it is used to further format the string.
// if printfFmt is NULL and width is non-zero, the string is constrained
// to fit the requested width.  If options has the FormatOptionNoTruncate
// flag set, then width is treated as a minimum, but not as a maximum value.
//
struct Formatter
{
	int        width;		 // 0 for 'width from printf'
	int        options;      // one or more of Formatter_Opt_XXX options
	char       fmt_letter;   // actual letter in the % escape
	char       fmt_type;     // one of the the printf_fmt_t enum values.
	char       fmtKind;      // identifies type type of the union
	char       altKind;      // identifies type of alt text to print when attribute cannot be fetched
	const char * printfFmt;    // may be NULL if fmtKind != PRINTF_FMT
	union {
		StringCustomFormat	sf;
		IntCustomFormat 	df;
		FloatCustomFormat 	ff;
//		AlwaysCustomFormat	af;
		ValueCustomFormat	vf;
		StringCustomRender  sr;
		IntCustomRender     dr;
		FloatCustomRender   fr;
		ValueCustomRender   vr;
	};
};

typedef struct {
	const char * key;           // keyword, table should be sorted by this.
	const char * default_attr;  // default attribute to fetch
	const char * printfFmt;     // optional % printf formatting after custom function is called
	CustomFormatFn cust;        // custom format callback function
	const char * extra_attribs; // other attributes that the custom format needs
} CustomFormatFnTableItem;
typedef case_sensitive_sorted_tokener_lookup_table<CustomFormatFnTableItem> CustomFormatFnTable;

#ifdef ALLOW_ROD_PRINTMASK
class MyRowOfData; // forward ref
#endif
class MyRowOfValues; // forward ref

class AttrListPrintMask
{
  public:
	// ctor/dtor
	AttrListPrintMask ();
	//AttrListPrintMask (const AttrListPrintMask &);
	~AttrListPrintMask ();

	void SetAutoSep(const char* rpre, const char * cpre, const char * cpost, const char * rpost);
	void SetOverallWidth(int wid);

	// register a format and an attribute
	void registerFormat (const char *print, int wid, int opts, const char *attr);
	void registerFormat (const char *print, int wid, int opts, const CustomFormatFn & fmt, const char *attr);
	void registerFormatF (const char *print, const char *attr, int opts=FormatOptionNoTruncate) { registerFormat(print, 0, opts, attr); }
	void registerformat (const CustomFormatFn & fmt, const char *attr) { registerFormat(NULL, 0, 0, fmt, attr); }

	// clear all formats
	void clearFormats (void);
	bool IsEmpty(void) { return formats.IsEmpty(); }
	int  ColCount(void) { return formats.Length(); }

	// for debugging, dump the current config
	void dump(std::string & out, const CustomFormatFnTable * pFnTable, List<const char> * pheadings=NULL);
	// for debugging, walk the current config
	int walk(int (*pfn)(void*pv, int index, Formatter * fmt, const char * attr, const char * head), void* pv, const List<const char> * pheadings=NULL) const;

	// display functions
	int   display (FILE *, AttrList *, AttrList *target=NULL);		// output to FILE *
	int   display (FILE *, AttrListList *, AttrList *target=NULL, List<const char> * pheadings=NULL); // output a list -> FILE *
	int   display (std::string & out, AttrList *, AttrList *target=NULL ); // append to string out. return number of chars added
#ifdef ALLOW_ROD_PRINTMASK
	int   render (MyRowOfData & row, AttrList *, AttrList *target=NULL ); // render columns to text and add to MyRowOfData, returns number of cols
	int   display (std::string & out, MyRowOfData & row); // append to string out. return number of chars added
#endif
	int   render (MyRowOfValues & row, AttrList *, AttrList *target=NULL ); // render columns to text and add to MyRowOfValues, returns number of cols
	int   display (std::string & out, MyRowOfValues & row); // append to string out. return number of chars added
	int   calc_widths(AttrList *, AttrList *target=NULL );          // set column widths
	int   calc_widths(AttrListList *, AttrList *target=NULL);
	int   display_Headings(FILE *, List<const char> & headings);
	char *display_Headings(const char * pszzHead);
	char *display_Headings(List<const char> & headings);
	int   display_Headings(FILE * file) { return display_Headings(file, headings); }
	char *display_Headings(void) { return display_Headings(headings); }
	void set_heading(const char * heading);
	bool has_headings() { return headings.Length() > 0; }
	void clear_headings() { headings.Clear(); }
	const char * store(const char * psz) { return stringpool.insert(psz); } // store a string in the local string pool.
	// iterate formatter and attribs, calling pfn and allowing fmt to be changed until pfn returns < 0
	int adjust_formats(int (*pfn)(void*pv, int index, Formatter * fmt, const char * attr), void* pv);

  private:
	mutable List<Formatter> formats;
	mutable List<char> 		attributes;
	mutable List<const char> headings;

	void clearList (List<Formatter> &);
	void clearList (List<char> &);
	void copyList  (List<Formatter> &, List<Formatter> &);
	void copyList  (List<char> &, List<char> &);

	int overall_max_width;
	const char *    row_prefix;
	const char *    col_prefix;
	const char *    col_suffix;
	const char *    row_suffix;
	void clearPrefixes();
	ALLOCATION_POOL stringpool;

	void PrintCol(MyString * pretval, Formatter & fmt, const char * value);
	void commonRegisterFormat (int wid, int opts, const char *print, const CustomFormatFn & sf,
							const char *attr
							);
};

#ifdef ALLOW_ROD_PRINTMASK

class MyRowOfData
{
public:
	MyRowOfData() : pdata(NULL), cols(0), cmax(0), flat(false) {};
	~MyRowOfData();
	int cat(const char * s);
	int SetMaxCols(int max_cols);

	// Copies a null-terminated character string into the object
	//MyRowOfData& operator=(const char *s);
	// appends a null-terminated string
	//MyRowOfData& operator+=(const char *s);
	// appends a MyString
	MyRowOfData& operator+=(const MyString &S) { cat(S.c_str()); return *this; }

	bool formatstr_cat(const char *format, ...) CHECK_PRINTF_FORMAT(2,3);

	//int Length();
	int ColCount() { return cols; }
	int ColWidth(int index=-1) {
		if (index < 0) index = cols+index;
		if (index >= 0 && index < cols) return strlen(pdata[index]);
		return -1;
	}
	const char * Column(int index) {
		if (index < 0) index = cols+index;
		if (index >= 0 && index < cols) return pdata[index];
		return NULL;
	}
	const char * SwapColumnData(int index, const char * cnew) {
		const char * cold = NULL;
		if (index < 0) index = cols+index;
		if (index >= 0 && index < cols) {
			cold = pdata[index];
			pdata[index] = const_cast<char*>(cnew);
		}
		return cold;
	}

private:
	char **       pdata; // pointer to data, either an array of pointers, or a pointer to an szz
	int           cols;
	int           cmax;
	bool          flat;
};

#endif

class MyRowOfValues
{
public:
	MyRowOfValues() : pdata(NULL), pvalid(NULL), cols(0), cmax(0) {};
	~MyRowOfValues();
	int SetMaxCols(int max_cols);

	int cat(const classad::Value & s);
	classad::Value * next(int & index);
	MyRowOfValues& operator+=(const classad::Value &S) { cat(S); return *this; }

	bool empty() { return cols > 0; }
	int ColCount() { return cols; }
	classad::Value * Column(int index) {
		if (index < 0) index = cols+index;
		if (index >= 0 && index < cols) return &pdata[index];
		return NULL;
	}
	unsigned char is_valid(int index) {
		if (index < 0) index = cols+index;
		if (index >= 0 && index < cols) return pvalid[index];
		return 0;
	}
	unsigned char set_col_valid(int index, unsigned char states) {
		if (index < 0) index = cols+index;
		if (index >= 0 && index < cols) { 
			unsigned char old = pvalid[index];
			pvalid[index] = states;
			return old;
		}
		return 0;
	}
	void set_valid(bool valid) { if (cols > 0 && cols <= cmax) pvalid[cols-1] = valid; }
	void reset() { cols = 0; }

private:
	classad::Value * pdata; // pointer to data, an array of classad::Values
	unsigned char  * pvalid; // point to array of flags per classad::Value
	int           cols;
	int           cmax;
};

// parse -af: options after the : and all of the included arguments up to the next -
// returns the number of arguments consumed
int parse_autoformat_args (
	int /*argc*/,
	const char* argv[],
	int ixArg,
	const char *popts,
	AttrListPrintMask & print_mask,
	classad::References & attrs, // out: returns attributes refereced by the expressions added to print_mask
	bool diagnostic);

// functions & classes in make_printmask.cpp

// This holds expressions that the user would like to use to group results by.
// They are ordered by precedence so group_by_keys[0] is first group key, etc.
class GroupByKeyInfo {
public:
	GroupByKeyInfo() : decending(false) {}
	GroupByKeyInfo(const char * ex, const char * as, bool dec) : expr(ex), name(as), decending(dec) {}
	std::string expr;
	std::string name;
	bool        decending;
};


// used to return what kind of header and footers have been requested in the
// file parsed by SetAttrListPrintMaskFromFile
enum printmask_headerfooter_t {
	STD_HEADFOOT=0,
	HF_NOTITLE=1,
	HF_NOHEADER=2,
	HF_NOSUMMARY=4,
	HF_CUSTOM=8,
	HF_BARE=15
};

// used to return what kind of printmask aggregation has been requested.
enum printmask_aggregation_t {
	PR_NO_AGGREGATION=0,
	PR_COUNT_UNIQUE,
	PR_FROM_AUTOCLUSTER, // For condor_q, select from autocluster set.
};

// interface for reading text one line at a time, used to abstract reading lines
// for input in SetAttrListPrintMaskFromStream
class SimpleInputStream {
public:
	virtual const char * nextline() = 0;
	virtual int count_of_lines_read() = 0;
	virtual ~SimpleInputStream() {};
protected:
	SimpleInputStream() {};
};

// simple line at a time file (or stdin) reader
class SimpleFileInputStream : public SimpleInputStream {
	FILE * file;
	bool   auto_close_file; // file is owned by this class, close it in destructor
	int    lines_read;
	SimpleFileInputStream() {}; // don't allow default construction
public:
	// Create a simple input stream for reading lines from an open file
	// if auto_close is true, the file will be closed when the stream is destroyed.
	SimpleFileInputStream(FILE* fh, bool auto_close=true) : file(fh), auto_close_file(auto_close), lines_read(0) {}
	virtual ~SimpleFileInputStream() { if (file && auto_close_file) fclose(file); file = NULL; }
	virtual int count_of_lines_read() { return lines_read; }
	virtual const char * nextline();
};

// Simple line at a time string literal reader 
class StringLiteralInputStream : public SimpleInputStream {
	const char * lit; // points to a string literal, so it's not freed by this class
	std::string line; // temp for the current line to return.
	size_t ix_eol;    // end of current line in lit
	int    lines_read;
	StringLiteralInputStream() {}; // don't allow default construction
public:
	StringLiteralInputStream(const char* psz) : lit(psz), ix_eol(0), lines_read(0) {}
	virtual ~StringLiteralInputStream() { }
	virtual int count_of_lines_read() { return lines_read; }
	virtual const char * nextline() {
		if ( ! lit || ! lit[ix_eol]) return NULL;

		// skip over current end of line
		const char* p = &lit[ix_eol];
		if (*p == '\r') ++p;
		if (*p == '\n') {
			++p;
			// If we hit end-of-file after skipping the current end of line, just return NULL.
			if ( ! *p && ix_eol > 0) { ix_eol = p - lit; return NULL; }
		}
		++lines_read;

		// remember this spot as the the start of line,
		// then skip ahead to the end of line or end of file.
		const char * bol = p;
		while (*p != 0 && *p != '\r' && *p != '\n') ++p;
		ix_eol = p - lit;

		size_t cch = p - bol;
		line.assign(bol, cch);
		return line.c_str();
	}
};


// this structure is used to hold custom print format details
// it is used by SetAttrListPrintMaskFromStream and by PrintPrintMask
typedef struct PrintMaskMakeSettings {
	std::string select_from;           // out: adtype name from SELECT
	printmask_headerfooter_t headfoot; // out, header and footer flags set in SELECT or SUMMARY
	printmask_aggregation_t aggregate; // out: aggregation mode in SELECT
	std::string where_expression;      // out: classad expression from WHERE
	classad::References attrs;        // out: ClassAd attributes referenced in mask or group_by outputs
	classad::References scopes;       // out: scopes for ClassAd attributes referenced in mask or group_by outputs (i.e. target or job)
	classad::References sumattrs;     // out: ClassAd attributes referenced in summary mask

	PrintMaskMakeSettings() : headfoot(STD_HEADFOOT), aggregate(PR_NO_AGGREGATION) {}
	void reset() {
		select_from.clear();
		where_expression.clear();
		attrs.clear();
		scopes.clear();
		sumattrs.clear();
		headfoot = STD_HEADFOOT;
		aggregate = PR_NO_AGGREGATION;
	}
} PrintMaskMakeSettings;

// Read a stream a line at a time, and parse it to fill out the print mask,
// header, group_by, where expression, and projection attributes.
// return is 0 on success, non-zero error code on failure.
//
int SetAttrListPrintMaskFromStream (
	SimpleInputStream & stream, // in: fetch lines from this stream until nextline() returns NULL
	const CustomFormatFnTable & FnTable, // in: table of custom output functions for SELECT
	AttrListPrintMask & prmask, // out: columns and headers set in SELECT
	PrintMaskMakeSettings & settings, // in,out: modifed by parsing the stream. BUT NOT CLEARED FIRST! (so the caller can set defaults)
	std::vector<GroupByKeyInfo> & group_by, // out: ordered set of attributes/expressions in GROUP BY
	AttrListPrintMask * summask, // out: columns and headers set in SUMMMARY
	std::string & error_message // out, if return is non-zero, this will be an error message
	);

int PrintPrintMask(std::string & output,
	const CustomFormatFnTable & FnTable,  // in: table of custom output functions for SELECT
	const AttrListPrintMask & mask,       // in: columns and headers set in SELECT
	const List<const char> * pheadings,   // in: headings override
	const PrintMaskMakeSettings & settings, // in: modifed by parsing the stream. BUT NOT CLEARED FIRST! (so the caller can set defaults)
	const std::vector<GroupByKeyInfo> & group_by,
	AttrListPrintMask * summask // out: columns and headers set in SUMMMARY
	); // in: ordered set of attributes/expressions in GROUP BY

#endif // __AD_PRINT_MASK__
