#include "StdAfx.h"
#include "Marker.h"
#include "GamePlugin.h"

#include "Player.h"

CRYREGISTER_CLASS(CMarker)

void CMarker::Initialize()
{
	pHost = GetEntity()->GetComponent<CPlayer>();
}

void CMarker::ProcessEvent(SEntityEvent & event)
{
}

void CMarker::Update(SEntityUpdateContext * param)
{
}
