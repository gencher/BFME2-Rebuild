// Native Shadow base: ctor 0x000F0F2B calls initialize 0x000EFA4E,
// installs table RVA 0x007CEFA0 (three slots), then writes +0x58..+0x64.
// W3DVolumetricShadow ctor 0x000F1528 and projected ctor 0x0010BA2C call
// that constructor and begin their own members at +0x68. SIZE 0x68.
#pragma once
#ifndef BFME2_WW3D2_SHADOW_H
#define BFME2_WW3D2_SHADOW_H
#include "Common/GameType.h"
#include "Common/Coord3D.h"
#include "Common/AsciiString.h"

class ShadowBase
{
public:
    virtual ~ShadowBase();
    virtual void slot01();
    virtual void release();

    void initialize();
    void enableShadowRender(Bool enable) { m_enabled = enable; }
    void setPosition(const Coord3D &pos) { m_position = pos; }
    void setAngle(Real angle) { m_angle = angle; }

    Bool m_enabled;
    Bool m_invisible;
    Coord3D m_position;
    Coord3D m_direction;
    Real m_angle;
    Int m_color;
    Int m_diffuse;
    Int m_opacity;
    Bool m_unknown30;
    Int m_type;
    UnsignedInt m_fadeWords[8];
};
typedef char ShadowBaseSize[(sizeof(ShadowBase) == 0x58) ? 1 : -1];

class Shadow : public ShadowBase
{
public:
    struct ShadowTypeInfo
    {
        ShadowTypeInfo();
        ~ShadowTypeInfo();
        AsciiString m_texture;
        AsciiString m_texture2;
        Int m_type;
        Real m_sizeX, m_sizeY;
        Real m_offsetX, m_offsetY;
        Real m_unknown1C;
        Real m_scale;
        Bool m_unknown24;
        Bool m_allowUpdates;
        Bool m_allowWorldAlign;
    };

    Shadow();
    virtual ~Shadow();

    void setColor(Int color);
    void setOpacity(Int opacity);
    void setSize(Real width, Real height) { m_width = width; m_height = height; }

    Real m_width, m_height;
    Real m_scale;
    Bool m_largeRadius;
};
typedef char ShadowSize[(sizeof(Shadow) == 0x68) ? 1 : -1];
typedef char ShadowTypeInfoSize[(sizeof(Shadow::ShadowTypeInfo) == 0x28) ? 1 : -1];
class Xfer;
void XferShadowType(Xfer *xfer, Int *value); // 0x00305F7A
#endif
