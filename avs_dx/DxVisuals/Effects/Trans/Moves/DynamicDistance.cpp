#include "stdafx.h"
#include "DynamicDistance.h"
#include <../InteropLib/interop.h>

IMPLEMENT_EFFECT( DynamicDistance, C_PulseClass )
using namespace Expressions;

namespace
{
	class Proto : public Prototype
	{
	public:
		Proto()
		{
			addBeatConstant( "b" );
			addFragmentOutput( "d" );
		}
	};
	static const Prototype& prototype()
	{
		static const Proto p;
		return p;
	}
}

DynamicDistanceStructs::StateData::StateData( AvsState& ass ) :
	CommonStateData( "DynamicDistance", prototype() )
{ }

DynamicDistance::DynamicDistance( AvsState *pState ) :
	tBase( pState )
{ }

HRESULT DynamicDistance::render( bool isBeat, RenderTargets& rt )
{
	if( !renderer.bindShaders( isBeat ) )
		return S_FALSE;
	const UINT psReadSlot = renderer.pixel().bindPrevFrame;
	return MovementFx::render( rt, avs->subpixel, false, psReadSlot, avs->blend, false );
}