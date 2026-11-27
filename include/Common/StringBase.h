// StringBase.h: WWLib's reference-counted string template. AsciiString is
// StringBase<char> and UnicodeString is StringBase<unsigned short> (mangled G),
// each behind thin inline wrappers.
// Target facts: one pointer at +0 to a pooled buffer whose eight-byte header
// holds the reference count (+0), length (+4) and capacity (+6); the text
// follows at +8 (clientUpdate and initLaser load the pointer, test it, add 8).
// The constructors, releaseBuffer and the destructor are private, as their
// mangled names say (??0?$StringBase@D@@AAE@ABV0@@Z 0x000365F0,
// ??0?$StringBase@D@@AAE@PBD@Z 0x00037BA0, ?releaseBuffer@?$StringBase@D@@AAEXXZ
// 0x00036410; wide 0x00037050, 0x00037E30, 0x00036E70).
// Retail game.dat .text calls (E8 rel32):
//   copy ctor      char 2164, wide 783   (wrapper copies: char 11, wide 0)
//   text ctor      char 2722, wide 184   (wrapper copies: char 224, wide 0)
//   set(StringBase) char 1211, wide 411  (??4 wrapper copies: 0 and 0)
//   releaseBuffer  char 7027, wide 1806  (~StringBase<G> 0x005B804E is reached
//                  by 2002 jumps, the unwind funclets, and 13 calls)
// so the wrappers' constructors and copy assignment are inline over these, and
// the destructor is inline (it expands to the releaseBuffer call).
// str() returns a pooled literal for an empty string (see str()); the
// function-local TheNullChr this header used before has no reference in retail.
#pragma once
#ifndef BFME2_STRINGBASE_H
#define BFME2_STRINGBASE_H

template <class CharT> class CharSource;

