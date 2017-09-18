you need find file
Code\CryPlugins\CryDefaultEntities\Module\Legacy\Helpers\EntityFlowNode.cpp
and fix line in method void CEntityFlowNodeFactory::GetConfiguration( SFlowNodeConfig& config )from this
    config.nFlags |= EFLN_TARGET_ENTITY|EFLN_HIDE_UI;
to this
config.nFlags |= EFLN_TARGET_ENTITYbut after this fix you still need drag FG node first from side panel and only then assign proper entity to it

in this example DFPlayer component has exposed FGNode with this fix for CEntityFlowNodeFactory 