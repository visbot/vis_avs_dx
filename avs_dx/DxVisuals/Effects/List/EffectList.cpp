#include "stdafx.h"
#include "EffectList.h"
#include "SavedContext.hpp"

eBlendMode EffectList::blendin() const
{
	return (eBlendMode)( ( avs->mode >> 8 ) & 31 );
}

float EffectList::inblendval() const
{
	return div255 * avs->inblendval;
}

eBlendMode EffectList::blendout() const
{
	return (eBlendMode)( ( ( avs->mode >> 16 ) & 31 ) ^ 1 );
}

float EffectList::outblendval() const
{
	return div255 * avs->outblendval;
}

HRESULT EffectList::updateParameters( Binder& binder )
{
	BoolHr hr;
	if( Blender::modeUsesShader( blendin() ) )
		hr.combine( m_blendIn.updateBindings( binder ) );

	hr.combine( __super::updateParameters( binder ) );

	if( Blender::modeUsesShader( blendout() ) )
		hr.combine( m_blendOut.updateBindings( binder ) );

	return hr;
}

HRESULT EffectList::render( bool isBeat, RenderTargets& rt )
{
	if( isBeat && avs->beat_render )
		avs->fake_enabled = avs->beat_render_frames;

	const int enabled = ( ( avs->mode & 2 ) ^ 2 ) || avs->fake_enabled > 0;

	if( !enabled )
		return S_FALSE;

	if( avs->beat_render && avs->fake_enabled > 0 )
		avs->fake_enabled--;

	SavedBlendMode __context;

	const eBlendMode blendIn = blendin();
	const bool clearBuffer = clearfb();
	if( blendIn == eBlendMode::Buffer )
	{
		CHECK( m_blendIn.blend( StaticResources::globalBuffers[ avs->bufferin ], m_rt, eBlendMode::Replace, inblendval() ) );
	}
	else
	{
		if( clearBuffer && blendIn != eBlendMode::Replace && m_rt.lastWritten() )
			m_rt.lastWritten().clear();

		if( !clearBuffer && blendIn == eBlendMode::Ignore )
		{
			CHECK( fadeRenderTarget( m_rt ) );
		}
		else
			CHECK( m_blendIn.blend( rt, m_rt, blendIn, inblendval() ) );
	}

	CHECK( EffectListBase::render( isBeat, m_rt ) );

	const eBlendMode blendOut = blendout();
	if( blendOut == eBlendMode::Buffer )
	{
		CHECK( m_blendOut.blend( m_rt, StaticResources::globalBuffers[ avs->bufferout ], eBlendMode::Replace, outblendval() ) );
	}
	else
	{
		CHECK( m_blendOut.blend( m_rt, rt, blendout(), outblendval() ) );
	}

	return S_OK;
}