template <class CharT> class StringBase
{
	friend class AsciiString;
	friend class UnicodeString;
public:
	const CharT *str() const
	{
		// The empty result is the pooled "" literal: retail's inline copies load
		// one read-only byte (char: .rdata RVA 0x007BAC1C, ~1000 referencing
		// functions, e.g. getNameAndIndex 0x000AA73A).
		return m_data ? peek() : emptyText((const CharT *)0);
	}
	static const char *emptyText(const char *) { return ""; }
	static const unsigned short *emptyText(const unsigned short *) { return L""; }
	int getLength() const { return m_data ? m_data->length : 0; }
	CharT *getBufferForRead(int length);				// char 0x00036640 (the chunk reader fills the text in place)
	// Out of line in retail (char 0x00001E2F), but defined here: FlammableUpdate
	// 0x0048CB18 tests a bone name as `mov eax,ptr; test eax,eax;
	// cmp word ptr [eax+4],0`, and the emotion unit keeps the out-of-line call
	// because its temporary has no EH state across it, which the non-throwing
	// declaration reproduces.
	bool isEmpty() const throw() { return m_data == 0 || m_data->length == 0; }
	bool isNotEmpty() const throw() { return !isEmpty(); }

	void set(const StringBase<CharT> &src);				// char 0x000366F0, wide 0x00037150
	void set(const CharT *s);							// char 0x000055F5, wide 0x0000565D
	void concat(const StringBase<CharT> &src);			// char 0x00006987, wide 0x00006A2A
	void concat(const CharT *s);						// char 0x00005629, wide 0x00005692
	void concat(const CharT *s, int len);				// char 0x000369A0, wide 0x00037410
	// Inline: its copies (char 0x0000564E, wide 0x000056B8) are never called.
	// Ordinary inlining of both character overloads loses at least two rows.
	// evidence: expanded at DisplayString::appendChar 0x00358957 and RAMFile::scanInt 0x0060580C; out-of-line copies 0x0000564E/0x000056B8 exist but are never called; the inlined copy passes the parameter's own address (GameInfo::setMap 0x00400126, NAT 0x005A74D8)
	__forceinline void concat(CharT c) { concat(&c, 1); }
	// DisplayString::reset 0x00358946 calls releaseBuffer<G> 0x00036E70 directly on
	// its string member: clear is the inline forwarder.
	void clear() { releaseBuffer(); }
	void removeLastChar();								// wide 0x000376E0 (12 calls)
	int compare(const StringBase<CharT> &s) const throw();	// char 0x000069D6; HeaderTemplateManager::findHeaderTemplate 0x00201A33 holds a by-value string across the call with no EH state change, so the call cannot throw
	int compare(const CharT *s) const throw();					// char 0x000069B1; layout parser31732D
	int compareNoCase(const CharT *s) const throw();		// char 0x00037980 (ScriptConditions::evaluateGameModeActive 0x003E92B2); throw():
														// OptionPreferences::getFirewallNeedToRefresh 0x002E4928 has no EH frame while a copy
														// is live across it, and 0x00059646 calls it twice on a live copy with no state store
	int compareNoCase(const StringBase<CharT> &s) const throw();	// char 0x00006A00 (throw(): Shell::findScreenByFilename 0x0035C6B4 has no EH frame) (ControlBarSchemeManager::findControlBarScheme calls it)
	void toLower();										// char 0x00036A70 (findControlBarScheme, newControlBarScheme)

	// The rest of the template's out-of-line surface: every StringBase<D>/<G>
	// COMDAT the map names, char RVA then wide RVA. Declared
	// only, as the members above: callers reach these copies with a call.
	// The CharSource overloads of set/concat and the private buffer helpers
	// are not declared.
	// Inline like concat(CharT): its copies (char 0x0000561A, wide 0x00005683)
	// are never called (AsciiString::operator=(char) 0x000065CA expands it).
	// evidence: expanded by AsciiString::operator=(char) 0x000065CA; out-of-line copies 0x0000561A/0x00005683 exist but are never called
	// Ordinary inlining of both character overloads loses at least two rows.
	__forceinline void set(CharT c) { CharT text = c; set(&text, 1); }
	void set(const CharT *s, int len);					// char 0x00036780, wide 0x000371E0
	void set(const CharT *s, int first, int count);		// char 0x00037C10, wide 0x00037E90 (argument roles not established)
	void set(const StringBase<CharT> &src, int first, int count);	// char 0x00037C70, wide 0x00037EF0 (argument roles not established)
	int compare(const CharT *s, int len) const;			// char 0x00006733, wide 0x000067B1
	int compare(CharT c) const;							// char 0x00035F60, wide 0x00036150
	int compareNoCase(const CharT *s, int len) const;	// char 0x00006772, wide 0x000067F0
	int compareNoCase(CharT c) const;					// char 0x000379E0, wide 0x00037AF0
	bool startsWith(const CharT *s) const;				// char 0x0002BE9C, wide 0x00036190
	bool startsWith(const CharT *s, int len) const;		// char 0x00035970, wide 0x00035B30
	bool startsWith(const StringBase<CharT> &s) const;	// char 0x00035FB0, wide 0x000361F0
	bool startsWithNoCase(const CharT *s) const;		// char 0x00036000, wide 0x00036250
	bool startsWithNoCase(const CharT *s, int len) const;	// char 0x000359B0, wide 0x00035B80
	bool startsWithNoCase(const StringBase<CharT> &s) const;	// char 0x0002C42F, wide 0x000362B0
	bool endsWith(const CharT *s) const;				// char 0x0002BEC1, wide 0x00036310
	bool endsWith(const CharT *s, int len) const;		// char 0x00035A00, wide 0x00035BD0
	bool endsWith(const StringBase<CharT> &s) const;	// char 0x00036060, wide 0x00036350
	bool endsWithNoCase(const CharT *s) const;			// char 0x000360D0, wide 0x00036390
	bool endsWithNoCase(const CharT *s, int len) const;	// char 0x00035A50, wide 0x00035C30
	bool endsWithNoCase(const StringBase<CharT> &s) const;	// char 0x00036110, wide 0x000363D0
	const CharT *find(CharT c) const;					// char 0x0002C459, wide 0x00035AB0
	const CharT *reverseFind(CharT c) const;			// char 0x00035930, wide 0x00035AF0
	// Inline (Zero Hour): NetPacket 0x005913CB/0x005914D4 test m_data at each character.
	// The out-of-line copies char 0x00035720, wide 0x00035760 remain for other callers.
	CharT getCharAt(int index) const { return m_data ? peek()[index] : 0; }
	bool isNone() const;								// char 0x00037DE0, wide 0x00038040
	bool isNotNone() const;								// char 0x00038050, wide 0x00038070
	void swap(StringBase<CharT> &other);				// char 0x00035910 (ICF-folded)
	void toUpper();										// char 0x00036B60, wide 0x000375E0
	void trim();										// char 0x00037CF0, wide 0x00037F70
	bool nextToken(StringBase<CharT> *token, const CharT *delimiters);	// char 0x00036D90, wide 0x00037830
	void format_va(const CharT *fmt, char *args);		// char 0x00036D40, wide 0x000377E0
	void format_va(const StringBase<CharT> &fmt, char *args);	// char 0x00037920, wide 0x00037A30
	void __cdecl format(const CharT *fmt, ...);			// char 0x00037D90, wide 0x00037FF0

private:
	struct Header
	{
		int refCount;									// 0x00
		unsigned short length;							// 0x04
		unsigned short capacity;						// 0x06
	};

	StringBase() : m_data(0) {}
	StringBase(const CharT *s);							// char 0x00037BA0, wide 0x00037E30
	StringBase(const StringBase<CharT> &src);			// char 0x000365F0, wide 0x00037050
	StringBase(const StringBase<CharT> &src, int first, int count);	// wide 0x00038120: empty, then set(src, first, count); the char twin (0x00037CA0) is not established
	~StringBase() { releaseBuffer(); }					// out-of-line copy: wide 0x005B804E

	CharT *peek() const { return (CharT *)(m_data + 1); }
	void releaseBuffer();								// char 0x00036410, wide 0x00036E70
	// char 0x000364A0, wide 0x00036F00 (WorldBuilder assert "StringBase<char>::ensureUniqueBufferOfSize"): makes the buffer
	// unshared with room for the count, keeping the old text when asked, then copies and appends the sources.
	void ensureUniqueBufferOfSize(int numCharsNeeded, bool preserveData, const CharSource<CharT> *strToCopy, const CharSource<CharT> *strToCat);

	Header *m_data;										// 0x00
};
// SIZE 0x4

#endif
