// DRAFT: LoadGameFadeWrapper, from its retail vtable (two slots: deleting destructor, invoke)
// and from the inline constructor and invoke body.
#pragma once
#include <stddef.h>

// A reference-counted wrapper around a plain fade callback.
class LoadGameFadeWrapper
{
public:
	typedef unsigned int (*FadeFunc)(float progress, bool fadeIn);

	LoadGameFadeWrapper(const FadeFunc &func) : m_refCount(0), m_func(func) {}
	virtual ~LoadGameFadeWrapper() {}
	virtual unsigned int invoke(float progress, bool fadeIn);

	int m_refCount; // +0x4
	FadeFunc m_func; // +0x8
};
typedef char LoadGameFadeWrapper_off_m_refCount[(offsetof(LoadGameFadeWrapper, m_refCount) == 0x4) ? 1 : -1];
typedef char LoadGameFadeWrapper_off_m_func[(offsetof(LoadGameFadeWrapper, m_func) == 0x8) ? 1 : -1];
