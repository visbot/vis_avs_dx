#pragma once
#include "../../EffectImpl.hpp"
#include "../PointSpritesRender.hpp"
using namespace Hlsl::Render::Starfield;

struct StarfieldStructs: public PointSpritesRender
{
	struct StarFormat
	{
		int X, Y;
		float Z;
		float Speed;
		int OX, OY;
	};

	struct AvsState
	{
		int enabled;
		int color;
		int MaxStars, MaxStars_set;
		int Xoff;
		int Yoff;
		int Zoff;
		float WarpSpeed;
		int blend;
		int blendavg;
		StarFormat Stars[ 4096 ];
		int Width, Height;
		int onbeat;
		float spdBeat;
		float incBeat;
		int durFrames;
		float CurrentSpeed;
		int nc;
	};

	struct StateData: public StarfieldState
	{
		UINT stateSize() { return 3; }

		StateData( const AvsState& s )
		{
			onbeat = s.onbeat ? true : false;
			spdBeat = s.spdBeat;
			WarpSpeed = s.WarpSpeed;
			durFrames = s.durFrames;
		}
	};

	using CsData = StarCS;

	class VsData : public StarVS
	{
		int m_color = -1;
	public:
		HRESULT update( const AvsState& ass, UINT stateOffset );
	};

	static const CAtlMap<CStringA, CStringA>& effectIncludes() { return includes(); }
};

class Starfield : public EffectBase1<StarfieldStructs>
{
	StructureBuffer starsBuffer;

public:

	Starfield( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT initializedState() override;

	HRESULT render( RenderTargets& rt ) override;
};