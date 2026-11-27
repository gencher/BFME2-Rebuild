#pragma once

#include <list>

#include "Common/SubsystemInterface.h"
#include "Common/AsciiString.h"
#include "Common/Coord3D.h"

enum CanMakeType;
enum LegalBuildCode;
class Coord3D;
class Object;
class ObjectSellInfo;
class Xfer;
class Player;
class ThingTemplate;

// One entry of the line-build list (0x1C bytes): where a piece goes, its angle and the
// name of the template to build there.
class Rva004D9A3C
{
public:
	Rva004D9A3C()
	{
		m_position.zero();
		m_angle = 0.0f;
		m_templateName.clear();
		m_unk0x18 = -1;
		m_unk0x10 = false;
	}

	Coord3D m_position;			// +0x00
	Real m_angle;				// +0x0C
	Bool m_unk0x10;				// +0x10
	AsciiString m_templateName;	// +0x14 looked up with ThingFactory::findTemplate
	Int m_unk0x18;				// +0x18
};

// The line-build list (0x18 bytes); BuildAssistant keeps one at +0x10. The entry array holds
// GlobalData's MaxLineBuildObjects entries, of which m_count are in use.
class Rva0039205C
{
public:
	Rva0039205C()
	{
		m_unk0x10 = -1;
		m_count = 0;
		m_tiles = NULL;
		m_unk0x0C = false;
		m_unk0x0D = false;
		m_unk0x04 = 0;
		m_unk0x14 = 0;
	}
	~Rva0039205C() { release(); }

	void release()
	{
		if (m_tiles)
		{
			delete[] m_tiles;
			m_tiles = NULL;
			m_count = 0;
		}
	}

	UnsignedInt m_count;		// +0x00
	Int m_unk0x04;				// +0x04
	Rva004D9A3C *m_tiles;		// +0x08
	Bool m_unk0x0C;				// +0x0C
	Bool m_unk0x0D;				// +0x0D
	Int m_unk0x10;				// +0x10
	Int m_unk0x14;				// +0x14
};

// The constructor initializes SubsystemInterface, then the sell list at +0xC.
class BuildAssistant : public SubsystemInterface
{
public:
	BuildAssistant();
	virtual ~BuildAssistant();

	virtual void init();
	virtual void reset();
	virtual void update();

	virtual Object *buildObjectNow(Object *builder, const ThingTemplate *what,
		const Coord3D *position, float angle, Player *owner);
	virtual bool rva00392DC0(Object *, const ThingTemplate *, const Coord3D *,
		Player *, unsigned int, Object **);
	virtual LegalBuildCode isLocationLegalToBuild(const Coord3D *, const ThingTemplate *,
		float, unsigned int, Object *, Player *);
	virtual bool isLocationClearOfObjects(const Coord3D *, const ThingTemplate *,
		float, Object *, unsigned int, Player *, bool);
	virtual bool rva0039361E(const Coord3D *, const ThingTemplate *, float, Object *, Player *);
	virtual void addBibs(const Coord3D *, const ThingTemplate *);
	virtual bool rva0039380F(Rva0039205C *, Object *, const Coord3D *, const Coord3D *, unsigned int);
	virtual bool rva0039321C(Rva0039205C *, Object *, Object *, unsigned int, bool);
	// The native entry returns zero. A caller has not established the return type.
	virtual void vfunc_0x58();
	virtual bool rva00391A70(const ThingTemplate *, const Object *);
	virtual CanMakeType canMakeUnit(Object *, const ThingTemplate *, int) const;
	virtual bool isPossibleToMakeUnit(Object *, const ThingTemplate *, int) const;
	virtual void sellObject(Object *);

	// Checks whether the factory's command set offers this template.
	bool offersBuildCommand(const ThingTemplate *builder, const ThingTemplate *whatToBuild);
	bool isRemovableForConstruction(Object *);
	void clearRemovableForConstruction(const ThingTemplate *whatToBuild, const Coord3D *pos, float angle);
	void xferTheSellList(Xfer *xfer);	// 0x003924FE: the list of objects being sold

private:
	// canMakeUnit passes this record through the object visitor.
	struct ProductionCount
	{
		UnsignedInt count;
		const ThingTemplate *unitType;
	};
	friend int countInProduction(Object *, void *);

	_STL::list<ObjectSellInfo *> m_sellList;
	Rva0039205C m_buildInfo;
};

// GameEngine allocates 0x28 bytes before calling the constructor.
typedef char BuildAssistantSizeCheck[sizeof(BuildAssistant) == 0x28 ? 1 : -1];

extern BuildAssistant *TheBuildAssistant;
