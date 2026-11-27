// ParticleSys.h: SAGE particle system manager and its intrusive handle.
// Target facts: destroyParticleSystemByID 0x001F5B79,
// findTemplate 0x001F90DA, createParticleSystem 0x001F5A6A and
// findParticleSystemByID 0x001F5B0A. Creation and lookup return a twelve-byte
// handle by value (hidden return pointer first), not ParticleSystem*. The
// handle zeroes its three words in its inline default constructor, unlinks at
// 0x0004CBC0 from an inline destructor that tests +0, and copies through
// assignment 0x0004CC3D. ParticleSystem's ID sits at +0xA8 and setPosition is
// the out-of-line 0x001F3899.
#pragma once
#ifndef BFME2_PARTICLESYS_H
#define BFME2_PARTICLESYS_H

#include "Common/GameType.h"
#include "Common/Coord3D.h"
#include "Common/AsciiString.h"
#include "Common/SubsystemInterface.h"
#include "Common/Snapshot.h"
#include "WWMath/Matrix3D.h"

class Xfer;
class Drawable;
class Object;
class RenderInfoClass;
class ParticleSystemTemplate;
class ParticleSystemManager;
class ParticleSystemHandle;

class ParticleSystem
{
public:
	void setPosition(const Coord3D *pos);
	void attachToDrawable(const Drawable *drawable); // 0x001F3C20
	void attachToObject(const Object *object); // 0x001F3C43
	void stop(); // 0x001F3852
	void destroy(); // 0x001F462C (ledger name; ActiveBody::deleteAllParticleSystems 0x004BE3EF calls it)

	UnsignedByte m_unk0x00[0x9C];
	ParticleSystemHandle *m_firstHandle;					// 0x9C
	ParticleSystemHandle *m_lastHandle;					// 0xA0
	UnsignedByte m_unk0xA4[4];
	ParticleSystemID m_systemID;						// 0xA8
	UnsignedByte m_unk0xAC[4];
	DrawableID m_attachedToDrawableID;					// 0xB0 (attachToDrawable)
	ObjectID m_attachedToObjectID;						// 0xB4 (attachToObject)
	UnsignedByte m_unk0xB8[4];
	Matrix3D m_localTransform;							// 0xBC (setPosition writes its translation column)
	UnsignedByte m_unk0xEC[0xB4];
	Bool m_isLocalIdentity;								// 0x1A0 (setPosition clears it)
	UnsignedByte m_unk0x1A1[2];
	Bool m_isStopped;									// 0x1A3 (stop sets it)
};
// Declared through 0x1A3; the full size is not recovered.

ParticleSystem *Make001FCBD7(); // Default object for an empty particle-system handle.

// The twelve-byte intrusive handle retail returns from the manager.
class ParticleSystemHandle
{
public:
	ParticleSystemHandle() throw() : m_system(0), m_previous(0), m_next(0) {}
	ParticleSystemHandle(const ParticleSystemHandle &that) throw();
	~ParticleSystemHandle() throw() { if (m_system) unlink(); }
	ParticleSystemHandle &operator=(const ParticleSystemHandle &that) throw();

	void unlink() throw();								// 0x0004CBC0

	operator bool() const { return m_system != 0; }
	ParticleSystem *operator->() const { return m_system ? m_system : Make001FCBD7(); }

	ParticleSystem *m_system;							// 0x00
	ParticleSystemHandle *m_previous;					// 0x04
	ParticleSystemHandle *m_next;						// 0x08
};
// SIZE 0xC

class ParticleSystemManager : public SubsystemInterface, public Snapshot
{
public:
	ParticleSystemManager();
	virtual ~ParticleSystemManager();

	virtual void init();
	virtual void reset();
	virtual void update();

	virtual Int vslot14() = 0;
	virtual void vslot15(Int value);
	virtual void render(RenderInfoClass &rinfo) = 0;
	virtual void vslot17(RenderInfoClass &rinfo) = 0;
	virtual void markDirty() = 0;
	virtual void rva001F9F66();

	ParticleSystemTemplate *findTemplate(const AsciiString &name) const;
	ParticleSystemHandle createParticleSystem(const ParticleSystemTemplate *sysTemplate, Bool createSlaves = true);
	ParticleSystemHandle findParticleSystemByID(ParticleSystemID id);
	int rva001F5AA4(ParticleSystemTemplate *, const Object *, int);
	void destroyParticleSystemByID(ParticleSystemID id);

protected:
	virtual void loadPostProcess();
	virtual const char *GetSnapshotName() const;
	virtual void xfer(Xfer *xfer);
};

extern ParticleSystemManager *TheParticleSystemManager;

// Transfers a particle system id through the Int overload (0x0030600A).
void XferParticleSystemID(Xfer *xfer, Int *value);

#endif
